/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "bsp_board.h"
#include "bsp_led.h"
#include "bsp_i2s.h"
#include "bsp_i2c.h"
#include "bsp_codec.h"
#include "../codec/es7210.h"
#include "../codec/es8311.h"
#include "button.h"
#include "bsp_btn.h"
#include "tca9554.h"
#include "ht8574.h"
#include "lcd_panel_config.h"
#include "bsp_lcd.h"
#include "../indev/indev.h"
#include "../indev/indev_tp.h"

static const char *TAG = "board lcd evb";

#define BIGENDIAN_DATA 1

#define EXPANDER_IO_PA_CTRL 0
#define EXPANDER_IO_SPI_CS 1
#define EXPANDER_IO_SPI_SCK 2
#define EXPANDER_IO_SPI_MOSI 3

static const board_button_t g_btns[] = {
    {BOARD_BTN_ID_ENTER, 0,      GPIO_NUM_9,    0},
};


static const board_res_desc_t g_board_lcd_evb_res = {

    .FUNC_LCD_EN =     (1),

    .LCD_IFACE =   (LCD_IFACE_SPI),
    .LCD_HOST  =   (SPI2_HOST),
    .LCD_DISP_IC_STR = "gc9a01",
    .LCD_FREQ =        (40 * 1000 * 1000),
    .LCD_CMD_BITS =    (8),
    .LCD_PARAM_BITS =  (8),
    .LCD_SWAP_XY =     (false),
    .LCD_MIRROR_X =    (false),
    .LCD_MIRROR_Y =    (false),
    .LCD_COLOR_INV =   (false),
    .BSP_INDEV_IS_TP =         (0),
    .TOUCH_PANEL_SWAP_XY =     (0),
    .TOUCH_PANEL_INVERSE_X =   (0),
    .TOUCH_PANEL_INVERSE_Y =   (0),

    .LCD_WIDTH =       (240),
    .LCD_HEIGHT =      (240),
    .LCD_COLOR_SPACE = ESP_LCD_COLOR_SPACE_RGB,

    .GPIO_LCD_CS      = GPIO_NUM_10,
    .GPIO_LCD_CLK     = GPIO_NUM_1,
    .GPIO_LCD_DIN     = GPIO_NUM_0,
    .GPIO_LCD_DOUT    = GPIO_NUM_NC,
    .GPIO_LCD_RST     = GPIO_NUM_2,
    .GPIO_LCD_DC      = GPIO_NUM_4,
    .GPIO_LCD_BL      = GPIO_NUM_5,
    .GPIO_LCD_BL_ON   = 1,


    .TOUCH_PANEL_I2C_ADDR = 0,
    .TOUCH_WITH_HOME_BUTTON = 0,

    .BSP_BUTTON_EN =   (1),
    .BUTTON_TAB =  g_btns,
    .BUTTON_TAB_LEN = sizeof(g_btns) / sizeof(g_btns[0]),

    .FUNC_I2C_EN =     (1),
    .GPIO_I2C_SCL =    (GPIO_NUM_18),
    .GPIO_I2C_SDA =    (GPIO_NUM_8),

    .FUNC_SDMMC_EN =   (0),
    .SDMMC_BUS_WIDTH = (1),
    .GPIO_SDMMC_CLK =  (GPIO_NUM_NC),
    .GPIO_SDMMC_CMD =  (GPIO_NUM_NC),
    .GPIO_SDMMC_D0 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_D1 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_D2 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_D3 =   (GPIO_NUM_NC),
    .GPIO_SDMMC_DET =  (GPIO_NUM_NC),

    .FUNC_SDSPI_EN =       (0),
    .SDSPI_HOST =          (SPI2_HOST),
    .GPIO_SDSPI_CS =       (GPIO_NUM_NC),
    .GPIO_SDSPI_SCLK =     (GPIO_NUM_NC),
    .GPIO_SDSPI_MISO =     (GPIO_NUM_NC),
    .GPIO_SDSPI_MOSI =     (GPIO_NUM_NC),

    .FUNC_SPI_EN =         (0),
    .GPIO_SPI_CS =         (GPIO_NUM_NC),
    .GPIO_SPI_MISO =       (GPIO_NUM_NC),
    .GPIO_SPI_MOSI =       (GPIO_NUM_NC),
    .GPIO_SPI_SCLK =       (GPIO_NUM_NC),

    .FUNC_RMT_EN =         (1),
    .GPIO_RMT_IR =         (GPIO_NUM_NC),
    .GPIO_RMT_LED =        (GPIO_NUM_8),
    .RMT_LED_NUM  =        4,

    .FUNC_I2S_EN =         (1),
    .GPIO_I2S_LRCK =       (GPIO_NUM_7),
    .GPIO_I2S_MCLK =       (GPIO_NUM_5),
    .GPIO_I2S_SCLK =       (GPIO_NUM_16),
    .GPIO_I2S_SDIN =       (GPIO_NUM_15),
    .GPIO_I2S_DOUT =       (GPIO_NUM_6),
    .CODEC_I2C_ADDR = 0,
    .AUDIO_ADC_I2C_ADDR = 0,

    .IMU_I2C_ADDR = 0,

    .FUNC_PWR_CTRL =       (0),
    .GPIO_PWR_CTRL =       (GPIO_NUM_NC),
    .GPIO_PWR_ON_LEVEL =   (1),

    .GPIO_MUTE_NUM =   GPIO_NUM_NC,
    .GPIO_MUTE_LEVEL = 1,

    .PMOD1 = NULL,
    .PMOD2 = NULL,

    .codec_ops = NULL,
    .io_expander_ops = NULL,
};

esp_err_t bsp_board_knob_panel_detect(void)
{
    return ESP_OK;
}

esp_err_t bsp_board_knob_panel_init(void)
{
    bsp_btn_init_default();
    ESP_ERROR_CHECK(bsp_lcd_init());
    bsp_led_init();
    bsp_led_set_rgb(0, 255, 0, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    bsp_led_set_rgb(0, 0, 255, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    indev_init_default();
    bsp_led_set_rgb(0, 0, 0, 255);
    vTaskDelay(pdMS_TO_TICKS(100));
    bsp_led_set_rgb(0, 0, 0, 0);
    return ESP_OK;
}

esp_err_t bsp_board_knob_panel_power_ctrl(power_module_t module, bool on)
{
    /* Control independent power domain */
    switch (module) {
    case POWER_MODULE_LCD:
        break;
    case POWER_MODULE_AUDIO:
        break;
    default:
        return ESP_ERR_INVALID_ARG;
    }

    return ESP_OK;
}

const board_res_desc_t *bsp_board_knob_panel_get_res_desc(void)
{
    return &g_board_lcd_evb_res;
}
