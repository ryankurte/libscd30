
#include "scd30/scd30.h"
#include "scd30/device.h"

// Internal read/write commands
static int write_command(struct scd30_s *device, uint16_t command, uint16_t* data);
static int read_command(struct scd30_s *device, uint16_t command, uint8_t* data, uint8_t data_len);

// Initialise the SCD30 device
int scd30_init(struct scd30_s *device, struct scd30_driver_s *driver, void* driver_ctx) {

}

// Set the measurement interval (for continuous sampling)
int scd30_set_measurement_interval(struct scd30_s *device, uint16_t interval_s){

}

// Start continuous mode
int scd30_start_continuous(struct scd30_s *device, uint16_t pressure_comp){

}

// Stop continuous mode
int scd30_stop_continuous(struct scd30_s *device) {

}

// Enable or disable automatic recalibration
int scd30_set_afc(struct scd30_s *device, bool enabled) {

}

// Set forced recalibration with reference CO2 PPM
int scd30_set_frc(struct scd30_s *device, uint16_t cal_ppm) {

}

// Set temperature offset to compensate for temperature variation
int scd30_set_temp_offset(struct scd30_s *device, float temp) {

}

// Set altitude offset to adjust CO2 measurement for altitude
int scd30_set_alt_offset(struct scd30_s *device, uint16_t alt) {

}

// Soft reset the device
int scd30_soft_reset(struct scd30_s *device) {

}

// Fetch the firmware version
int scd30_firmware_version(struct scd30_s *device, uint16_t *version) {

}

// Check for data ready
int scd30_data_ready(struct scd30_s *device) {

}

// Read measurements from the SCD30
int scd30_read(struct scd30_s *device, float *co2, float *temp, float *humid) {

}

// Close the SCD30
int scd30_close(struct scd30_s *device) {

}

// Internal device write command
static int write_command(struct scd30_s *device, uint16_t command, uint16_t* data) {

}

// Internal device read command
static int read_command(struct scd30_s *device, uint16_t command, uint8_t* data, uint8_t data_len) {

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

