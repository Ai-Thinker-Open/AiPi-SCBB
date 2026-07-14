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

#ifdef SCBB_BSP_PWM_DMA_HEADER
#include SCBB_BSP_PWM_DMA_HEADER
#define AXK_WS2812_ACLL(_func, ...) bsp_pwm_dma_##_func(__VA_ARGS__)
#elif __has_include("stm32f10x_pwm_dma.h")
#include "stm32f10x_pwm_dma.h"
#define AXK_WS2812_ACLL(_func, ...) bsp_pwm_dma_##_func(__VA_ARGS__)
#else
#error "Please include the appropriate PWM DMA header for WS2812 or set SCBB_BSP_PWM_DMA_HEADER in scbb_config.h."
#endif

// 添加bsp_pwm_dma_deinit函数声明
void bsp_pwm_dma_deinit(void);

typedef struct {
  color_t color;
} axk_ws2812_dev_t;
// 定义WS2812灯条结构体
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
#endif