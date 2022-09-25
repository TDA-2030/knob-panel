/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "bsp_board.h"
#include "driver/gpio.h"
#include "lvgl_port.h"
#include "esp_log.h"
#include "ui/ui.h"

static const char *TAG = "main";

void app_main(void)
{
    bsp_board_init();
    lvgl_port_config_t lvgl_config = {
        .display = {
            .width = 240,
            .height = 240,
            .buf_size = 240 * 40,
        },
        .tick_period = 2,
        .task = {
            .core_id = -1,
            .period = 5,
            .priority = 1,
        },
    };
    lvgl_port(&lvgl_config);

    lvgl_sem_take();
    ui_init();
    lvgl_sem_give();
}
