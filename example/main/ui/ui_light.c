#include "lvgl.h"
#include <stdio.h>
#include "ui.h"
#include "ui_light.h"

static const char *TAG = "ui light";

static lv_obj_t *page;
static lv_obj_t *label1;
static ret_cb_t return_callback;
static uint8_t brightness = 0;
static uint8_t ct = 0; //Color temperature

#define BNOG 5 //Number of grades for brightness

static void brightness_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
    }
}

static void light_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    printf("%s: evt=%d\n", TAG, code);
    if (LV_EVENT_FOCUSED == code) {
        lv_group_set_editing(lv_group_get_default(), true);
    } else if (LV_EVENT_KEY == code) {
        uint32_t key = lv_event_get_key(e);
        if (LV_KEY_RIGHT == key) {
            brightness = ui_get_num_offset(brightness, BNOG, 1);
        } else if (LV_KEY_LEFT == key) {
            brightness = ui_get_num_offset(brightness, BNOG, -1);
        }
        lv_label_set_text_fmt(label1, "%d%%", brightness * (100 / BNOG));

    } else if (LV_EVENT_LONG_PRESSED == code) {
        lv_indev_wait_release(lv_indev_get_next(NULL));
        ui_light_delete();
    }
}

void ui_light_init(const ui_app_param_t *param)
{
    if (page) {
        LV_LOG_WARN("light page already created");
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

    label1 = lv_label_create(page);
    LV_FONT_DECLARE(font_cn_32);
    lv_obj_set_style_text_font(label1, &lv_font_montserrat_32, 0);
    lv_label_set_text_fmt(label1, "%d%%", brightness * (100 / BNOG));
    lv_obj_set_width(label1, 150);  /*Set smaller width to make the lines wrap*/
    lv_obj_set_style_text_align(label1, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 60);

    LV_IMG_DECLARE(light_bottom1);
    lv_obj_t *img = lv_img_create(page);
    lv_img_set_src(img, &light_bottom1);
    lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);

    // lv_obj_add_event_cb(arc, brightness_event_cb, LV_EVENT_VALUE_CHANGED, label3);

#define CANVAS_WIDTH  200
#define CANVAS_HEIGHT  150

    {
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.radius = 10;
        rect_dsc.bg_opa = LV_OPA_COVER;
        rect_dsc.bg_grad.dir = LV_GRAD_DIR_HOR;
        rect_dsc.bg_grad.stops[0].color = lv_palette_main(LV_PALETTE_RED);
        rect_dsc.bg_grad.stops[1].color = lv_palette_main(LV_PALETTE_BLUE);
        rect_dsc.border_width = 2;
        rect_dsc.border_opa = LV_OPA_90;
        rect_dsc.border_color = lv_color_white();
        rect_dsc.shadow_width = 5;
        rect_dsc.shadow_ofs_x = 5;
        rect_dsc.shadow_ofs_y = 5;

        lv_draw_label_dsc_t label_dsc;
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = lv_palette_main(LV_PALETTE_ORANGE);

        static uint8_t cbuf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

        lv_obj_t *canvas = lv_canvas_create(page);
        lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
        lv_obj_center(canvas);
        lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);

        lv_canvas_draw_rect(canvas, 70, 60, 100, 70, &rect_dsc);

        lv_canvas_draw_text(canvas, 40, 20, 100, &label_dsc, "Some text on text canvas");

        /*Test the rotation. It requires another buffer where the original image is stored.
         *So copy the current image to buffer and rotate it to the canvas*/
        static uint8_t cbuf_tmp[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];
        memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
        lv_img_dsc_t img;
        img.data = (void *)cbuf_tmp;
        img.header.cf = LV_IMG_CF_TRUE_COLOR;
        img.header.w = CANVAS_WIDTH;
        img.header.h = CANVAS_HEIGHT;

        lv_canvas_fill_bg(canvas, lv_palette_lighten(LV_PALETTE_GREY, 3), LV_OPA_COVER);
        lv_canvas_transform(canvas, &img, 120, LV_IMG_ZOOM_NONE, 0, 0, CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, true);
    }


    lv_anim_t a1;
    lv_anim_init(&a1);
    lv_anim_set_var(&a1, label1);
    lv_anim_set_values(&a1, lv_obj_get_y_aligned(label1) + 20, lv_obj_get_y_aligned(label1));
    lv_anim_set_exec_cb(&a1, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_set_path_cb(&a1, lv_anim_path_overshoot);
    lv_anim_set_time(&a1, 300);
    lv_anim_start(&a1);
    // lv_anim_t a2;
    // lv_anim_init(&a2);
    // lv_anim_set_var(&a2, img);
    // lv_anim_set_values(&a2, lv_obj_get_y_aligned(img) + 20, lv_obj_get_y_aligned(img));
    // lv_anim_set_exec_cb(&a2, (lv_anim_exec_xcb_t)lv_obj_set_y);
    // lv_anim_set_path_cb(&a2, lv_anim_path_overshoot);
    // lv_anim_set_time(&a2, 400);
    // lv_anim_start(&a2);

    lv_obj_add_event_cb(page, light_event_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(page, light_event_cb, LV_EVENT_KEY, NULL);
    lv_obj_add_event_cb(page, light_event_cb, LV_EVENT_LONG_PRESSED, NULL);
    ui_add_obj_to_encoder_group(page);
}


void ui_light_delete(void)
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
