#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ENCODER_EVENT_INC,
    ENCODER_EVENT_DEC,
    ENCODER_EVENT_MAX,
} encoder_event_t;

typedef void (* encoder_cb_t)(void *);

esp_err_t encoder_init(int gpio_a, int gpio_b);
int32_t encoder_get_value(void);
esp_err_t encoder_register_callback(encoder_event_t event, encoder_cb_t cb, void *user_data);

#ifdef __cplusplus
}
#endif
