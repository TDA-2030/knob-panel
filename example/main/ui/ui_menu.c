#include "lvgl.h"
#include <stdio.h>
#include "esp_system.h"
#ifdef ESP_IDF_VERSION
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_btn.h"
#endif
#include "ui.h"

static lv_obj_t *page;


#define APP_NUM 5//(sizeof(menu) / sizeof(ui_menu_app_t))
#define APP_ICON_GAP_PIXEL (80)

typedef struct {
    const char *name;
    const lv_img_dsc_t *icon;
    void (*create)(void);
    void (*delete)(void);
} ui_menu_app_t;

LV_IMG_DECLARE(icon_clock);
LV_IMG_DECLARE(icon_fans);
LV_IMG_DECLARE(icon_light);
LV_IMG_DECLARE(icon_player);
LV_IMG_DECLARE(icon_weather);

static ui_menu_app_t menu[] = {
    {"clock", &icon_clock, NULL, NULL},
    {"clock", &icon_fans, NULL, NULL},
    {"clock", &icon_light, NULL, NULL},
    {"clock", &icon_player, NULL, NULL},
    {"clock", &icon_weather, NULL, NULL},
    // {"clock", &icon_weather, NULL, NULL},
    // {"clock", &icon_weather, NULL, NULL},
};

static uint8_t app_index = 0;

#define ICONS_SHOW_NUM 3
static lv_obj_t *icons[ICONS_SHOW_NUM + 1];
static lv_coord_t old_y[ICONS_SHOW_NUM + 1];
static uint8_t visible_index[ICONS_SHOW_NUM];
static uint8_t invisable_index;

static void menu_anim_ready_cb(lv_anim_t *a);

static uint32_t get_num_offset(uint32_t num, int32_t max, int32_t offset)
{
    if (num >= max) {
        printf("[ERROR] num should less than max\n");
        return num;
    }

    uint32_t i;
    if (offset >= 0) {
        i = (num + offset) % max;
    } else {
        offset = max + (offset % max);
        i = (num + offset) % max;
    }

    return i;
}

static uint32_t get_app_index(int8_t offset)
{
    return get_num_offset(app_index, APP_NUM, offset);
}

static void menu_anim_exec_cb(void *args, int32_t v)
{
    int8_t extra_icon_index = (int8_t)args;

    for (int i = 0; i < ICONS_SHOW_NUM + 1; i++) {
        if (extra_icon_index > 0) {
            lv_obj_set_y(icons[i], old_y[i] - v);
        } else {
            lv_obj_set_y(icons[i], old_y[i] + v);
        }
        int32_t abs_y = LV_ABS(lv_obj_get_y_aligned(icons[i]));
        if (abs_y < 130) {
            lv_img_set_zoom(icons[i], 256 * (130 - abs_y) / 100);
        }
    }
}

static void menu_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    printf("evt=%d\n", code);
    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    }

    if (LV_EVENT_KEY == code) {

        uint32_t key = lv_event_get_key(e);
        int8_t  extra_icon_index = 0;
        if (LV_KEY_RIGHT == key) {
            extra_icon_index = (ICONS_SHOW_NUM / 2) + 1;
        } else if (LV_KEY_LEFT == key) {
            extra_icon_index = -(ICONS_SHOW_NUM / 2) - 1;
        }

        lv_img_set_src(icons[invisable_index], menu[get_app_index(extra_icon_index)].icon);
        lv_obj_align(icons[invisable_index], LV_ALIGN_CENTER, 0, (extra_icon_index)* APP_ICON_GAP_PIXEL);
        lv_img_set_zoom(icons[invisable_index], 1);

        for (size_t i = 0; i < ICONS_SHOW_NUM + 1; i++) {
            old_y[i] = lv_obj_get_y_aligned(icons[i]);
        }

        lv_obj_remove_event_cb(page, menu_event_cb);

        lv_anim_t a1;
        lv_anim_init(&a1);
        lv_anim_set_var(&a1, (void *)extra_icon_index);
        lv_anim_set_values(&a1, 0, APP_ICON_GAP_PIXEL);
        lv_anim_set_exec_cb(&a1, menu_anim_exec_cb);
        lv_anim_set_path_cb(&a1, lv_anim_path_ease_in_out);
        lv_anim_set_ready_cb(&a1, menu_anim_ready_cb);
        lv_anim_set_time(&a1, 400);
        lv_anim_set_user_data(&a1, (void *)extra_icon_index);
        lv_anim_start(&a1);
    }
}

static void menu_anim_ready_cb(lv_anim_t *a)
{
    printf("%s\n", __FUNCTION__);
    int8_t extra_icon_index = (int8_t)lv_anim_get_user_data(a);
    int8_t dir = extra_icon_index > 0 ? 1 : -1;
    app_index = get_app_index(dir);
    invisable_index = get_num_offset(invisable_index, ICONS_SHOW_NUM + 1, dir);
    for (size_t i = 0; i < ICONS_SHOW_NUM; i++) {
        visible_index[i] = get_num_offset(visible_index[i], ICONS_SHOW_NUM + 1, dir);
    }
    printf("dir=%d, app_index=%d, invisable_index=%d\n", dir, app_index, invisable_index);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
}

void ui_menu_init(void)
{
    if (page) {
        LV_LOG_WARN("menu page already created");
        return;
    }

    page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, lv_obj_get_width(lv_obj_get_parent(page)), lv_obj_get_height(lv_obj_get_parent(page)));
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_radius(page, 0, 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(page);
    lv_obj_refr_size(page);

    lv_obj_t *image_bg = lv_img_create(page);
    LV_IMG_DECLARE(img_bg);
    lv_img_set_src(image_bg, &img_bg);
    lv_obj_align(image_bg, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_img_opa(image_bg, LV_OPA_60, 0);

    for (int i = 0; i < ICONS_SHOW_NUM; i++) {
        visible_index[i] = i;
        icons[visible_index[i]] = lv_img_create(image_bg);
        lv_img_set_src(icons[visible_index[i]], menu[get_app_index(i - (ICONS_SHOW_NUM / 2))].icon);
        lv_obj_align(icons[visible_index[i]], LV_ALIGN_CENTER, 0, (i - (ICONS_SHOW_NUM / 2)) * APP_ICON_GAP_PIXEL);
        int32_t abs_y = LV_ABS(lv_obj_get_y_aligned(icons[visible_index[i]]));
        if (abs_y < 130) {
            lv_img_set_zoom(icons[visible_index[i]], 256 * (130 - abs_y) / 100);
        } else {
            lv_img_set_zoom(icons[visible_index[i]], 1);
        }
    }
    invisable_index = ICONS_SHOW_NUM;
    icons[invisable_index] = lv_img_create(image_bg);
    lv_obj_set_size(icons[invisable_index], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(icons[invisable_index], LV_ALIGN_CENTER, 0, lv_obj_get_height(lv_obj_get_parent(image_bg)));


    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, menu_event_cb, LV_EVENT_KEY, NULL);
    ui_add_obj_to_encoder_group(page);



    // lv_obj_t *label_city = lv_label_create(img);
    // lv_label_set_text(label_city, "Shang hai");
    // lv_obj_set_style_text_font(label_city, &lv_font_montserrat_20, 0);
    // lv_obj_set_size(label_city, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_style_text_align(label_city, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label_city, LV_ALIGN_CENTER, 0, -70);

    // lv_obj_t *label_temperature = lv_label_create(img);
    // lv_label_set_text(label_temperature, "24℃");
    // LV_FONT_DECLARE(font_cn_48);
    // lv_obj_set_style_text_font(label_temperature, &font_cn_48, 0);
    // lv_obj_set_size(label_temperature, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_style_text_align(label_temperature, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align_to(label_temperature, label_city, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

    // LV_IMG_DECLARE(img_cloudy);
    // lv_obj_t *img_icon = lv_img_create(img);
    // lv_img_set_src(img_icon, &img_cloudy);
    // lv_obj_align_to(img_icon, label_temperature, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    // lv_obj_t *label_state = lv_label_create(img);
    // lv_label_set_text_fmt(label_state, "Mostly sunny\nMin:%02d℃ Max:%02d℃", 22, 28);
    // LV_FONT_DECLARE(font_cn_12);
    // lv_obj_set_style_text_font(label_state, &font_cn_12, 0);
    // lv_obj_set_width(label_state, 150);
    // lv_obj_set_style_text_align(label_state, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align_to(label_state, img_icon, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
}



