
#include "scd30/scd30.h"
#include "scd30/device.h"

#include "stdio.h"

// Internal read/write commands
static int write_command(struct scd30_s *device, uint16_t command, uint16_t* data);
static int read_command(struct scd30_s *device, uint16_t command, uint8_t* data, uint8_t data_len);

// Internal CRC8 implementation
static uint8_t crc8(uint8_t* data, uint8_t length);

// Internal function to convert returned data to floats
static int convert(uint8_t *data, float *value);

// Initialise the SCD30 device
int scd30_init(struct scd30_s *device, uint8_t address, const scd30_driver_t *driver, void* driver_ctx) {
	device->address = address;

	// Store driver and context
	device->driver = driver;
	device->driver_ctx = driver_ctx;

	// Read back firmware version
	uint16_t firmware_version = 0;
	int res = scd30_firmware_version(device, &firmware_version);
	if(res < 0) {
		printf("SCD30: init, failed to read firmware version\r\n");
		return res;
	}

	printf("SCD30: init, version: %.2x", firmware_version);

	return 0;
}

// Set the measurement interval (for continuous sampling)
int scd30_set_measurement_interval(struct scd30_s *device, uint16_t interval_s){
	return write_command(device, SetMeasurementInterval, &interval_s);
}

// Start continuous mode
int scd30_start_continuous(struct scd30_s *device, uint16_t pressure_comp){
	return write_command(device, StartContinuousMode, &pressure_comp);
}

// Stop continuous mode
int scd30_stop_continuous(struct scd30_s *device) {
	return write_command(device, StopContinuousMode, NULL);
}

// Enable or disable automatic recalibration
int scd30_set_afc(struct scd30_s *device, bool enabled) {
	uint16_t v = enabled ? 1 : 0;
	return write_command(device, SetAfc, &v);
}

// Set forced recalibration with reference CO2 PPM
int scd30_set_frc(struct scd30_s *device, uint16_t cal_ppm) {
	return write_command(device, SetFrc, &cal_ppm);
}

// Set temperature offset to compensate for temperature variation
int scd30_set_temp_offset(struct scd30_s *device, float temp) {
	uint16_t t =  (uint16_t) (temp * 100);
	return write_command(device, SetTempOffset, &t);
}

// Set altitude offset to adjust CO2 measurement for altitude
int scd30_set_alt_offset(struct scd30_s *device, uint16_t alt) {
	return write_command(device, SetAltComp, &alt);
}

// Soft reset the device
int scd30_soft_reset(struct scd30_s *device) {
	return write_command(device, SoftReset, NULL);
}

// Fetch the firmware version
int scd30_firmware_version(struct scd30_s *device, uint16_t *version) {
	return read_command(device, GetFirmwareVersion, (uint8_t*)version, sizeof(version));
}

// Check for data ready
int scd30_data_ready(struct scd30_s *device) {
	uint8_t data[3];

	// Read ready command
	int res = read_command(device, GetDataReady, data, sizeof(data));
	if (res < 0) {
		return res;
	}

	// Check RX data CRC
	uint8_t crc = crc8(data, 2);
	if (crc != data[2]) {
		return -1;
	}

	// Check ready flag
	if (data[1] != 0) {
		return 1;
	}

	return 0;
}

// Read measurements from the SCD30
int scd30_get_measurement(struct scd30_s *device, float *co2, float *temp, float *humid) {
	uint8_t data[18];

	int res = read_command(device, ReadMeasurement, data, sizeof(data));
	if (res < 0) {
		return res;
	}

	res = convert(&data[0], co2);
	if (res < 0) {
		return res;
	}

	res = convert(&data[6], temp);
	if (res < 0) {
		return res;
	}

	res = convert(&data[12], humid);
	if (res < 0) {
		return res;
	}

	return 0;
}

// Close the SCD30
int scd30_close(struct scd30_s *device) {
	int res = scd30_stop_continuous(device);
	if (res < 0) {
		printf("SDC close, error stopping continuous mode\r\n");
	}

	device->driver = NULL;
	device->driver_ctx = NULL;

	return 0;
}


static int convert(uint8_t *data, float *value) {
	// Check first CRC
	uint8_t crc1 = crc8(&data[0], 2);
	if(crc1 != data[2]) {
		return -1;
	}
	// Check second CRC
	uint8_t crc2 = crc8(&data[3], 2);
	if(crc2 != data[5]) {
		return -1;
	}
	// Build float data from BE representation
	uint32_t u = (((uint32_t)data[0]) << 24) | (((uint32_t)data[1]) << 16) | (((uint32_t)data[3]) << 8) | (((uint32_t)data[4]) << 0);

	// Cast data to float and set
	*value = *(float*)&u;

	return 0;
}

// Internal device write command
static int write_command(struct scd30_s *device, uint16_t command, uint16_t* data) {
	uint8_t buff[5] = { command >> 8, command & 0xFF, 0, 0, 0};
	uint8_t len = 2;

	// Set data if non-null
	if (data != NULL) {
		buff[2] = *data >> 8;
		buff[3] = *data & 0xFF;
		buff[4] = crc8(&buff[2], 2);
		
		len = 5;
	}

	int res = device->driver->i2c_write(device->driver_ctx, device->address | I2C_WRITE_FLAG, buff, len);
	if (res < 0) {
		return res;
	}

	return 0;
}

// Internal device read command
static int read_command(struct scd30_s *device, uint16_t command, uint8_t* buff, uint8_t buff_len) {
	uint8_t data[2] = { command >> 8, command & 0xFF};

	// Write read command 
	int res = device->driver->i2c_write(device->driver_ctx, device->address | I2C_WRITE_FLAG, data, sizeof(data));
	if (res < 0) {
		return res;
	}

	// Read data and return result
	res = device->driver->i2c_read(device->driver_ctx, device->address | I2C_READ_FLAG, buff, buff_len);
	if (res < 0) {
		return res;
	}

	return 0;
}



static uint8_t crc8(uint8_t* data, uint8_t length) {
    uint8_t crc = CRC_INIT;

    for(int i=0; i<length; i++) {
		crc ^= data[i];

		for(int b=0; b<8; b++) {
			if((crc & 0x80) != 0) {
			crc = (crc << 1) ^ CRC_POLY;
			} else {
			crc = crc << 1;
			}
		}
    }

    return crc ^ CRC_XOR;
}

