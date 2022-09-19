/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "esp_log.h"
#include "bsp_board.h"
#include "bsp_i2c.h"
#include "bsp_i2s.h"
#include "bsp_codec.h"
#include "esp32c3_knob_panel_v1.1.h"

static const char *TAG = "bsp boards";

static const boards_info_t g_boards_info[] = {
    {BOARD_ESP32C3_KNOB_PANEL,  "C3_KNOB_PANEL",   bsp_board_knob_panel_detect, bsp_board_knob_panel_init,  bsp_board_knob_panel_power_ctrl, bsp_board_knob_panel_get_res_desc},
};
static boards_info_t *g_board = NULL;

static esp_err_t bsp_board_detect(void)
{
    esp_err_t ret = ESP_ERR_NOT_FOUND;
    for (size_t i = 0; i < sizeof(g_boards_info) / sizeof(boards_info_t); i++) {
        const board_res_desc_t *brd = g_boards_info[i].board_get_res_desc();

        ESP_LOGI(TAG, "detecting board %s", g_boards_info[i].name);
        if (ESP_OK == g_boards_info[i].board_detect()) {
            g_board = (boards_info_t *)&g_boards_info[i];
            break;
        }
    }
    if (g_board) {
        ESP_LOGI(TAG, "Detected board: [%s]", g_board->name);
        ret = ESP_OK;
    } else {
        ESP_LOGE(TAG, "Can't Detect a correct board");
    }
    return ret;
}

const boards_info_t *bsp_board_get_info(void)
{
    return g_board;
}

const board_res_desc_t *bsp_board_get_description(void)
{
    return g_board->board_get_res_desc();
}

esp_err_t bsp_board_init(void)
{
    if (ESP_OK == bsp_board_detect()) {
        return g_board->board_init();
    }
    return ESP_FAIL;
}

esp_err_t bsp_board_power_ctrl(power_module_t module, bool on)
{
    if (g_board) {
        return g_board->board_power_ctrl(module, on);
    }
    return ESP_FAIL;
}
