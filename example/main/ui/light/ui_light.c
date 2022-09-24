#include "lvgl.h"
#include <stdio.h>

static lv_obj_t *arc;
static lv_obj_t *img;

static void brightness_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text_fmt(label, "%d", lv_arc_get_value(obj));
        lv_obj_set_style_img_opa(img, lv_arc_get_value(obj) * 255 / 100, 0);
    }
}

static void scroll_begin_event(lv_event_t * e)
{
    /*Disable the scroll animations. Triggered when a tab button is clicked */
    if(lv_event_get_code(e) == LV_EVENT_SCROLL_BEGIN) {
        lv_anim_t * a = lv_event_get_param(e);
        if(a)  a->time = 0;
    }
}

void ui_light_init(void)
{
    lv_obj_t *page = lv_scr_act();

    lv_group_t * group = lv_group_create();
    lv_indev_t * indev = lv_indev_get_next(NULL);
    lv_indev_type_t indev_type = lv_indev_get_type(indev);
    lv_indev_set_group(indev, group);

    /*Create a Tab view object*/
    lv_obj_t * tabview;
    tabview = lv_tabview_create(page, LV_DIR_TOP, 0);
    
    // lv_obj_add_event_cb(lv_tabview_get_content(tabview), scroll_begin_event, LV_EVENT_SCROLL_BEGIN, NULL);

    // lv_obj_set_style_bg_color(tabview, lv_palette_darken(LV_PALETTE_RED, 1), 0);

    lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tabview);
    // lv_obj_set_style_bg_color(tab_btns, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    // lv_obj_set_style_text_color(tab_btns, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    // lv_obj_set_style_border_side(tab_btns, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);

    /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");lv_group_add_obj(group, tab_btns);
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");//lv_group_add_obj(group, tab2);
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");//lv_group_add_obj(group, tab3);
    lv_obj_clear_flag(tab1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(tab2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_clear_flag(tab3, LV_OBJ_FLAG_SCROLLABLE);

    /**
     * Tab 1 for brightness
     */
    arc = lv_arc_create(tab1);
    lv_obj_set_size(arc, lv_obj_get_width(tab1) - 15, lv_obj_get_height(tab1) - 15);
    lv_arc_set_rotation(arc, 180);
    lv_arc_set_bg_angles(arc, 0, 180);
    // lv_arc_set_angles(arc, 0, 30);
    lv_arc_set_value(arc, 30);
    // lv_arc_set_range(arc, 0, 100);
    lv_obj_set_style_arc_width(arc, 20, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 20, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_make(60, 60, 60), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_make(200, 150, 20), LV_PART_INDICATOR);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);   /*Be sure the knob is not displayed*/
    // lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);  /*To not allow adjusting by click*/
    lv_obj_center(arc);

    lv_obj_t *label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "value");
    lv_obj_set_style_text_font(label1, &lv_font_simsun_16_cjk, 0);
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, -70);

    lv_obj_t *label2 = lv_label_create(tab1);
    lv_label_set_text(label2, "%");
    lv_obj_set_style_text_font(label2, &lv_font_montserrat_20, 0);
    lv_obj_set_width(label2, 50);
    lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label2, LV_ALIGN_CENTER, 40, -10);

    lv_obj_t *label3 = lv_label_create(tab1);
    lv_label_set_text_fmt(label3, "%d", lv_arc_get_value(arc));
    lv_obj_set_style_text_font(label3, &lv_font_montserrat_40, 0);
    lv_obj_set_width(label3, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label3, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label3, LV_ALIGN_CENTER, 0, 0);

    LV_IMG_DECLARE(light_brightness);
    img = lv_img_create(tab1);
    lv_img_set_src(img, &light_brightness);
    lv_obj_align_to(img, label3, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_bg_color(img, lv_color_make(200, 0, 0), 0);
    lv_obj_set_style_img_opa(img, lv_arc_get_value(arc) * 255 / 100, 0);

    lv_obj_add_event_cb(arc, brightness_event_cb, LV_EVENT_VALUE_CHANGED, label3);

    /**
     * Tab 2 for colorwheel
     */
    lv_obj_t * cw;
    cw = lv_colorwheel_create(tab2, true);
    lv_obj_set_size(cw, 200, 200);
    lv_obj_center(cw);

    /**
     * Tab 3 for colorwheel
     */
    lv_obj_t * label = lv_label_create(tab3);
    lv_label_set_text(label, "First tab");
}

void ui_light_set_brightness(uint8_t value)
{
    lv_arc_set_value(arc, value);
}

