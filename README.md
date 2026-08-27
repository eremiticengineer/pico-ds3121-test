# DS3231 Test

This is a simple project that sets and reads the date/time from a DS3231 RTC.

## Cloning the project

Clone the project with FreeRTOS and FreeRTOS-FAT-CLI-for-RPi-Pico submodules:

```
git clone --recurse-submodules https://github.com/eremiticengineer/pico-ds3121-test
```

If you cloned without recursing submodules:

```
git submodule update --init --recursive
```

## FreeRTOS-Kernal setup for new projects

When creating a FreeRTOS and fmt project from scratch, clone the main branches into the project:

```
git init
git submodule add https://github.com/FreeRTOS/FreeRTOS-Kernel.git lib/FreeRTOS-Kernel
git submodule update --init --recursive
git add .gitmodules lib/FreeRTOS-Kernel
git submodule add https://github.com/fmtlib/fmt lib/fmt
git submodule update --init --recursive
git add .gitmodules lib/fmt
```

## FreeRTOSConfig.h

This file customises FreeRTOS for your project. The file:

```
include/FreeRTOSConfig.h
```

is this one from the pico-examples:

```
pico-examples/freertos/FreeRTOSConfig_examples_common.h
```

## Setting the date/time

In ds3121test.cpp uncomment rtc_setup_task and comment out rtc_task:

```
xTaskCreate(rtc_setup_task, "RTC Setup", 1024, nullptr, tskIDLE_PRIORITY + 2, nullptr);

//xTaskCreate(rtc_task, "RTC Task", 1024, nullptr, tskIDLE_PRIORITY + 1, nullptr);
```

set the date/time in rtc_setup_task, for example:

```
Saturday 28/6/25 12:45:00

struct tm buildTime = {
    .tm_sec = 0,
    .tm_min = 45,
    .tm_hour = 12,
    .tm_mday = 28,
    .tm_mon = 6 - 1,        // Months since January, so January = 0
    .tm_year = 2025 - 1900, // Years since 1900
    .tm_wday = 6,           // Optional: 0 = Sunday ... 6 = Saturday
};
```

build and upload to the pico several minutes before 12:45:00 then unplug the pico. At 12:45:00, plug in the pico. Plugging in will set the date/time as per the **buildTime** variable.

Uncomment rtc_task and comment out rtc_setup_task:

```
//xTaskCreate(rtc_setup_task, "RTC Setup", 1024, nullptr, tskIDLE_PRIORITY + 2, nullptr);

xTaskCreate(rtc_task, "RTC Task", 1024, nullptr, tskIDLE_PRIORITY + 1, nullptr);
```

Unplug the pico and plug in with bootsel held down for programming. Build and upload to the pico. The rtc will stay on the correct time if the battery is in place.


## References

[Setting accurate time with DS3231.h library](https://forum.arduino.cc/t/setting-accurate-time-with-ds3231-h-library/627993/7)
