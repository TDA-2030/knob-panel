
#include <stdio.h>
#include <time.h>
#include "lvgl.h"
#include "ui.h"
#include "ui_washing.h"

static lv_obj_t  *page;
static ret_cb_t return_callback;


static void washing_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        uint32_t key = lv_event_get_key(e);

    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_washing_delete();
    }
}

static void bub1_anim_cb(void *args, int32_t v)
{
    lv_obj_t *img_bub = (lv_obj_t *)args;
    lv_coord_t y = lv_obj_get_y_aligned(img_bub);
    int opa = 0;
    if (y < -80) {
        opa = 255 * (120 + y) / 80;
    }
    printf("opa=%d\n", opa);
    lv_obj_set_style_img_opa(img_bub, opa, 0);
    lv_obj_set_y(img_bub, v);
}

static void bub2_anim_cb(void *args, int32_t v)
{
    lv_obj_t *img_bub = (lv_obj_t *)args;
    // lv_obj_set_style_img_opa(img_bub, 255 * v / 120, 0);
    lv_obj_set_y(img_bub, v);
}


void ui_washing_init(ret_cb_t ret_cb)
{
    if (page) {
        LV_LOG_WARN("washing page already created");
        return;
    }

    return_callback = ret_cb;

    page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, lv_obj_get_width(lv_obj_get_parent(page)), lv_obj_get_height(lv_obj_get_parent(page)));
    lv_obj_set_style_border_width(page, 5, 0);
    lv_obj_set_style_radius(page, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(page, lv_color_make(20, 20, 20), 0);
    lv_obj_set_style_border_color(page, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(page);
    lv_obj_refr_size(page);

    LV_IMG_DECLARE(img_washing_bg);
    LV_IMG_DECLARE(img_washing_wave);
    LV_IMG_DECLARE(img_washing_bubble1);
    LV_IMG_DECLARE(img_washing_bubble2);

    lv_obj_t *win = lv_img_create(page);
    lv_img_set_src(win, &img_washing_bg);
    // lv_obj_set_size(win, lv_obj_get_width(page) / 2, lv_obj_get_width(page) / 2);
    // lv_obj_set_style_border_width(win, 2, 0);
    // lv_obj_set_style_radius(win, LV_RADIUS_CIRCLE, 0);
    // lv_obj_set_style_bg_color(win, lv_color_make(20, 20, 20), 0);
    // lv_obj_set_style_border_color(win, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
    // lv_obj_clear_flag(win, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(win, page, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t *img_wave = lv_img_create(win);
    lv_img_set_src(img_wave, &img_washing_wave);
    lv_obj_center(img_wave);

    lv_obj_t *img_bub1 = lv_img_create(win);
    lv_img_set_src(img_bub1, &img_washing_bubble1);
    lv_obj_center(img_bub1);
    lv_obj_t *img_bub2 = lv_img_create(win);
    lv_img_set_src(img_bub2, &img_washing_bubble2);
    lv_obj_center(img_bub2);

    // lv_obj_t *arc = lv_arc_create(page);
    // lv_obj_set_size(arc, lv_obj_get_width(page) - 15, lv_obj_get_height(page) - 15);
    // lv_arc_set_rotation(arc, 180);
    // lv_arc_set_bg_angles(arc, 0, 180);
    // // lv_arc_set_angles(arc, 0, 30);
    // lv_arc_set_value(arc, 30);
    // // lv_arc_set_range(arc, 0, 100);
    // lv_obj_set_style_arc_width(arc, 20, LV_PART_MAIN);
    // lv_obj_set_style_arc_width(arc, 20, LV_PART_INDICATOR);
    // lv_obj_set_style_arc_color(arc, lv_color_make(60, 60, 60), LV_PART_MAIN);
    // lv_obj_set_style_arc_color(arc, lv_color_make(20, 70, 200), LV_PART_INDICATOR);
    // lv_obj_set_style_outline_width(arc, 2, LV_STATE_FOCUSED | LV_PART_KNOB);
    // lv_obj_set_style_outline_color(arc, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_STATE_FOCUSED | LV_PART_KNOB);
    // lv_obj_set_style_outline_color(arc, lv_palette_main(LV_PALETTE_YELLOW), LV_STATE_EDITED | LV_PART_KNOB);
    // lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    // // lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    // lv_obj_center(arc);

    lv_obj_t *label1 = lv_label_create(page);
    // LV_FONT_DECLARE(font_cn_32);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_16, 0);
    lv_label_set_text(label1, "- 45 min -");
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 60, 20);

    // lv_obj_t *label2 = lv_label_create(page);
    // lv_label_set_text(label2, "%");
    // lv_obj_set_style_text_font(label2, &lv_font_montserrat_20, 0);
    // lv_obj_set_width(label2, 50);
    // lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label2, LV_ALIGN_CENTER, 40, -10);

    // lv_obj_t *label3 = lv_label_create(page);
    // lv_label_set_text_fmt(label3, "%d", lv_arc_get_value(arc));
    // lv_obj_set_style_text_font(label3, &lv_font_montserrat_40, 0);
    // lv_obj_set_width(label3, 150);  /*Set smaller width to make the lines wrap*/
    // lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0);

    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, img_bub1);
    lv_anim_set_delay(&a1, 0);
    lv_anim_set_values(&a1, lv_obj_get_y_aligned(img_bub1), lv_obj_get_y_aligned(img_bub1) - 120);
    lv_anim_set_exec_cb(&a1, bub1_anim_cb);
    lv_anim_set_path_cb(&a1, lv_anim_path_ease_in_out);
    lv_anim_set_time(&a1, 3000);
    lv_anim_set_repeat_count(&a1, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a1);

    lv_anim_t a2;
    lv_anim_init(&a2);
    lv_anim_set_var(&a2, img_bub2);
    lv_anim_set_delay(&a2, 0);
    lv_anim_set_values(&a2, lv_obj_get_y_aligned(img_bub2), lv_obj_get_y_aligned(img_bub2) - 120);
    lv_anim_set_exec_cb(&a2, bub2_anim_cb);
    lv_anim_set_path_cb(&a2, lv_anim_path_ease_in_out);
    lv_anim_set_time(&a2, 3000);
    lv_anim_set_repeat_count(&a2, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a2);

    lv_obj_add_event_cb(page, washing_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, washing_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}

void ui_washing_delete(void)
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
