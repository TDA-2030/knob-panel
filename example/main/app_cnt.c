#include <stdio.h>
#include "app_cnt.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define GPIO_CNT_A 6
#define GPIO_CNT_B 7
#define GPIO_PIN (1ull << GPIO_CNT_A)|(1ull << GPIO_CNT_B)

static xQueueHandle gpioEventQueue = NULL;

static size_t EC11_Value = 0; // 获取旋转的值

static  size_t    EC11_A_Last = 0;                        //EC11的A引脚上一次的状态
static  size_t    EC11_B_Last = 0; 

static size_t    EC11_A_Now = 1;
static size_t    EC11_B_Now = 1;

/*********************************

* @函数名：EC11_Display

* @描  述：判断方向，计算旋转的值

* @参  数：无

* @返回值：无

*********************************/

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

typedef struct cnt_step_config
{
    size_t adjust_range ;
    size_t per_time ; //10ms
    float Weight_one ;
    float Weight_two ;
    float Weight_three ;
}cnt_step_config_t;

static const char *TAG = "CNT";

static void gpioTaskExample(void *arg){
    uint32_t ioNum = (uint32_t) arg;
    pha_value = gpio_get_level(GPIO_CNT_A);
    phb_value = gpio_get_level(GPIO_CNT_B);
    pha_value_old = pha_value;
    phb_value_old = phb_value;
    while (1)
    {
        if (xQueueReceive(gpioEventQueue, &ioNum, portMAX_DELAY)){
            if(ioNum == GPIO_CNT_A){
                pha_value = gpio_get_level(GPIO_CNT_A);
                if(pha_value != pha_value_old){
                    printf("GPIO[%d] intrrupted, level: %d\n", ioNum, gpio_get_level(ioNum));
                    pha_value_old = pha_value;
                    pha_value_change = 1;
                    if(phb_value_change != 1){
                        if(pha_value_old != phb_value_old){
                            dir = 1;
                        }else{
                            dir = -1;
                        }
                    }
                    EC11_Value += dir;
                    ESP_LOGI(TAG,"%d,time is %lld ",EC11_Value,diff_us);
                }
            } else {
                phb_value = gpio_get_level(GPIO_CNT_B);
                if(phb_value != phb_value_old){
                    printf("GPIO[%d] intrrupted, level: %d\n", ioNum, gpio_get_level(ioNum));
                    phb_value_old = phb_value;
                    phb_value_change = 1;
                    if(pha_value_change != 1){
                        if(pha_value_old != phb_value_old){
                            dir = -1;
                        }else{
                            dir = 1;
                        }
                    }
                    EC11_Value += dir;
                    ESP_LOGI(TAG,"%d,time is %lld ",EC11_Value,diff_us);
                }
            }
            if(pha_value_change == 1 && phb_value_change == 1 ){
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

static void IRAM_ATTR intrHandler (void *arg){
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpioEventQueue, &gpio_num, NULL);
}

void gpio_isr_init()
{
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = GPIO_PIN,
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE,
        .pull_up_en = 1,
    };
    gpio_config(&gpio_cfg);
    gpioEventQueue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpioTaskExample, "ExampleTask", 2048, NULL, 10, NULL);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_CNT_A, intrHandler, (void*)GPIO_CNT_A);
    gpio_isr_handler_add(GPIO_CNT_B, intrHandler, (void*)GPIO_CNT_B);
}