#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_btn.h"
#include "indev/encoder.h"
#endif
#include "lvgl.h"
#include "lvgl_port.h"
#include "ui.h"
#include "ui_menu.h"
#include "ui_clock.h"
#include "ui_light.h"
#include "ui_player.h"
#include "ui_weather.h"

static const char *TAG = "ui";
static lv_group_t *group;

struct ui_funcs {
    void (*create)(void);
    void (*delete)(void);
};

struct ui_funcs apps[] = {
    {ui_weather_init, ui_weather_delete},
    {ui_player_init, ui_player_delete},
    {ui_clock_init, ui_clock_delete},
    {ui_light_init, ui_light_delete},
};

static int8_t app_index, app_index_last;


static void encoder_change_cb(void *args)
{
    int dir = (int)args;
    uint8_t app_num = sizeof(apps) / sizeof(struct ui_funcs);
    printf("change %d\n", dir);

    app_index += dir;
    if (app_index >= app_num) {
        app_index = 0;
    }
    if (app_index < 0) {
        app_index = app_num - 1;
    }
    lvgl_sem_take();
    if (app_index_last >= 0) {
        apps[app_index_last].delete();
    }
    apps[app_index].create();
    lvgl_sem_give();
    app_index_last = app_index;
}


void ui_init(void)
{
    group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_t *indev = lv_indev_get_next(NULL);
    if (LV_INDEV_TYPE_ENCODER == lv_indev_get_type(indev)) {
        printf("add group for encoder\n");
        lv_indev_set_group(indev, group);
        lv_group_focus_freeze(group, false);
    }

    
    ui_menu_init();

//     app_index = 2;
//     apps[app_index].create();
//     app_index_last = app_index;
// #ifdef ESP_IDF_VERSION
//     encoder_register_callback(ENCODER_EVENT_INC, encoder_change_cb, (void *) -1);
//     encoder_register_callback(ENCODER_EVENT_DEC, encoder_change_cb, (void *)1);
// #endif
}

void ui_add_obj_to_encoder_group(lv_obj_t *obj)
{
    lv_group_add_obj(group, obj);
}
