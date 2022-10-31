#include "lvgl.h"
#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_btn.h"
#endif
#include "ui.h"
#include "ui_clock.h"
#include "ui_light.h"
#include "ui_player.h"
#include "ui_weather.h"
#include "ui_thermostat.h"
#include "ui_washing.h"

#if USE_NEW_MENU

typedef struct {
    const char *name;
    const lv_img_dsc_t *icon;
    const lv_img_dsc_t *icon_ns;
    lv_color_t theme_color;
    void (*create)(const ui_app_param_t *param);
} ui_menu_app_t;

LV_IMG_DECLARE(icon_clock);
LV_IMG_DECLARE(icon_fans);
LV_IMG_DECLARE(icon_thermostat);
LV_IMG_DECLARE(icon_thermostat_ns);
LV_IMG_DECLARE(icon_light);
LV_IMG_DECLARE(icon_light_ns);
LV_IMG_DECLARE(icon_player);
LV_IMG_DECLARE(icon_weather);
LV_IMG_DECLARE(icon_washing);
LV_IMG_DECLARE(icon_washing_ns);

static ui_menu_app_t menu[] = {
    {"Washing", &icon_washing, &icon_washing_ns, LV_COLOR_MAKE(36, 163, 235), ui_washing_init},
    {"Thermostat", &icon_thermostat, &icon_thermostat_ns, LV_COLOR_MAKE(249, 139, 122), ui_thermostat_init},
    {"Light", &icon_light, &icon_light_ns, LV_COLOR_MAKE(255, 229, 147), ui_light_init},
};

#define APP_NUM 3//(sizeof(menu) / sizeof(ui_menu_app_t))
static lv_obj_t *icons[APP_NUM];
static uint8_t app_index = 0;
static lv_obj_t *page;
static lv_obj_t *label1;

static uint32_t get_app_index(int8_t offset)
{
    return ui_get_num_offset(app_index, APP_NUM, offset);
}

static void arc_path_by_theta(int16_t theta, int16_t *x, int16_t *y)
{
    const int ox = 0, oy = 0;
    *x = ox + ((lv_trigo_sin(theta) * 45) >> LV_TRIGO_SHIFT);
    *y = oy + ((lv_trigo_sin(theta + 90) * 45) >> LV_TRIGO_SHIFT);
}

static void obj_set_to_hightlight(lv_obj_t *obj, bool enable)
{
    if (enable) {
        lv_obj_set_style_shadow_width(obj, 15, 0);
        lv_obj_set_style_shadow_spread(obj, 3, 0);
    } else {
        lv_obj_set_style_shadow_width(obj, 0, 0);
        lv_obj_set_style_shadow_spread(obj, 0, 0);
    }
}

static void app_return_cb(void *args)
{
    ui_add_obj_to_encoder_group(page);
}

static void menu_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        uint32_t key = lv_event_get_key(e);
        int8_t last_index = app_index;
        if (LV_KEY_RIGHT == key) {
            app_index = get_app_index(1);
        } else if (LV_KEY_LEFT == key) {
            app_index = get_app_index(-1);
        }
        for (int i = 0; i < APP_NUM; i++) {
            obj_set_to_hightlight(icons[i], i == app_index);
        }
        lv_obj_swap(icons[last_index], icons[get_app_index(0)]);
        lv_img_set_src(icons[last_index], menu[last_index].icon_ns);
        lv_img_set_src(icons[get_app_index(0)], menu[get_app_index(0)].icon);
        lv_obj_set_style_border_color(page, menu[get_app_index(0)].theme_color, 0);
        lv_label_set_text(label1, menu[get_app_index(0)].name);

    } else if (LV_EVENT_CLICKED == code) {
        lv_group_set_editing(lv_group_get_default(), false);
        ui_remove_all_objs_from_encoder_group();
        ui_app_param_t param = {
            .ret_cb = app_return_cb,
            .theme_color = menu[app_index].theme_color,
        };
        menu[get_app_index(0)].create(&param);
    }
}

void ui_menu_init(void)
{
    if (page) {
        LV_LOG_WARN("menu page already created");
        return;
    }

    page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, lv_obj_get_width(lv_obj_get_parent(page)), lv_obj_get_height(lv_obj_get_parent(page)));
    lv_obj_set_style_border_width(page, 5, 0);
    lv_obj_set_style_border_color(page, menu[get_app_index(0)].theme_color, 0);
    lv_obj_set_style_radius(page, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(page);
    lv_obj_refr_size(page);

    for (int i = APP_NUM - 1; i >= 0; i--) {
        int16_t x, y;
        arc_path_by_theta(180 + i * 120, &x, &y);
        icons[i] = lv_img_create(page);
        if (i == app_index) {
            lv_img_set_src(icons[i], menu[i].icon);
        } else {
            lv_img_set_src(icons[i], menu[i].icon_ns);
        }
        lv_obj_align(icons[i], LV_ALIGN_CENTER, x, y);
        lv_obj_set_style_border_color(icons[i], menu[i].theme_color, 0);
        lv_obj_set_style_shadow_color(icons[i], menu[i].theme_color, 0);
        lv_obj_set_style_border_width(icons[i], 2, 0);
        lv_obj_set_style_radius(icons[i], LV_RADIUS_CIRCLE, 0);
        obj_set_to_hightlight(icons[i], i == app_index);


    }

    label1 = lv_label_create(page);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_20, 0);
    lv_label_set_text(label1, menu[app_index].name);
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_BOTTOM_MID, 0, -6);

    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_CLICKED, NULL);
    ui_add_obj_to_encoder_group(page);
}


#endif
