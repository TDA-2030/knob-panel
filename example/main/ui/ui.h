#ifndef UI_UI_H__
#define UI_UI_H__

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void ui_init(void);
void ui_add_obj_to_encoder_group(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif
