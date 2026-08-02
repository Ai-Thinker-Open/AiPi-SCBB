/**
 * @file axk_oled_096_spi.h
 * @brief 0.96" OLED (SSD1306, 128x64, SPI) — 纯 ACLL 跨平台。
 *
 * 硬件 SPI，帧缓冲渲染。支持像素级绘制、全屏刷新和清屏。
 * 默认引脚（BL616，可覆盖）：SCL=IO13, SDA=IO11, CS=IO10, DC=IO1, RST=IO18。
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef AXK_OLED_096_SPI_H
#define AXK_OLED_096_SPI_H

#include "scbb_config.h"

#ifdef SCBB_OLED_096_SPI_ENABLED

#ifdef SCBB_OLED_096_SPI_SPI_HEADER
#include SCBB_OLED_096_SPI_SPI_HEADER
#endif
#ifdef SCBB_OLED_096_SPI_GPIO_HEADER
#include SCBB_OLED_096_SPI_GPIO_HEADER
#endif
#ifdef SCBB_OLED_096_SPI_DELAY_HEADER
#include SCBB_OLED_096_SPI_DELAY_HEADER
#endif

#include <stdint.h>
#include <stdbool.h>

#define AXK_OLED_096_SPI_W  128  /**< 显示宽度（像素） */
#define AXK_OLED_096_SPI_H  64   /**< 显示高度（像素） */

#ifndef AXK_OLED_096_SPI_PIN_SCL
#define AXK_OLED_096_SPI_PIN_SCL  13  /**< SPI 时钟脚 */
#endif
#ifndef AXK_OLED_096_SPI_PIN_SDA
#define AXK_OLED_096_SPI_PIN_SDA  11  /**< SPI MOSI 数据脚 */
#endif
#ifndef AXK_OLED_096_SPI_PIN_CS
#define AXK_OLED_096_SPI_PIN_CS   10  /**< 片选脚（低有效） */
#endif
#ifndef AXK_OLED_096_SPI_PIN_DC
#define AXK_OLED_096_SPI_PIN_DC   1   /**< 数据/命令选择脚 */
#endif
#ifndef AXK_OLED_096_SPI_PIN_RST
#define AXK_OLED_096_SPI_PIN_RST  18  /**< 复位脚 */
#endif
#ifndef AXK_OLED_096_SPI_SPI
#define AXK_OLED_096_SPI_SPI  "spi0" /**< SPI 设备名 */
#endif

/**
 * @brief 初始化 SSD1306 OLED 并通过 SPI 发送初始化序列
 *
 * @return int  0: 成功
 */
int  axk_oled_096_spi_init(void);

/**
 * @brief 将帧缓冲清为纯色
 *
 * @param[in]  color  0: 全黑，1: 全白
 * @note  调用后需执行 axk_oled_096_spi_flush() 刷新显示
 */
void axk_oled_096_spi_clear(uint8_t color);

/**
 * @brief 在帧缓冲中设置或清除单个像素
 *
 * @param[in]  x   X 坐标（0–127）
 * @param[in]  y   Y 坐标（0–63）
 * @param[in]  on  1: 点亮，0: 熄灭
 */
void axk_oled_096_spi_set_pixel(int x, int y, int on);

/**
 * @brief 将帧缓冲通过 SPI 刷新到 OLED 显示屏
 */
void axk_oled_096_spi_flush(void);

#endif /* SCBB_OLED_096_SPI_ENABLED */
#endif /* AXK_OLED_096_SPI_H */
