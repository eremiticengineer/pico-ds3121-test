#include <stdio.h>
#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <string>
#include <fmt/base.h>

#include "DS3231.h"

// Standard Task priority
#define DS3231_TASK_PRIORITY (tskIDLE_PRIORITY + 2UL)

std::string dateTime;
SemaphoreHandle_t i2c_mutex;

void ds3231_task(void* pvParameters) {
    DS3231 *pDS3231 = static_cast<DS3231 *>(pvParameters);

    while (true) {
        struct tm time;
        if (xSemaphoreTake(i2c_mutex, portMAX_DELAY)) {
          if (pDS3231->readTime(time)) {
              printf("Date: %02d/%02d/%04d Time: %02d:%02d:%02d\n",
                time.tm_mday, time.tm_mon + 1, time.tm_year + 1900,
                time.tm_hour, time.tm_min, time.tm_sec);

              dateTime = fmt::format("{:02d}/{:02d}/{:04d} {:02d}:{:02d}:{:02d}",
                time.tm_mday, time.tm_mon + 1, time.tm_year + 1900,
                time.tm_hour, time.tm_min, time.tm_sec);

              printf("%s\n", dateTime.c_str());
          }
          else {
              printf("Failed to read time\n");
          }
          xSemaphoreGive(i2c_mutex);
        }
        vTaskDelay(1000);
    }
}

// void ds3231_setup_task(void* pvParameters) {
//     DS3231 *pDS3231 = static_cast<DS3231 *>(pvParameters);

//     struct tm buildTime = {
//         .tm_sec = 0,
//         .tm_min = 30,
//         .tm_hour = 14,
//         .tm_mday = 25,
//         .tm_mon = 12 - 1,        // Months since January, so January = 0
//         .tm_year = 2026 - 1900,  // Years since 1900
//         .tm_wday = 2,            // Optional: 0 = Sunday ... 6 = Saturday
//     };

//     if (pDS3231->setTime(buildTime)) {
//         printf("RTC time set to %04d-%02d-%02d %02d:%02d:%02d\n",
//                buildTime.tm_year + 1900, buildTime.tm_mon + 1, buildTime.tm_mday,
//                buildTime.tm_hour, buildTime.tm_min, buildTime.tm_sec);
//     }
//     else {
//         printf("Failed to set RTC time\n");
//     }

//     vTaskDelete(NULL); // Self-terminate
// }

int main(void) {
    stdio_init_all();

    int sda_pin = 8;
    int scl_pin = 9;
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    i2c_mutex = xSemaphoreCreateMutex();
    DS3231 ds3231(i2c0);
    ds3231.init();

    //xTaskCreate(ds3231_setup_task, "RTC Setup", 1024, (void*)&ds3231, tskIDLE_PRIORITY + 2, nullptr);
    xTaskCreate(ds3231_task, "DS3231 Task", 1024, (void*)&ds3231, DS3231_TASK_PRIORITY, nullptr);

    vTaskStartScheduler();

    return 0;
}
