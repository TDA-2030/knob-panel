
#include <stdio.h>
#include <time.h>
#include "lvgl.h"

static lv_obj_t  *page, *meter = NULL;
static lv_meter_indicator_t *indic_sec ;
static lv_meter_indicator_t *indic_min ;
static lv_meter_indicator_t *indic_hour ;
static lv_timer_t *timer;

static void clock_handler(lv_timer_t *t)
{
    static time_t time_last = 0;
    time_t now;
    struct tm timeinfo = {0};
    time(&now);
    if (now != time_last) {
        time_last = now;
        localtime_r(&now, &timeinfo);
        printf("time=%d:%d:%d\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
        lv_meter_set_indicator_end_value(meter, indic_sec, timeinfo.tm_sec);
        lv_meter_set_indicator_end_value(meter, indic_min, timeinfo.tm_min);
        if (timeinfo.tm_hour > 12) {
            timeinfo.tm_hour -= 12;
        }
        if (timeinfo.tm_hour + 9 > 12) { // for scale rotate 300 degree
            lv_meter_set_indicator_end_value(meter, indic_hour, timeinfo.tm_hour + 9 - 12);
        } else {
            lv_meter_set_indicator_end_value(meter, indic_hour, timeinfo.tm_hour + 9);
        }
    }
}

void ui_clock_init(void)
{
    if (page) {
        LV_LOG_WARN("clock page already created");
        return;
    }

    page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, lv_obj_get_width(lv_obj_get_parent(page)), lv_obj_get_height(lv_obj_get_parent(page)));
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_radius(page, 0, 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_center(page);

    meter = lv_meter_create(page);
    lv_obj_set_size(meter, 220, 220);
    lv_obj_center(meter);
    lv_obj_set_style_border_color(meter, lv_color_make(50, 50, 50), 0);
    lv_obj_set_style_bg_color(meter, lv_color_make(210, 150, 150), 0);
    lv_obj_remove_style(meter, NULL, LV_PART_TICKS);
    lv_obj_remove_style(meter, NULL, LV_PART_ITEMS);
    /*Create a scale for the minutes*/
    /*61 ticks in a 360 degrees range (the last and the first line overlaps)*/
    lv_meter_scale_t *scale_min = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_min, 61, 1, 10, lv_color_make(200, 200, 200));
    lv_meter_set_scale_range(meter, scale_min, 0, 60, 360, 180);
    /*Create another scale for the hours. It's only visual and contains only major ticks*/
    lv_meter_scale_t *scale_hour = lv_meter_add_scale(meter);
    lv_meter_set_scale_ticks(meter, scale_hour, 12, 0, 0, lv_color_white()); /*12 ticks*/
    lv_meter_set_scale_major_ticks(meter, scale_hour, 1, 2, 20, lv_color_white(), 10); /*Every tick is major*/
    lv_meter_set_scale_range(meter, scale_hour, 1, 12, 330, 300); /*[1..12] values in an almost full circle*/
    LV_IMG_DECLARE(img_needle_hour);
    LV_IMG_DECLARE(img_needle_min);
    LV_IMG_DECLARE(img_needle_sec);
    /*Add a the hands from images*/
    indic_hour = lv_meter_add_needle_img(meter, scale_hour, &img_needle_hour, 5, 15);
    indic_min = lv_meter_add_needle_img(meter, scale_min, &img_needle_min, 4, 15);
    indic_sec = lv_meter_add_needle_img(meter, scale_min, &img_needle_sec, 5, 15); // second needle on the top

    timer = lv_timer_create(clock_handler, 200, NULL);
    clock_handler(timer);
}

void ui_clock_delete(void)
{
    if (page) {
        lv_timer_del(timer);
        lv_obj_del(page);
        page = NULL;
    }
}
