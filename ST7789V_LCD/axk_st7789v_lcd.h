/**
 * @file axk_st7789v_lcd.h
 * @brief ST7789V 驱动芯片通用 LCD 模块（硬件 SPI）。
 *
 * 适用于所有 ST7789V 驱动屏幕：1.47"、1.69"、1.9"、2.0" 等。
 * 只需配置分辨率、偏移量和引脚即可适配。
 * 跨平台：Bouffalo BL616/BL618、STM32、任意 C99 32 位 MCU。
 *
 * 默认引脚（BL616，可通过 #define 覆盖）：
 *   SCL=IO13, SDA=IO11, CS=IO10, DC=IO1, RST=IO18, BLK=IO20
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef AXK_ST7789V_LCD_H
#define AXK_ST7789V_LCD_H

#include "scbb_config.h"

#ifdef SCBB_ST7789V_LCD_ENABLED

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

/** @brief 显示宽度（像素） */
#ifndef AXK_ST7789V_LCD_WIDTH
#define AXK_ST7789V_LCD_WIDTH   172
#endif
/** @brief 显示高度（像素） */
#ifndef AXK_ST7789V_LCD_HEIGHT
#define AXK_ST7789V_LCD_HEIGHT  320
#endif
/** @brief X 偏移：172px 居中于 240px GRAM */
#ifndef AXK_ST7789V_LCD_OFFSET_X
#define AXK_ST7789V_LCD_OFFSET_X 34
#endif
/** @brief Y 偏移 */
#ifndef AXK_ST7789V_LCD_OFFSET_Y
#define AXK_ST7789V_LCD_OFFSET_Y 0
#endif

#ifndef AXK_ST7789V_LCD_PIN_SCL
#define AXK_ST7789V_LCD_PIN_SCL   13    /**< SPI 时钟脚 */
#endif
#ifndef AXK_ST7789V_LCD_PIN_SDA
#define AXK_ST7789V_LCD_PIN_SDA   11    /**< SPI MOSI 数据脚 */
#endif
#ifndef AXK_ST7789V_LCD_PIN_CS
#define AXK_ST7789V_LCD_PIN_CS    10    /**< 片选脚（低有效） */
#endif
#ifndef AXK_ST7789V_LCD_PIN_DC
#define AXK_ST7789V_LCD_PIN_DC    1     /**< 数据/命令选择脚 */
#endif
#ifndef AXK_ST7789V_LCD_PIN_RST
#define AXK_ST7789V_LCD_PIN_RST   18    /**< 复位脚 */
#endif
#ifndef AXK_ST7789V_LCD_PIN_BLK
#define AXK_ST7789V_LCD_PIN_BLK   20    /**< 背光控制脚 */
#endif

#ifndef AXK_ST7789V_LCD_SPI_DEV
#define AXK_ST7789V_LCD_SPI_DEV   "spi0" /**< SPI 设备名 */
#endif
#ifndef AXK_ST7789V_LCD_SPI_FREQ
#define AXK_ST7789V_LCD_SPI_FREQ  40000000 /**< SPI 时钟频率（Hz） */
#endif

/**
 * @brief 通过 SPI 初始化 ST7789V LCD 并配置 GPIO
 *
 * 上电显示，发送初始化指令序列，清屏，开启背光。
 *
 * @return int  0: 成功
 * @note  Bouffalo 平台需依赖 lcd.h 和 bflb_gpio.h
 */
int  axk_st7789v_lcd_init(void);

/**
 * @brief 开关 LCD 背光
 *
 * @param[in]  on  true: 开，false: 关
 */
void axk_st7789v_lcd_backlight(bool on);

/**
 * @brief 设置活动绘制窗口（CASET + RASET）
 *
 * @param[in]  x1  起始 X（含）
 * @param[in]  y1  起始 Y（含）
 * @param[in]  x2  结束 X（含）
 * @param[in]  y2  结束 Y（含）
 */
void axk_st7789v_lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * @brief 向指定窗口绘制像素块（阻塞传输）
 *
 * @param[in]  x1      起始 X
 * @param[in]  y1      起始 Y
 * @param[in]  x2      结束 X
 * @param[in]  y2      结束 Y
 * @param[in]  *pixels RGB565 像素数据
 */
void axk_st7789v_lcd_draw_pixels(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                                  uint16_t *pixels);

/**
 * @brief 用纯色填充矩形区域
 *
 * @param[in]  x1    起始 X
 * @param[in]  y1    起始 Y
 * @param[in]  x2    结束 X
 * @param[in]  y2    结束 Y
 * @param[in]  color RGB565 填充颜色
 */
void axk_st7789v_lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                          uint16_t color);

/**
 * @brief 清屏为单一颜色
 *
 * @param[in]  color  RGB565 填充颜色
 */
void axk_st7789v_lcd_clear(uint16_t color);

#endif /* SCBB_ST7789V_LCD_ENABLED */
#endif /* AXK_ST7789V_LCD_H */
