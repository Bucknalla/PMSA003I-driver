#ifndef ZEPHYR_DRIVERS_SENSOR_PLANTOWER_PMSA003I_H_
#define ZEPHYR_DRIVERS_SENSOR_PLANTOWER_PMSA003I_H_

#include <zephyr/drivers/sensor.h>

// PMSA003I registers
#define PMSA003I_REG_CONFIG     0x00
#define PMSA003I_REG_PM_1_0     0x04
#define PMSA003I_REG_PM_2_5     0x06
#define PMSA003I_REG_PM_10      0x08

// PMSA003I start bytes
#define PMSA003I_REG_START_BYTE_1 0x42
#define PMSA003I_REG_START_BYTE_2 0x4d

#define CFG_PMSA003I_SERIAL_TIMEOUT 1000

// Sensor data
struct pmsa003i_data {
	uint16_t pm_1_0;
	uint16_t pm_2_5;
	uint16_t pm_10;
};

// Configuration data
struct pmsa003i_config {
	struct i2c_dt_spec i2c;
};

#endif /* ZEPHYR_DRIVERS_SENSOR_PLANTOWER_PMSA003I_H_ */
