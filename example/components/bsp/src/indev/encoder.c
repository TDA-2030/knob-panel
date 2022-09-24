#include <stdio.h>
#include "encoder.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "Encoder";

static xQueueHandle gpioEventQueue = NULL;

static int32_t EC11_Value = 0;

static int GPIO_CNT_A;
static int GPIO_CNT_B;
static bool pha_value = 0;
static bool phb_value = 0;
static bool pha_value_old = 0;
static bool phb_value_old = 0;
static bool pha_value_change = 0;
static bool phb_value_change = 0;
static int64_t begin_us = 0;
static int64_t end_us = 0;
static int64_t diff_us = 0;
static size_t dir = 0;// -1 -> left 1-> right

static void gpioTaskExample(void *arg)
{
    uint32_t ioNum = (uint32_t) arg;
    pha_value = gpio_get_level(GPIO_CNT_A);
    phb_value = gpio_get_level(GPIO_CNT_B);
    pha_value_old = pha_value;
    phb_value_old = phb_value;
    while (1) {
        if (xQueueReceive(gpioEventQueue, &ioNum, portMAX_DELAY)) {
            if (ioNum == GPIO_CNT_A) {
                pha_value = gpio_get_level(GPIO_CNT_A);
                if (pha_value != pha_value_old) {
                    pha_value_old = pha_value;
                    pha_value_change = 1;
                    if (phb_value_change != 1) {
                        if (pha_value_old != phb_value_old) {
                            dir = 1;
                        } else {
                            dir = -1;
                        }
                    }
                    EC11_Value += dir;
                    ESP_LOGI(TAG, "%d,time is %lld ", EC11_Value, diff_us);
                }
            } else {
                phb_value = gpio_get_level(GPIO_CNT_B);
                if (phb_value != phb_value_old) {
                    phb_value_old = phb_value;
                    phb_value_change = 1;
                    if (pha_value_change != 1) {
                        if (pha_value_old != phb_value_old) {
                            dir = -1;
                        } else {
                            dir = 1;
                        }
                    }
                    EC11_Value += dir;
                    ESP_LOGI(TAG, "%d,time is %lld ", EC11_Value, diff_us);
                }
            }
            if (pha_value_change == 1 && phb_value_change == 1 ) {
                /* 计算每次旋钮变动变动的速度 未完成 */
                // end_us = esp_timer_get_time();
                // diff_us = end_us - begin_us;
                // end_us = 0;
                // begin_us = 0;
                // ESP_LOGI(TAG,"%d,time is %lld ",EC11_Value,diff_us);
                pha_value_change = 0;
                phb_value_change = 0;
                dir = 0;
            }
        }
    }
}

static void IRAM_ATTR intrHandler (void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpioEventQueue, &gpio_num, NULL);
}

esp_err_t encoder_init(int gpio_a, int gpio_b)
{
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(gpio_a) | BIT64(gpio_b),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE,
        .pull_up_en = 1,
    };
    gpio_config(&gpio_cfg);
    GPIO_CNT_A = gpio_a;
    GPIO_CNT_B = gpio_b;
    gpioEventQueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpioTaskExample, "encoder", 2048, NULL, 10, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_CNT_A, intrHandler, (void *)GPIO_CNT_A);
    gpio_isr_handler_add(GPIO_CNT_B, intrHandler, (void *)GPIO_CNT_B);
    return ESP_OK;
}

int32_t encoder_get_value(void)
{
    return EC11_Value;
}