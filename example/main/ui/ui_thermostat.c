
#include <stdio.h>
#include <time.h>
#include "lvgl.h"
#include "ui.h"
#include "ui_thermostat.h"

static lv_obj_t  *page;
static ret_cb_t return_callback;


static void thermostat_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    printf("evt=%d\n", code);
    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        uint32_t key = lv_event_get_key(e);
        printf("key=%d\n", key);

    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_thermostat_delete();
    }
}

static void temperature_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text_fmt(label, "%d", lv_arc_get_value(obj));
    }
}

void ui_thermostat_init(const ui_app_param_t *param)
{
    if (page) {
        LV_LOG_WARN("thermostat page already created");
        return;
    }

    return_callback = param->ret_cb;

    page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, lv_obj_get_width(lv_obj_get_parent(page)), lv_obj_get_height(lv_obj_get_parent(page)));
    lv_obj_set_style_border_width(page, 5, 0);
    lv_obj_set_style_border_color(page, param->theme_color, 0);
    lv_obj_set_style_radius(page, LV_RADIUS_CIRCLE, 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(page);
    lv_obj_refr_size(page);

    lv_obj_t *arc = lv_arc_create(page);
    lv_obj_set_size(arc, lv_obj_get_width(page) - 50, lv_obj_get_height(page) - 50);
    lv_arc_set_rotation(arc, 180 + 30);
    lv_arc_set_bg_angles(arc, 0, 120);
    lv_arc_set_value(arc, 21);
    lv_arc_set_range(arc, 16, 30);
    lv_obj_set_style_arc_width(arc, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 8, LV_PART_INDICATOR);
    lv_obj_set_style_pad_all(arc, 2, LV_PART_KNOB);
    lv_obj_set_style_arc_color(arc, lv_color_make(60, 60, 60), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_obj_get_style_border_color(page, 0), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arc, lv_color_make(255, 255, 255), LV_PART_KNOB);
    lv_obj_set_style_outline_width(arc, 2, LV_STATE_FOCUSED | LV_PART_KNOB);
    lv_obj_set_style_outline_color(arc, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_FOCUSED | LV_PART_KNOB);
    lv_obj_set_style_outline_color(arc, lv_palette_main(LV_PALETTE_YELLOW), LV_STATE_EDITED | LV_PART_KNOB);
    lv_obj_center(arc);

    lv_obj_t *label1 = lv_label_create(page);
    LV_FONT_DECLARE(font_cn_32);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_48, 0);
    lv_label_set_text(label1, "26");
    lv_obj_set_width(label1, 60);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label2 = lv_label_create(page);
    lv_label_set_text(label2, "°c");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_20, 0);
    lv_obj_set_width(label2, 50);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align_to(label2, label1, LV_ALIGN_OUT_RIGHT_TOP, -10, 0);

    lv_obj_add_event_cb(arc, temperature_event_cb, LV_EVENT_VALUE_CHANGED, label1);
    lv_obj_add_event_cb(page, thermostat_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, thermostat_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, thermostat_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, arc);
    lv_anim_set_values(&a1, lv_obj_get_y_aligned(arc) - 20, lv_obj_get_y_aligned(arc));
    lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a1, lv_anim_path_overshoot);
    lv_anim_set_time(&a1, 400);
    lv_anim_start(&a1);
    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, label1);
    lv_anim_set_values(&a2, lv_obj_get_y_aligned(label1) + 20, lv_obj_get_y_aligned(label1));
    lv_anim_set_exec_cb(&a2, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a2, lv_anim_path_overshoot);
    lv_anim_set_time(&a2, 400);
    lv_anim_start(&a2);
}

void ui_thermostat_delete(void)
{
    if (page) {
        ui_remove_all_objs_from_encoder_group();
        lv_obj_del(page);
        page = NULL;
        if (return_callback) {
            return_callback(NULL);
        }
    }
}
