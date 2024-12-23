#define DT_DRV_COMPAT plantower_pmsa003i

#include <errno.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/logging/log.h>

#include "pmsa003i.h"

// Enable logging at a given level
LOG_MODULE_REGISTER(PMSA003I, CONFIG_SENSOR_LOG_LEVEL);

static int pmsa003i_init(const struct device *dev);
static uint8_t pmsa003i_calculate_checksum(uint8_t *buffer);
static int pmsa003i_sample_fetch(const struct device *dev,
								enum sensor_channel chan);
static int pmsa003i_channel_get(const struct device *dev,
							   enum sensor_channel chan,
							   struct sensor_value *val);

// Initialize the PMSA003I
static int pmsa003i_init(const struct device *dev)
{
	const struct pmsa003i_config *cfg = dev->config;
	int ret = 0;

	LOG_DBG("Initializing PMSA003I");

	if (!device_is_ready(cfg->i2c.bus)) {
		LOG_ERR("Bus device is not ready");
		return -ENODEV;
	}

	return ret;
}

static uint8_t pmsa003i_calculate_checksum(uint8_t *buf) {
    uint16_t sum = 0;
    for (int i = 0; i < 30; i++) {
        sum += buf[i];
    }
    return (uint8_t)sum;
}

static int pmsa003i_read_with_timeout(const struct pmsa003i_config *cfg, uint8_t *buf, size_t len) {
    int64_t start_time = k_uptime_get();
    int64_t timeout_ms = 1000;

    while (k_uptime_get() - start_time < timeout_ms) {
        int ret = i2c_read_dt(&cfg->i2c, buf, len);
        if (ret < 0) {
            return ret;
        }

        if (buf[0] == PMSA003I_REG_START_BYTE_1 &&
            buf[1] == PMSA003I_REG_START_BYTE_2) {
            return 0;
        }

        k_sleep(K_MSEC(10));
    }

    return -ETIMEDOUT;
}
//------------------------------------------------------------------------------
// Public functions (API)

// Read temperature value from the device and store it in the device data struct
// Call this before calling mcp9808_channel_get()
static int pmsa003i_sample_fetch(const struct device *dev,
								enum sensor_channel chan)
{
	const struct pmsa003i_config *cfg = dev->config;
	struct pmsa003i_data *drv_data = dev->data;

	uint8_t pmsa003i_read_buffer[32] = {0};
	uint8_t calculated_checksum = 0;
	int ret = 0;

	/* sample output */
	/* 42 4D 00 1C 00 01 00 01 00 01 00 01 00 01 00 01 01 92
	 * 00 4E 00 03 00 00 00 00 00 00 71 00 02 06
	 */

    ret = pmsa003i_read_with_timeout(cfg, pmsa003i_read_buffer, sizeof(pmsa003i_read_buffer));
    if (ret < 0) {
        LOG_WRN("Failed to read valid data within timeout (err: %d)", ret);
        return ret;
    }

	// Check checksum
    calculated_checksum = pmsa003i_calculate_checksum(pmsa003i_read_buffer);
    if (calculated_checksum != pmsa003i_read_buffer[31]) {
        LOG_WRN("checksum mismatch (calc: 0x%02X, recv: 0x%02X)",
                calculated_checksum, pmsa003i_read_buffer[31]);
        return -EIO;
    }

	drv_data->pm_1_0 =
	    (pmsa003i_read_buffer[4] << 8) + pmsa003i_read_buffer[5];
	drv_data->pm_2_5 =
	    (pmsa003i_read_buffer[6] << 8) + pmsa003i_read_buffer[7];
	drv_data->pm_10 =
	    (pmsa003i_read_buffer[8] << 8) + pmsa003i_read_buffer[9];

	LOG_DBG("pm1.0 = %d", drv_data->pm_1_0);
	LOG_DBG("pm2.5 = %d", drv_data->pm_2_5);
	LOG_DBG("pm10 = %d", drv_data->pm_10);

	return ret;
}

// Get the PM1.0, PM2.5, or PM10 value stored in the device data struct
// Make sure to call pmsa003i_sample_fetch() to update the device data
static int pmsa003i_channel_get(const struct device *dev,
							   enum sensor_channel chan,
			    			   struct sensor_value *val)
{
	const struct pmsa003i_data *data = dev->data;

    switch (chan) {
        case SENSOR_CHAN_PM_1_0:
            val->val1 = data->pm_1_0;
            val->val2 = 0;
            break;
        case SENSOR_CHAN_PM_2_5:
            val->val1 = data->pm_2_5;
            val->val2 = 0;
            break;
        case SENSOR_CHAN_PM_10:
            val->val1 = data->pm_10;
            val->val2 = 0;
            break;
        default:
            LOG_ERR("Unsupported channel: %d", chan);
            return -ENOTSUP;
    }

	return 0;
}

//------------------------------------------------------------------------------
// Devicetree handling - This is the magic that connects this driver source code
// 	to the Devicetree so that you can use it in your application!

// Define the public API functions for the driver
static const struct sensor_driver_api pmsa003i_api_funcs = {
	.sample_fetch = pmsa003i_sample_fetch,
	.channel_get = pmsa003i_channel_get,
};

// Expansion macro to define the driver instances
// If inst is set to "42" by the Devicetree compiler, this macro creates code
// with the unique id of "42" for the structs, e.g. mcp9808_data_42.
#define PMSA003I_DEFINE(inst)                                        		   \
																			   \
	/* Create an instance of the data struct */								   \
	static struct pmsa003i_data pmsa003i_data_##inst;                 		   \
                                                                    		   \
	/* Create an instance of the config struct and populate with DT values */  \
	static const struct pmsa003i_config pmsa003i_config_##inst = {			   \
		.i2c = I2C_DT_SPEC_INST_GET(inst),                          		   \
	};        																   \
                                                                    		   \
	/* Create a "device" instance from a Devicetree node identifier and */	   \
	/* registers the init function to run during boot. */					   \
	SENSOR_DEVICE_DT_INST_DEFINE(inst, 										   \
								 pmsa003i_init, 								   \
								 NULL, 										   \
								 &pmsa003i_data_##inst,						   \
				     			 &pmsa003i_config_##inst, 					   \
								 POST_KERNEL,                       		   \
				     			 CONFIG_SENSOR_INIT_PRIORITY, 				   \
								 &pmsa003i_api_funcs);						   \

// The Devicetree build process calls this to create an instance of structs for
// each device (PMSA003I) defined in the Devicetree Source (DTS)
DT_INST_FOREACH_STATUS_OKAY(PMSA003I_DEFINE)
