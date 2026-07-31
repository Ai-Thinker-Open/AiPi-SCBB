/**
 * @file color_mode.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief HSV/RGB 颜色转换与动画工具。
 *
 * 提供 HSV ↔ RGB 互转、含缓动函数的颜色插值、渐变生成和色轮动画。
 * 供 WS2812 及其他支持颜色的外设使用。
 *
 * @version 0.1
 * @date 2025-07-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef COLOR_MODE_H
#define COLOR_MODE_H
#include <stdint.h>

/** @brief 24 位 RGB 颜色表示。 */
typedef struct {
    uint8_t r; /**< 红色通道（0–255）。 */
    uint8_t g; /**< 绿色通道（0–255）。 */
    uint8_t b; /**< 蓝色通道（0–255）。 */
} color_t;

/** @brief HSV 颜色表示。 */
typedef struct {
    float h; /**< 色相（0.0–360.0 度）。 */
    float s; /**< 饱和度（0.0–1.0）。 */
    float v; /**< 明度/亮度（0.0–1.0）。 */
} hsv_color_t;

/**
 * @brief 将 HSV 颜色转换为 RGB 颜色
 *
 * 使用标准 HSV→RGB 算法，含舍入处理以减少精度损失。
 *
 * @param[in]  hsv      HSV 颜色结构体
 * @return     color_t  RGB 颜色结构体
 */
color_t hsv_to_rgb(hsv_color_t hsv);

/**
 * @brief 将 RGB 颜色转换为 HSV 颜色
 *
 * 将 0–255 RGB 归一化到 0.0–1.0，计算色相/饱和度/明度。
 *
 * @param[in]  rgb          RGB 颜色结构体
 * @return     hsv_color_t  HSV 颜色结构体
 */
hsv_color_t rgb_to_hsv(color_t rgb);

/**
 * @brief 两个 RGB 颜色之间平滑过渡
 *
 * 在指定步数内从起始颜色插值到目标颜色，每步通过回调输出中间颜色。
 * 使用二次缓入缓出缓动函数。
 *
 * @param[in]  start           起始颜色
 * @param[in]  end             目标颜色
 * @param[in]  steps           插值步数
 * @param[in]  updateCallback  每步回调（接收中间颜色和用户数据）
 * @param[in]  userData        用户数据指针
 * @note  回调可为 NULL，此时函数立即返回
 */
void smoothcolorTransition(color_t start, color_t end, int steps,
                           void (*updateCallback)(color_t, void *), void *userData);

/**
 * @brief 生成 360 度色相渐变（彩虹效果）
 *
 * @param[in]  steps       渐变步数
 * @param[in]  brightness  明度通道（0.0–1.0）
 * @param[in]  callback    每步回调（接收颜色和步数索引）
 * @param[in]  data        用户数据指针
 */
void generate360Gradient(int steps, float brightness,
                         void (*callback)(color_t, int, void *), void *data);

/**
 * @brief 色轮动画，可配置速度
 *
 * @param[in]  speed              动画速度
 * @param[in]  updateColorOutput  每步输出回调
 * @param[in]  userData           用户数据指针
 */
void animateColorWheel(uint8_t speed,
                       void (*updateColorOutput)(color_t, void *), void *userData);

#endif /* COLOR_MODE_H */
