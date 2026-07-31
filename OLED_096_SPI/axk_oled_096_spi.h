/**
 * @file axk_oled_096_spi.h
 * @brief 0.96" OLED (SSD1306, 128x64, SPI) — 纯 ACLL 跨平台
 */
#ifndef _AXK_OLED_096_SPI_H_
#define _AXK_OLED_096_SPI_H_

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

#define AXK_OLED_096_SPI_W  128
#define AXK_OLED_096_SPI_H  64

#ifndef AXK_OLED_096_SPI_PIN_SCL
#define AXK_OLED_096_SPI_PIN_SCL  13
#endif
#ifndef AXK_OLED_096_SPI_PIN_SDA
#define AXK_OLED_096_SPI_PIN_SDA  11
#endif
#ifndef AXK_OLED_096_SPI_PIN_CS
#define AXK_OLED_096_SPI_PIN_CS   10
#endif
#ifndef AXK_OLED_096_SPI_PIN_DC
#define AXK_OLED_096_SPI_PIN_DC   1
#endif
#ifndef AXK_OLED_096_SPI_PIN_RST
#define AXK_OLED_096_SPI_PIN_RST  18
#endif
#ifndef AXK_OLED_096_SPI_SPI
#define AXK_OLED_096_SPI_SPI  "spi0"
#endif

int  axk_oled_096_spi_init(void);
void axk_oled_096_spi_clear(uint8_t color);  /* 0=黑, 1=白 */
void axk_oled_096_spi_set_pixel(int x, int y, int on);
void axk_oled_096_spi_flush(void);           /* 刷新全屏 */

#endif
#endif
