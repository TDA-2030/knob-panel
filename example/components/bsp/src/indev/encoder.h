#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t encoder_init(int gpio_a, int gpio_b);
int32_t encoder_get_value(void);

#ifdef __cplusplus
}
#endif
