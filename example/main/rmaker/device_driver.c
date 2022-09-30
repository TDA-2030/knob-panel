/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <esp_log.h>
#include <sdkconfig.h>
#include <string.h>
#include "esp_check.h"
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_params.h>
#include <esp_rmaker_standard_types.h>

#include "app_led.h"
#include "device_driver.h"
#include "driver/gpio.h"
#include "rmaker_devices.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"
// #include "app_sr.h"
// #include "app_fan.h"
// #include "app_switch.h"

static const char *TAG = "esp_box_driver";



esp_err_t app_driver_light_init(char *unique_name, int gpio_r, int gpio_g, int gpio_b, int h, int s, int v, int power, char *voice)
{
    return ESP_OK;
}

esp_err_t app_driver_set_light_color(char *unique_name, int h, int s, int v)
{
    ESP_LOGI(TAG, "Request to set the color of %s to h:%d s:%d v:%d", unique_name, h, s, v);
    return ESP_OK;
}

esp_err_t app_driver_set_light_power(char *unique_name, bool status)
{
    ESP_LOGI(TAG, "Request to set the power of %s to %d", unique_name, status);
    return ESP_OK;
}

esp_err_t app_driver_set_light_gpio(char *unique_name, int r, int g, int b)
{
    ESP_LOGI(TAG, "Request to set the GPIO of %s to r:%d g:%d b:%d,", unique_name, r, g, b);
    return ESP_OK;
}

esp_err_t app_driver_set_light_voice(char *unique_name, char *voice)
{
    ESP_LOGI(TAG, "Request to set the voice instruction of %s to %s", unique_name, voice);
    return ESP_OK;
}

esp_err_t app_driver_report_light_status(char *unique_name, bool status, int h, int s, int v)
{
    esp_rmaker_param_update_and_report(esp_rmaker_device_get_param_by_name(
                                           esp_rmaker_node_get_device_by_name(esp_rmaker_get_node(), unique_name), ESP_RMAKER_DEF_POWER_NAME), esp_rmaker_bool(status));

    esp_rmaker_param_update_and_report(esp_rmaker_device_get_param_by_name(
                                           esp_rmaker_node_get_device_by_name(esp_rmaker_get_node(), unique_name), ESP_RMAKER_DEF_HUE_NAME), esp_rmaker_int(h));

    esp_rmaker_param_update_and_report(esp_rmaker_device_get_param_by_name(
                                           esp_rmaker_node_get_device_by_name(esp_rmaker_get_node(), unique_name), ESP_RMAKER_DEF_SATURATION_NAME), esp_rmaker_int(s));

    esp_rmaker_param_update_and_report(esp_rmaker_device_get_param_by_name(
                                           esp_rmaker_node_get_device_by_name(esp_rmaker_get_node(), unique_name), ESP_RMAKER_DEF_BRIGHTNESS_NAME), esp_rmaker_int(v));
    return ESP_OK;
}

esp_err_t app_driver_fan_init(char *unique_name, int gpio, int power, char *voice)
{
    return ESP_OK;
}

esp_err_t app_driver_set_fan_power(char *unique_name, bool status)
{
    return ESP_OK;
}

esp_err_t app_driver_set_fan_gpio(char *unique_name, int gpio, int active_level)
{
    ESP_LOGI(TAG, "Request to set the gpio of %s to %d", unique_name, gpio);
    return ESP_OK;
}

esp_err_t app_driver_set_fan_voice(char *unique_name, char *voice)
{
    ESP_LOGI(TAG, "Request to set the voice instruction of %s to %s, but the driver does not support it", unique_name, voice);

    // app_sr_update_voice_cmd(unique_name, voice);

    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t app_driver_report_fan_status(char *unique_name, bool status)
{
    esp_rmaker_param_update_and_report(esp_rmaker_device_get_param_by_name(
                                           esp_rmaker_node_get_device_by_name(esp_rmaker_get_node(), unique_name), ESP_RMAKER_DEF_POWER_NAME), esp_rmaker_bool(status));
    return ESP_OK;
}

esp_err_t app_driver_switch_init(char *unique_name, int gpio, int active_level, int power, char *voice)
{
    return ESP_OK;
}

esp_err_t app_driver_set_switch_power(char *unique_name, bool status)
{
    return ESP_OK;
}

esp_err_t app_driver_set_switch_gpio(char *unique_name, int gpio, int active_level)
{
    ESP_LOGI(TAG, "Request to set the gpio of %s to %d, active_level: %d", unique_name, gpio, active_level);
    return ESP_OK;
}

esp_err_t app_driver_set_switch_voice(char *unique_name, char *voice)
{
    ESP_LOGI(TAG, "Request to set the voice instruction of %s to %s, but the driver does not support it", unique_name, voice);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t app_driver_report_switch_status(char *unique_name, bool status)
{
    esp_rmaker_param_update_and_report(esp_rmaker_device_get_param_by_name(
                                           esp_rmaker_node_get_device_by_name(esp_rmaker_get_node(), unique_name), ESP_RMAKER_DEF_POWER_NAME), esp_rmaker_bool(status));
    return ESP_OK;
}
