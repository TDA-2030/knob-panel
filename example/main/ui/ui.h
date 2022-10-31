#ifndef UI_UI_H__
#define UI_UI_H__

#include "lvgl.h"

#define USE_NEW_MENU 1

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*ret_cb_t)(void *args);
typedef struct {
    ret_cb_t ret_cb;
    lv_color_t theme_color;
} ui_app_param_t;


void ui_init(void);
void ui_add_obj_to_encoder_group(lv_obj_t *obj);
void ui_remove_all_objs_from_encoder_group(void);
uint32_t ui_get_num_offset(uint32_t num, int32_t max, int32_t offset);

#ifdef __cplusplus
}
#endif

#endif
