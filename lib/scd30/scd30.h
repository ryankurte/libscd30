
#ifndef SCD30_H
#define SCD30_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// I2V interaction functions for dependency injection
typedef int (*i2c_write_f)(void* context, int address, int length_out, unsigned char *data_out);
typedef int (*i2c_read_f)(void* context, int address, int length_in, unsigned char *data_in);
typedef int (*i2c_write_read_f)(void* context, int address,
                                int length_out, unsigned char *data_out,
                                int length_in, unsigned char *data_in);

// SCD30 driver object
// This defines required platform methods to interact with the SCD30 device
struct scd30_driver_s {
    i2c_write_f i2c_write;
    i2c_read_f i2c_read;
    i2c_write_read_f i2c_write_read;
};

// SCD30 device object
struct scd30_s {
    int open;				// Indicates the device has been opened successfully
    struct scd30_driver_s driver;	// Driver function object
    void* driver_ctx;			// Driver context pointer
};


// Initialise an SCD30 device
int scd30_init(struct scd30_s *device, struct scd30_driver_s *driver, void* driver_ctx);

int scd30_set_measurement_interval(struct scd30_s *device, uint16_t interval_s);

int scd30_start_continuous(struct scd30_s *device, uint16_t pressure_comp);

int scd30_stop_continuous(struct scd30_s *device);

// Enable or disable automatic recalibration
int scd30_set_afc(struct scd30_s *device, bool enabled);

// Set forced recalibration with reference CO2 PPM
int scd30_set_frc(struct scd30_s *device, uint16_t cal_ppm);

// Set temperature offset to compensate for temperature variation
int scd30_set_temp_offset(struct scd30_s *device, float temp);

// Set altitude offset to adjust CO2 measurement for altitude
int scd30_set_alt_offset(struct scd30_s *device, uint16_t alt);

// Soft reset the device
int scd30_soft_reset(struct scd30_s *device);

// Fetch the firmware version
int scd30_firmware_version(struct scd30_s *device, uint16_t *version);

// Check for data ready
int scd30_data_ready(struct scd30_s *device);

// Read measurements from the SCD30
int scd30_read(struct scd30_s *device, float *co2, float *temp, float *humid);

// Close the SCD30
int scd30_close(struct scd30_s *device);

#endif

