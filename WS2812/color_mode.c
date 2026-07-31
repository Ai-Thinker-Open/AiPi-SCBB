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
 * @return     color_t  RGB 颜色结构体
 */
color_t hsv_to_rgb(hsv_color_t hsv) {
    float h = hsv.h;
    float s = hsv.s;
    float v = hsv.v;

    int hi = (int)(h / 60) % 6;
    float f = h / 60.0 - hi;

    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    color_t rgb;
    switch (hi) {
    case 0: rgb.r = v * 255; rgb.g = t * 255; rgb.b = p * 255; break;
    case 1: rgb.r = q * 255; rgb.g = v * 255; rgb.b = p * 255; break;
    case 2: rgb.r = p * 255; rgb.g = v * 255; rgb.b = t * 255; break;
    case 3: rgb.r = p * 255; rgb.g = q * 255; rgb.b = v * 255; break;
    case 4: rgb.r = t * 255; rgb.g = p * 255; rgb.b = v * 255; break;
    case 5: rgb.r = v * 255; rgb.g = p * 255; rgb.b = q * 255; break;
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
 * @return     hsv_color_t  HSV 颜色结构体
 */
hsv_color_t rgb_to_hsv(color_t rgb) {
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float max = fmax(r, fmax(g, b));
    float min = fmin(r, fmin(g, b));
    float delta = max - min;
    hsv_color_t hsv;
    float divisor = delta > 0 ? delta : 1e-6f;

    if (delta == 0) {
        hsv.h = 0;
    } else if (max == r) {
        hsv.h = 60 * (fmod((g - b) / divisor, 6));
        if (hsv.h < 0) hsv.h += 360;
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
 * @return     color_t 插值结果颜色
 */
static color_t interpolatecolor_t(color_t start, color_t end, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    color_t result;
    result.r = (unsigned char)(start.r + (end.r - start.r) * t);
    result.g = (unsigned char)(start.g + (end.g - start.g) * t);
    result.b = (unsigned char)(start.b + (end.b - start.b) * t);
    return result;
}

/* ── 缓动函数（ease functions） ── */
static float easeLinear(float t)     { return t; }
static float easeInQuad(float t)     { return t * t; }
static float easeOutQuad(float t)    { return -t * (t - 2); }

static float easeInOutQuad(float t) {
    if (t < 0.5f) return 2 * t * t;
    return -1 + (4 - 2 * t) * t;
}

static float easeInCubic(float t)    { return t * t * t; }

static float easeOutCubic(float t) {
    t--;
    return t * t * t + 1;
}

static float easeOutElastic(float t) {
    if (t == 0 || t == 1) return t;
    return pow(2, -10 * t) * sin((t * 10 - 0.75) * (2 * M_PI) / 3) + 1;
}

static float easeOutBounce(float t) {
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
void smoothcolorTransition(color_t start, color_t end, int steps,
                           void (*updateCallback)(color_t, void *), void *userData) {
    if (updateCallback == NULL) return;
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / steps;
        float easedT = easeInOutQuad(t);
        color_t current = interpolatecolor_t(start, end, easedT);
        updateCallback(current, userData);
    }
}

/* generate360Gradient 和 animateColorWheel 函数尚未实现 */
