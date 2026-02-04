/**
 * @file axk_ws2812.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_ws2812.h"
#include <stdlib.h>
#include <string.h>
#include <sys/_types.h>

axk_ws2812_strip_t *axk_ws2812_strip_dev = NULL;

/**
 * @brief 初始化WS2812灯条
 *
 * @param axk_ws2812_strip
 * @return int
 */
int axk_ws2812_init(axk_ws2812_strip_t *axk_ws2812_strip) {

  if ((axk_ws2812_strip == NULL) || (axk_ws2812_strip->led_count == 0)) {
    return -1;
  }
  AXK_WS2812_ACLL(init, axk_ws2812_strip->led_count);
  // 初始化WS2812灯条设备
  if (axk_ws2812_strip->dev == NULL) {
    axk_ws2812_strip->dev =
        malloc(axk_ws2812_strip->led_count * sizeof(axk_ws2812_dev_t));
    axk_ws2812_strip->brightness = 0.5;

    for (unsigned char i = 0; i < axk_ws2812_strip->led_count; i++) {
      axk_ws2812_strip->dev[i].color.r = 0;
      axk_ws2812_strip->dev[i].color.g = 0;
      axk_ws2812_strip->dev[i].color.b = 0;
    }
  }

  if (axk_ws2812_strip_dev == NULL) {
    axk_ws2812_strip_dev = axk_ws2812_strip;
  }
  return 0;
}
/**
 * @brief 显示WS2812灯条上的LED颜色
 *
 */
void axk_ws2812_show_leds(void) { AXK_WS2812_ACLL(with_num); }

/**
 * @brief 设置单个LED的颜色
 *
 * @param index
 * @param r
 * @param g
 * @param b
 */
void axk_ws2812_set_pixel_color(uint8_t index, uint8_t r, uint8_t g,
                                uint8_t b) {
  if (index >= axk_ws2812_strip_dev->led_count) {
    return;
  }
  axk_ws2812_strip_dev->dev[index].color.r = r;
  axk_ws2812_strip_dev->dev[index].color.g = g;
  axk_ws2812_strip_dev->dev[index].color.b = b;
}
/**
 * @brief 设置单个LED的亮度
 *
 * @param index
 * @param brightness
 */
void axk_ws2812_set_pixel_brightness(uint8_t index, float brightness) {
  // 更新所有LED以应用新亮度
  hsv_color_t hsv = rgb_to_hsv(axk_ws2812_strip_dev->dev[index].color);
  // 使用原始颜色的HSV，仅调整亮度（强制最大值以保持颜色）
  hsv.v = brightness;
  // hsv.s = ;
  color_t rgb = hsv_to_rgb(hsv);
  axk_ws2812_set_pixel_color(index, rgb.r, rgb.g, rgb.b);
  // ws2812_show_leds();
}
/**
 * @brief 设置WS2812灯条上所有LED的颜色
 *
 * @param r
 * @param g
 * @param b
 * @param brightness
 */
void axk_ws2812_set_all_pixels_color(uint8_t r, uint8_t g, uint8_t b,
                                     float brightness) {
  color_t rgb = {r, g, b};
  for (uint8_t i = 0; i < axk_ws2812_strip_dev->led_count; i++) {
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
    hsv_color_t hsv = rgb_to_hsv(rgb);
    // 使用原始颜色的HSV，仅调整亮度（强制最大值以保持颜色）
    hsv.v = brightness;
    rgb = hsv_to_rgb(hsv);
    axk_ws2812_set_pixel_color(i, rgb.r, rgb.g, rgb.b);
  }
  axk_ws2812_show_leds();
}
/**
 * @brief 设置WS2812灯条上所有LED的全局亮度
 *
 * @param brightness
 */
void axk_ws2812_set_global_brightness(float brightness) {
  // 更新所有LED以应用新亮度
  axk_ws2812_strip_dev->brightness = brightness;
  for (uint8_t i = 0; i < axk_ws2812_strip_dev->led_count; i++) {
    hsv_color_t hsv = rgb_to_hsv(axk_ws2812_strip_dev->dev[i].color);
    // 使用原始颜色的HSV，仅调整亮度（强制最大值以保持颜色）
    hsv.v = axk_ws2812_strip_dev->brightness;
    color_t rgb = hsv_to_rgb(hsv);
    axk_ws2812_set_pixel_color(i, rgb.r, rgb.g, rgb.b);
  }
  axk_ws2812_show_leds();
}
/**
 * @brief 设置单个LED的颜色（HSV模式）
 *
 * @param index
 * @param h
 * @param s
 * @param v
 */
void axk_ws2812_set_pixel_color_hsv(uint8_t index, uint8_t h, uint8_t s,
                                    uint8_t v) {
  hsv_color_t hsv = {h, s, v};
  color_t rgb = hsv_to_rgb(hsv);
  axk_ws2812_set_pixel_color(index, rgb.r, rgb.g, rgb.b);
}
/**
 * @brief 设置WS2812灯条上的LED数量
 *
 * @param count
 */
void axk_ws2812_set_led_count(uint8_t count) {
  axk_ws2812_strip_dev->led_count = count;
}
/**
 * @brief 获取WS2812灯条上的LED数量
 *
 * @return uint8_t
 */
unsigned char axk_ws2812_get_led_count(void) {
  return axk_ws2812_strip_dev->led_count;
}