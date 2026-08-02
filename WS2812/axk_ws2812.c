/**
 * @file axk_ws2812.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief WS2812 可寻址 RGB LED 灯条驱动实现（PWM+DMA）。
 * @version 0.1
 * @date 2026-01-30
 * @copyright Copyright (c) 2026
 */
#include <string.h>

#include "axk_ws2812.h"

#ifdef SCBB_WS2812_ENABLED

/** 当前活跃灯条的全局指针，由 axk_ws2812_init() 设置 */
axk_ws2812_strip_t *g_axk_ws2812_strip_dev = NULL;

/** 模块静态 LED 设备缓冲（最多 AXK_WS2812_MAX_NUM 颗） */
static axk_ws2812_dev_t s_axk_ws2812_dev_buf[AXK_WS2812_MAX_NUM];

/**
 * @brief 初始化 WS2812 灯条，绑定设备数组并清空所有 LED
 *
 * @param[in/out] axk_ws2812_strip  用户提供的灯条结构体指针
 * @return       int                操作状态
 *                - 0: 初始化成功
 *                - -1: 参数为空、led_count 为 0 或超过上限
 * @note         设备数组为 NULL 时使用模块静态缓冲（上限 AXK_WS2812_MAX_NUM）
 */
int axk_ws2812_init(axk_ws2812_strip_t *axk_ws2812_strip) {
    if ((axk_ws2812_strip == NULL) || (axk_ws2812_strip->led_count == 0) ||
        (axk_ws2812_strip->led_count > AXK_WS2812_MAX_NUM)) {
        return -1;
    }
    AXK_WS2812_ACLL(init, axk_ws2812_strip->led_count);

    if (axk_ws2812_strip->dev == NULL) {
        axk_ws2812_strip->dev = s_axk_ws2812_dev_buf;
    }
    axk_ws2812_strip->brightness = 0.5;

    for (unsigned char i = 0; i < axk_ws2812_strip->led_count; i++) {
        axk_ws2812_strip->dev[i].color.r = 0;
        axk_ws2812_strip->dev[i].color.g = 0;
        axk_ws2812_strip->dev[i].color.b = 0;
    }

    if (g_axk_ws2812_strip_dev == NULL) {
        g_axk_ws2812_strip_dev = axk_ws2812_strip;
    }
    return 0;
}

/**
 * @brief 将颜色缓冲区通过 PWM+DMA 发送到物理灯条
 */
void axk_ws2812_show_leds(void) { AXK_WS2812_ACLL(with_num); }

/**
 * @brief 按索引设置单颗 LED 的 RGB 颜色
 *
 * @param[in]  index  LED 索引（从 0 开始）
 * @param[in]  r      红色值（0–255）
 * @param[in]  g      绿色值（0–255）
 * @param[in]  b      蓝色值（0–255）
 */
void axk_ws2812_set_pixel_color(uint8_t index, uint8_t r, uint8_t g,
                                                                uint8_t b) {
    if (index >= g_axk_ws2812_strip_dev->led_count) {
        return;
    }
    g_axk_ws2812_strip_dev->dev[index].color.r = r;
    g_axk_ws2812_strip_dev->dev[index].color.g = g;
    g_axk_ws2812_strip_dev->dev[index].color.b = b;
}

/**
 * @brief 通过 HSV 转换调整单颗 LED 亮度
 *
 * @param[in]  index      LED 索引（从 0 开始）
 * @param[in]  brightness 目标明度值（0.0–1.0）
 */
void axk_ws2812_set_pixel_brightness(uint8_t index, float brightness) {
    if (index >= g_axk_ws2812_strip_dev->led_count) {
        return;
    }
    axk_hsv_color_t hsv = axk_rgb_to_hsv(g_axk_ws2812_strip_dev->dev[index].color);
    hsv.v = brightness;
    axk_color_t rgb = axk_hsv_to_rgb(hsv);
    axk_ws2812_set_pixel_color(index, rgb.r, rgb.g, rgb.b);
}

/**
 * @brief 将所有 LED 设为同一颜色，按给定亮度调整并刷新
 *
 * @param[in]  r          红色值（0–255）
 * @param[in]  g          绿色值（0–255）
 * @param[in]  b          蓝色值（0–255）
 * @param[in]  brightness 亮度系数（0.0–1.0）
 */
void axk_ws2812_set_all_pixels_color(uint8_t r, uint8_t g, uint8_t b,
                                                                          float brightness) {
    axk_color_t rgb = {r, g, b};
    for (uint8_t i = 0; i < g_axk_ws2812_strip_dev->led_count; i++) {
        rgb.r = r;
        rgb.g = g;
        rgb.b = b;
        axk_hsv_color_t hsv = axk_rgb_to_hsv(rgb);
        hsv.v = brightness;
        rgb = axk_hsv_to_rgb(hsv);
        axk_ws2812_set_pixel_color(i, rgb.r, rgb.g, rgb.b);
    }
    axk_ws2812_show_leds();
}

/**
 * @brief 通过 HSV 转换设置整个灯条全局亮度并刷新
 *
 * @param[in]  brightness 全局亮度（0.0–1.0）
 */
void axk_ws2812_set_global_brightness(float brightness) {
    g_axk_ws2812_strip_dev->brightness = brightness;
    for (uint8_t i = 0; i < g_axk_ws2812_strip_dev->led_count; i++) {
        axk_hsv_color_t hsv = axk_rgb_to_hsv(g_axk_ws2812_strip_dev->dev[i].color);
        hsv.v = g_axk_ws2812_strip_dev->brightness;
        axk_color_t rgb = axk_hsv_to_rgb(hsv);
        axk_ws2812_set_pixel_color(i, rgb.r, rgb.g, rgb.b);
    }
    axk_ws2812_show_leds();
}

/**
 * @brief 用 HSV 值设置单颗 LED 颜色（内部转为 RGB）
 *
 * @param[in]  index  LED 索引（从 0 开始）
 * @param[in]  h      色相（0–255，映射 0–360°）
 * @param[in]  s      饱和度（0–255，映射 0.0–1.0）
 * @param[in]  v      明度（0–255，映射 0.0–1.0）
 */
void axk_ws2812_set_pixel_color_hsv(uint8_t index, uint8_t h, uint8_t s,
                                    uint8_t v) {
    axk_hsv_color_t hsv;
    /* 将 0-255 输入映射到 HSV 标称范围（h: 0-360，s/v: 0.0-1.0） */
    hsv.h = h * (360.0f / 255.0f);
    hsv.s = s / 255.0f;
    hsv.v = v / 255.0f;
    axk_color_t rgb = axk_hsv_to_rgb(hsv);
    axk_ws2812_set_pixel_color(index, rgb.r, rgb.g, rgb.b);
}

/**
 * @brief 更新灯条逻辑 LED 数量（不重新分配内存）
 *
 * @param[in]  count  新的 LED 数量（1–60）
 */
void axk_ws2812_set_led_count(uint8_t count) {
    if (count > AXK_WS2812_MAX_NUM) {
        return;
    }
    g_axk_ws2812_strip_dev->led_count = count;
}

/**
 * @brief 获取当前活跃灯条的 LED 数量
 *
 * @return unsigned char  LED 数量
 */
unsigned char axk_ws2812_get_led_count(void) {
    return g_axk_ws2812_strip_dev->led_count;
}

#endif /* SCBB_WS2812_ENABLED */
