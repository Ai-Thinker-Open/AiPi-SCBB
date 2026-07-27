/**
 * @file axk_1_47_lcd.h
 * @brief 1.47" IPS LCD (ST7789V, 172x320) 硬件SPI驱动模块 — Bouffalo BL616/BL618
 *
 * 接线: SCL=IO13, SDA=IO11, CS=IO10, DC=IO1, RST=IO18, BLK=IO20
 *
 * @version 0.1
 * @date 2026-07-27
 */
#ifndef _AXK_1_47_LCD_H_
#define _AXK_1_47_LCD_H_

#include "scbb_config.h"

#ifdef SCBB_1_47_LCD_ENABLED

#include "bflb_gpio.h"
#include "bflb_spi.h"
#include "bflb_mtimer.h"

/* ─── 屏幕参数 ─── */
#define AXK_1_47_LCD_WIDTH   172
#define AXK_1_47_LCD_HEIGHT  320

/* ─── 默认引脚 (可在 scbb_config.h 覆盖) ─── */
#ifndef AXK_1_47_LCD_PIN_SCL
#define AXK_1_47_LCD_PIN_SCL   GPIO_PIN_13
#endif
#ifndef AXK_1_47_LCD_PIN_SDA
#define AXK_1_47_LCD_PIN_SDA   GPIO_PIN_11
#endif
#ifndef AXK_1_47_LCD_PIN_CS
#define AXK_1_47_LCD_PIN_CS    GPIO_PIN_10
#endif
#ifndef AXK_1_47_LCD_PIN_DC
#define AXK_1_47_LCD_PIN_DC    GPIO_PIN_1
#endif
#ifndef AXK_1_47_LCD_PIN_RST
#define AXK_1_47_LCD_PIN_RST   GPIO_PIN_18
#endif
#ifndef AXK_1_47_LCD_PIN_BLK
#define AXK_1_47_LCD_PIN_BLK   GPIO_PIN_20
#endif

/* ─── SPI 配置 ─── */
#define AXK_1_47_LCD_SPI       "spi0"
#define AXK_1_47_LCD_SPI_FREQ  40000000  /* 40MHz */

/* ─── API ─── */
int  axk_1_47_lcd_init(void);
void axk_1_47_lcd_backlight(bool on);
void axk_1_47_lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void axk_1_47_lcd_draw_pixels(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *pixels);
void axk_1_47_lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void axk_1_47_lcd_clear(uint16_t color);

#endif /* SCBB_1_47_LCD_ENABLED */
#endif /* _AXK_1_47_LCD_H_ */
