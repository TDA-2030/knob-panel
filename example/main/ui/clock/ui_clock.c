#include "lvgl.h"
#include <stdio.h>

static lv_obj_t *meter = NULL;

static void set_value(void *indic, int32_t v);

static lv_meter_indicator_t *indic_sec ;
static lv_meter_indicator_t *indic_min ;
static lv_meter_indicator_t *indic_hour ;
static uint32_t min, hour;

void ui_clock_init(void)
{
    meter = lv_meter_create(lv_scr_act());
    lv_obj_set_size(meter, 220, 220);
    lv_obj_center(meter);

    lv_obj_remove_style(meter, NULL, LV_PART_TICKS);
    lv_obj_remove_style(meter, NULL, LV_PART_ITEMS);
    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_color_make(30, 30, 30));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 180);
    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_color_black()); /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_black(), 10); /*Every tick is major*/
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300); /*[1..12] values in an almost full circle*/
    LV_IMG_DECLARE(img_clock_hour);
    LV_IMG_DECLARE(img_clock_min);
    /*Add a the hands from images*/
    indic_sec = lv_meter_add_needle_img(meter, scale_min, &img_clock_min, 5, 5); //
    indic_min = lv_meter_add_needle_img(meter, scale_min, &img_clock_min, 5, 5); //
    indic_hour = lv_meter_add_needle_img(meter, scale_hour, &img_clock_hour, 5, 5);
    lv_meter_set_indicator_end_value(meter, indic_sec, 0);
    lv_meter_set_indicator_end_value(meter, indic_min, 0);
    lv_meter_set_indicator_end_value(meter, indic_hour, 9);
    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_time(&a, 60000); // 60 seconds to run a cycle
    lv_anim_set_var(&a, indic_sec);
    lv_anim_start(&a);
    min = 0;
    hour = 0;
}

static void set_value(void *indic, int32_t v)
{
    lv_meter_set_indicator_end_value(meter, indic, v); // 设置分针值
    if (v >= 60) {
        min++;
        if (min >= 60) {
            min = 0;
            hour++;
            if (hour >= 12) {
                hour = 0;
            }
            if (hour + 9 > 12) { // for scale rotate 300 degree
                lv_meter_set_indicator_end_value(meter, indic_hour, hour + 9 - 12);
            } else {
                lv_meter_set_indicator_end_value(meter, indic_hour, hour + 9);
            }
        }
        lv_meter_set_indicator_end_value(meter, indic_min, min);
    }
}
