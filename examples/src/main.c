#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <stdio.h>

int main(void)
{
	const struct device *const pmsa = DEVICE_DT_GET(DT_NODELABEL(pmsa003i));
	int ret;

    // Check if the PMSA003I present
	if (pmsa == NULL) {
		printf("PMSA003I not found.\n");
		return 0;
	}

    // Check if the PMSA003I is ready
	if (!device_is_ready(pmsa)) {
		printf("Device %s is not ready.\n", pmsa->name);
		return 0;
	}

    while (1)
    {
        struct sensor_value pm1_0, pm2_5, pm10;

        // Fetch the PM1.0, PM2.5, and PM10 values from the sensor into the device's data structure
        ret = sensor_sample_fetch(pmsa);
        if (ret != 0) {
            printf("Sample fetch error: %d\n", ret);
            return 0;
        }

        // Copy the PM1.0 value from the device's data structure into the pm1_0 struct
        ret = sensor_channel_get(pmsa, SENSOR_CHAN_PM_1_0, &pm1_0);
        if (ret != 0) {
            printf("Channel get error: %d\n", ret);
            return 0;
        }

        // Copy the PM2.5 value from the device's data structure into the pm2_5 struct
        ret = sensor_channel_get(pmsa, SENSOR_CHAN_PM_2_5, &pm2_5);
        if (ret != 0) {
            printf("Channel get error: %d\n", ret);
            return 0;
        }

        // Copy the PM10 value from the device's data structure into the pm10 struct
        ret = sensor_channel_get(pmsa, SENSOR_CHAN_PM_10, &pm10);
        if (ret != 0) {
            printf("Channel get error: %d\n", ret);
            return 0;
        }

        // Print the PM1.0 value
        printf("PM1.0: %d\n", pm1_0.val1);
        printf("PM2.5: %d\n", pm2_5.val1);
        printf("PM10: %d\n", pm10.val1);

        // Sleep for 5 seconds
        k_sleep(K_SECONDS(5));
    }

    return 0;
}