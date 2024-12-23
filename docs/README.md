# PMSA003I - Documentation

## Overview

The PMSA003I is a PM sensor that uses the I2C protocol to communicate with the host.
It can be used to measure the PM1.0, PM2.5, and PM10 particle concentrations.

## Usage

The PMSA003I driver is designed to be used with the Zephyr Sensor API.

For example:

```c
int ret;
ret = sensor_sample_fetch(pmsa);
ret = sensor_channel_get(pmsa, SENSOR_CHAN_PM_1_0, &pm1_0);
...
```

An example application can be found in the [examples](../examples) directory. This is a simple application that fetches the PM1.0, PM2.5, and PM10 values from the sensor and prints them to the console. It is designed to be used with Blues Swan Feather MCU.

## Datasheet

The datasheet for the PMSA003I is available [here](datasheet.pdf).