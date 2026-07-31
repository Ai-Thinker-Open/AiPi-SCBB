/**
 * @file axk_st7789v_lcd.h
 * @brief ST7789V 驱动芯片通用 LCD 模块（硬件SPI）
 *
 * 适用于市面上所有 ST7789V 驱动芯片的屏幕：1.47"、1.69"、1.9"、2.0" 等。
 * 只需配置分辨率、偏移量和引脚，即可适配不同尺寸。
 *
 * 跨平台：Bouffalo BL616/BL618 / STM32 / 任意 C99 32位 MCU
 *
 * 默认引脚(可覆盖):
 *   BL616: SCL=IO13, SDA=IO11, CS=IO10, DC=IO1, RST=IO18, BLK=IO20
 *
 * @version 0.1
 * @date 2026-07-27
 */
#ifndef _AXK_ST7789V_LCD_H_
#define _AXK_ST7789V_LCD_H_

#include "scbb_config.h"

#ifdef SCBB_ST7789V_LCD_ENABLED

/* ── BSP headers ── */
#ifdef SCBB_ST7789V_LCD_SPI_HEADER
#include SCBB_ST7789V_LCD_SPI_HEADER
#endif
#ifdef SCBB_ST7789V_LCD_GPIO_HEADER
#include SCBB_ST7789V_LCD_GPIO_HEADER
#endif
#ifdef SCBB_ST7789V_LCD_DELAY_HEADER
#include SCBB_ST7789V_LCD_DELAY_HEADER
#endif

#include <stdint.h>
#include <stdbool.h>

/* ── 屏幕参数 (可覆盖) ── */
#ifndef AXK_ST7789V_LCD_WIDTH
#define AXK_ST7789V_LCD_WIDTH   172
#endif
#ifndef AXK_ST7789V_LCD_HEIGHT
#define AXK_ST7789V_LCD_HEIGHT  320
#endif
#ifndef AXK_ST7789V_LCD_OFFSET_X
#define AXK_ST7789V_LCD_OFFSET_X 34   /* 172宽居中于240宽GRAM */
#endif
#ifndef AXK_ST7789V_LCD_OFFSET_Y
#define AXK_ST7789V_LCD_OFFSET_Y 0
#endif

/* ── 引脚默认值 (可覆盖) ── */
#ifndef AXK_ST7789V_LCD_PIN_SCL
#define AXK_ST7789V_LCD_PIN_SCL   13
#endif
#ifndef AXK_ST7789V_LCD_PIN_SDA
#define AXK_ST7789V_LCD_PIN_SDA   11
#endif
#ifndef AXK_ST7789V_LCD_PIN_CS
#define AXK_ST7789V_LCD_PIN_CS    10
#endif
#ifndef AXK_ST7789V_LCD_PIN_DC
#define AXK_ST7789V_LCD_PIN_DC    1
#endif
#ifndef AXK_ST7789V_LCD_PIN_RST
#define AXK_ST7789V_LCD_PIN_RST   18
#endif
#ifndef AXK_ST7789V_LCD_PIN_BLK
#define AXK_ST7789V_LCD_PIN_BLK   20
#endif

/* ── SPI 默认值 (可覆盖) ── */
#ifndef AXK_ST7789V_LCD_SPI_DEV
#define AXK_ST7789V_LCD_SPI_DEV   "spi0"
#endif
#ifndef AXK_ST7789V_LCD_SPI_FREQ
#define AXK_ST7789V_LCD_SPI_FREQ  40000000
#endif

/* SPI 配置结构由 BSP 提供 (bsp_spi_cfg_t)，见 BSP header */

/* ── API ── */
int  axk_st7789v_lcd_init(void);
void axk_st7789v_lcd_backlight(bool on);
void axk_st7789v_lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void axk_st7789v_lcd_draw_pixels(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *pixels);
void axk_st7789v_lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void axk_st7789v_lcd_clear(uint16_t color);

#endif /* SCBB_ST7789V_LCD_ENABLED */
#endif /* _AXK_ST7789V_LCD_H_ */
