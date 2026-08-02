/**
 * @file axk_ws2812.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief WS2812 可寻址 RGB LED 灯条驱动（PWM+DMA）。
 *
 * 支持最多 60 颗 LED，提供逐像素 RGB/HSV 颜色、亮度控制及全局亮度调节。
 * 数据格式: GRB（硬件传输顺序）。
 * 跨平台：通过 scbb_config.h 中的 ACLL 宏实现 BSP 抽象。
 *
 * @version 0.1
 * @date 2026-01-30
 * @copyright Copyright (c) 2026
 */
#ifndef AXK_WS2812_H
#define AXK_WS2812_H
#include "color_mode.h"
#include "scbb_config.h"

#ifdef SCBB_WS2812_ENABLED

#ifdef SCBB_WS2812_PWM_DMA_HEADER
#include SCBB_WS2812_PWM_DMA_HEADER
#endif

/** @brief WS2812 灯珠数量上限 */
#define AXK_WS2812_MAX_NUM 60

/** @brief 单颗 LED 设备，存储 RGB 颜色 */
typedef struct {
  axk_color_t color;
} axk_ws2812_dev_t;

/** @brief LED 灯条实例，设备数组 + 数量 + 亮度 */
typedef struct {
  axk_ws2812_dev_t *dev; /**< LED 设备数组（为 NULL 时使用模块静态缓冲） */
  unsigned int led_count; /**< LED 数量 */
  float brightness;       /**< 全局亮度系数（0.0–1.0） */
} axk_ws2812_strip_t;

/** @brief 当前活跃灯条的全局指针（由 axk_ws2812_init 设置） */
extern axk_ws2812_strip_t *g_axk_ws2812_strip_dev;

/**
 * @brief 初始化 WS2812 灯条，指定 LED 数量
 *
 * 分配设备数组，设置默认亮度（50%），清除所有 LED。
 * 灯条结构体由调用方分配（栈或静态区均可）。
 *
 * @param[in/out] axk_ws2812_strip  用户提供的灯条结构体指针
 * @return       int                操作状态
 *                - 0: 初始化成功
 *                - -1: 结构体为 NULL 或 led_count 为 0
 * @note  须在其他 WS2812 函数调用之前执行
 * @see   axk_ws2812_show_leds()
 */
int axk_ws2812_init(axk_ws2812_strip_t *axk_ws2812_strip);

/**
 * @brief 将 LED 颜色缓冲区通过 PWM+DMA 发送到物理灯条
 */
void axk_ws2812_show_leds(void);

/**
 * @brief 按索引设置单颗 LED 的 RGB 颜色
 *
 * @param[in]  index  LED 索引（从 0 开始）
 * @param[in]  r      红色值（0–255）
 * @param[in]  g      绿色值（0–255）
 * @param[in]  b      蓝色值（0–255）
 */
void axk_ws2812_set_pixel_color(uint8_t index, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief 通过 HSV 转换调整单颗 LED 的亮度
 *
 * @param[in]  index      LED 索引（从 0 开始）
 * @param[in]  brightness 目标明度值（0.0–1.0）
 */
void axk_ws2812_set_pixel_brightness(uint8_t index, float brightness);

/**
 * @brief 将所有 LED 设为同一颜色，按给定亮度调整并刷新
 *
 * @param[in]  r          红色值（0–255）
 * @param[in]  g          绿色值（0–255）
 * @param[in]  b          蓝色值（0–255）
 * @param[in]  brightness 亮度系数（0.0–1.0）
 */
void axk_ws2812_set_all_pixels_color(uint8_t r, uint8_t g, uint8_t b,
                                     float brightness);

/**
 * @brief 通过 HSV 转换设置整个灯条全局亮度并刷新
 *
 * @param[in]  brightness 全局亮度（0.0–1.0）
 */
void axk_ws2812_set_global_brightness(float brightness);

/**
 * @brief 用 HSV 值设置单颗 LED 颜色（内部转为 RGB）
 *
 * @param[in]  index  LED 索引（从 0 开始）
 * @param[in]  h      色相（0–255，映射 0–360°）
 * @param[in]  s      饱和度（0–255，映射 0.0–1.0）
 * @param[in]  v      明度（0–255，映射 0.0–1.0）
 */
void axk_ws2812_set_pixel_color_hsv(uint8_t index, uint8_t h, uint8_t s,
                                    uint8_t v);

/**
 * @brief 更新灯条逻辑 LED 数量（不重新分配内存）
 *
 * @param[in]  count  新的 LED 数量（1–60）
 * @note  确保新数量不超过原始分配大小
 */
void axk_ws2812_set_led_count(uint8_t count);

/**
 * @brief 获取当前活跃灯条的 LED 数量
 *
 * @return unsigned char  LED 数量
 */
unsigned char axk_ws2812_get_led_count(void);

#endif /* SCBB_WS2812_ENABLED */
#endif /* AXK_WS2812_H */
