/**
 * @file axk_ws2812.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_WS2812_H__
#define __AXK_WS2812_H__
#include "color_mode.h"
#include "scbb_config.h"

#ifdef SCBB_WS2812_ENABLED

/* BSP headers and ACLL macros are defined in scbb_config.h */
#ifdef SCBB_WS2812_PWM_DMA_HEADER
#include SCBB_WS2812_PWM_DMA_HEADER
#endif

typedef struct {
  color_t color;
} axk_ws2812_dev_t;

typedef struct {
  axk_ws2812_dev_t *dev;
  unsigned int led_count;
  float brightness;
} axk_ws2812_strip_t;

extern axk_ws2812_strip_t *axk_ws2812_strip_dev;
int axk_ws2812_init(axk_ws2812_strip_t *axk_ws2812_strip);
void axk_ws2812_show_leds(void);
void axk_ws2812_set_pixel_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
void axk_ws2812_set_pixel_brightness(uint8_t index, float brightness);
void axk_ws2812_set_all_pixels_color(uint8_t r, uint8_t g, uint8_t b,
                                     float brightness);
void axk_ws2812_set_global_brightness(float brightness);
void axk_ws2812_set_pixel_color_hsv(uint8_t index, uint8_t h, uint8_t s,
                                    uint8_t v);
void axk_ws2812_set_led_count(uint8_t count);

#endif /* SCBB_WS2812_ENABLED */
#endif
