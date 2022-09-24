#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "lvgl.h"
#include "bsp_board.h"
#include "bsp_lcd.h"
#include "indev/indev.h"
#include "lvgl_port.h"

static char *TAG = "lvgl_port";
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static TaskHandle_t task = NULL;
static SemaphoreHandle_t sem_lock = NULL;

static void display_init(lvgl_port_config_t *config);
static void tick_init(uint8_t period);
static void lvgl_task(void *arg);

void lvgl_sem_take(void)
{
    if (xTaskGetCurrentTaskHandle() != task) {
        xSemaphoreTake(sem_lock, portMAX_DELAY);
    }
}

void lvgl_sem_give(void)
{
    if (xTaskGetCurrentTaskHandle() != task) {
        xSemaphoreGive(sem_lock);
    }
}

static void encoder_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    static int32_t last_v = 0;
    indev_data_t invd;
    indev_get_major_value(&invd);

    data->enc_diff = invd.encoder_value - last_v;
    data->state = invd.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    last_v = invd.encoder_value;
    printf("diff=%d s=%d\n", data->enc_diff, data->state);
}

static void indev_init(void)
{
    indev_init_default();
    /*Register a encoder input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    lv_indev_t *indev_encoder = lv_indev_drv_register(&indev_drv);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_encoder, group);`*/

}

void lvgl_port(lvgl_port_config_t *config)
{
    lv_init();
    display_init(config);
    indev_init();
    tick_init(config->tick_period);

    sem_lock = xSemaphoreCreateBinary();
    xSemaphoreGive(sem_lock);
    xTaskCreatePinnedToCore(
        lvgl_task, "lvgl", 4096, (void *)config->task.period, config->task.priority,
        &task, config->task.core_id
    );
    ESP_LOGI(TAG, "Finish init");
}

static void flush_cb(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    bsp_lcd_flush(area->x1, area->y1, area->x2 + 1, area->y2 + 1, (const void *)color_p);
}

static bool trans_done_cb(void *args)
{
    lv_disp_flush_ready(&disp_drv);
    return 0;
}

static void display_init(lvgl_port_config_t *config)
{
    static lv_disp_draw_buf_t disp_buf;
    lv_color_t *buf_1 = (lv_color_t *)heap_caps_malloc(config->display.buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, config->display.buf_size);

    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = flush_cb;
    disp_drv.hor_res = config->display.width;
    disp_drv.ver_res = config->display.height;
    lv_disp_drv_register(&disp_drv);
    bsp_lcd_set_cb(trans_done_cb, NULL);
}

static void tick_inc(void *arg)
{
    uint8_t period = (uint8_t)arg;
    lv_tick_inc(period);
}

static void tick_init(uint8_t period)
{
    esp_timer_handle_t timer;
    esp_timer_create_args_t args = {
        .name = "lvgl_tick",
        .callback = tick_inc,
        .dispatch_method = ESP_TIMER_TASK,
        .skip_unhandled_events = true,
        .arg = (void *)period,
    };
    ESP_ERROR_CHECK(esp_timer_create(&args, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, period * 1000));
}

static void lvgl_task(void *arg)
{
    uint8_t period = (uint8_t)arg;
    for (;;) {
        xSemaphoreTake(sem_lock, portMAX_DELAY);
        lv_timer_handler();
        xSemaphoreGive(sem_lock);
        vTaskDelay(pdMS_TO_TICKS(period));
    }
}
