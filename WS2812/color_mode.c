/**
 * @file color_mode.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief HSV/RGB 颜色转换与动画实现。
 * @version 0.1
 * @date 2025-07-10
 * @copyright Copyright (c) 2025
 */
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "color_mode.h"

/**
 * @brief 将 HSV 颜色转换为 RGB 颜色
 *
 * @param[in]  hsv      HSV 颜色结构体
 * @return     axk_color_t  RGB 颜色结构体
 */
axk_color_t axk_hsv_to_rgb(axk_hsv_color_t hsv) {
    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;

    int hi = (int)(h / 60) % 6;
    float f = h / 60.0 - hi;

    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    axk_color_t rgb;
    switch (hi) {
    case 0:
        rgb.r = v * 255;
        rgb.g = t * 255;
        rgb.b = p * 255;
        break;
    case 1:
        rgb.r = q * 255;
        rgb.g = v * 255;
        rgb.b = p * 255;
        break;
    case 2:
        rgb.r = p * 255;
        rgb.g = v * 255;
        rgb.b = t * 255;
        break;
    case 3:
        rgb.r = p * 255;
        rgb.g = q * 255;
        rgb.b = v * 255;
        break;
    case 4:
        rgb.r = t * 255;
        rgb.g = p * 255;
        rgb.b = v * 255;
        break;
    case 5:
        rgb.r = v * 255;
        rgb.g = p * 255;
        rgb.b = q * 255;
        break;
    }

    rgb.r = (uint8_t)(rgb.r + 0.5f);
    rgb.g = (uint8_t)(rgb.g + 0.5f);
    rgb.b = (uint8_t)(rgb.b + 0.5f);
    return rgb;
}

/**
 * @brief 将 RGB 颜色转换为 HSV 颜色
 *
 * @param[in]  rgb          RGB 颜色结构体
 * @return     axk_hsv_color_t  HSV 颜色结构体
 */
axk_hsv_color_t axk_rgb_to_hsv(axk_color_t rgb) {
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float max = fmax(r, fmax(g, b));
    float min = fmin(r, fmin(g, b));
    float delta = max - min;
    axk_hsv_color_t hsv;
    float divisor = delta > 0 ? delta : 1e-6f;

    if (delta == 0) {
        hsv.h = 0;
    } else if (max == r) {
        hsv.h = 60 * (fmod((g - b) / divisor, 6));
        if (hsv.h < 0) {
            hsv.h += 360;
        }
    } else if (max == g) {
        hsv.h = 60 * ((b - r) / divisor + 2);
    } else {
        hsv.h = 60 * ((r - g) / divisor + 4);
    }

    hsv.s = (max == 0) ? 0 : (delta / max);
    hsv.v = max;
    return hsv;
}

/**
 * @brief 两个 RGB 颜色之间线性插值
 *
 * @param[in]  start   起始颜色
 * @param[in]  end     目标颜色
 * @param[in]  t       插值系数（0.0–1.0）
 * @return     axk_color_t 插值结果颜色
 */
static axk_color_t axk_interpolate_color(axk_color_t start, axk_color_t end, float t) {
    if (t < 0.0f) {
        t = 0.0f;
    }
    if (t > 1.0f) {
        t = 1.0f;
    }
    axk_color_t result;
    result.r = (unsigned char)(start.r + (end.r - start.r) * t);
    result.g = (unsigned char)(start.g + (end.g - start.g) * t);
    result.b = (unsigned char)(start.b + (end.b - start.b) * t);
    return result;
}

/* ── 缓动函数（ease functions） ── */

/**
 * @brief 线性缓动
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_linear(float t) {
    return t;
}

/**
 * @brief 二次方缓入
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_in_quad(float t) {
    return t * t;
}

/**
 * @brief 二次方缓出
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_out_quad(float t) {
    return -t * (t - 2);
}

/**
 * @brief 二次方缓入缓出
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_in_out_quad(float t) {
    if (t < 0.5f) {
        return 2 * t * t;
    }
    return -1 + (4 - 2 * t) * t;
}

/**
 * @brief 三次方缓入
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_in_cubic(float t) {
    return t * t * t;
}

/**
 * @brief 三次方缓出
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_out_cubic(float t) {
    t--;
    return t * t * t + 1;
}

/**
 * @brief 弹性缓出
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_out_elastic(float t) {
    if (t == 0 || t == 1) {
        return t;
    }
    return pow(2, -10 * t) * sin((t * 10 - 0.75) * (2 * M_PI) / 3) + 1;
}

/**
 * @brief 回弹缓出
 *
 * @param[in]  t      插值系数（0.0-1.0）
 * @return     float  缓动结果
 */
static float axk_ease_out_bounce(float t) {
    if (t < 1 / 2.75) {
        return 7.5625 * t * t;
    } else if (t < 2 / 2.75) {
        t -= 1.5 / 2.75;
        return 7.5625 * t * t + 0.75;
    } else if (t < 2.5 / 2.75) {
        t -= 2.25 / 2.75;
        return 7.5625 * t * t + 0.9375;
    } else {
        t -= 2.625 / 2.75;
        return 7.5625 * t * t + 0.984375;
    }
}

/**
 * @brief 两颜色间平滑过渡，二次缓入缓出缓动
 *
 * @param[in]  start           起始颜色
 * @param[in]  end             目标颜色
 * @param[in]  steps           插值步数
 * @param[in]  updateCallback  每步回调（接收中间颜色和用户数据）
 * @param[in]  userData        用户数据指针
 */
void axk_smooth_color_transition(axk_color_t start, axk_color_t end, int steps,
                                 void (*updateCallback)(axk_color_t, void *), void *userData) {
    if (updateCallback == NULL || steps <= 0) {
        return;
    }
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        float easedT = axk_ease_in_out_quad(t);
        axk_color_t current = axk_interpolate_color(start, end, easedT);
        updateCallback(current, userData);
    }
}

/* axk_generate_360_gradient 和 axk_animate_color_wheel 函数尚未实现 */
