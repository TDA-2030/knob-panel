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

static const char *TAG = "ui";
static lv_group_t *group;

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
}

void ui_add_obj_to_encoder_group(lv_obj_t *obj)
{
    lv_group_add_obj(group, obj);
}
