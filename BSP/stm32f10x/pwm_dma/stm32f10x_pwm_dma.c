/**
 * @file stm32f10x_pwm_dma.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "stm32f10x_pwm_dma.h"
#include "FreeRTOS.h"
#include "log.h"
#include "portable.h"

#include "axk_ws2812.h"
#include "tim.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** 当前灯珠数量 */
static uint16_t s_bsp_ws2812_max_led_num = 0;
// 每个LED需要24位数据 + 一个RESET信号
// 使用更紧凑的表示方式，每个位只需要一个PWM周期
static uint16_t *buffer_ptr = NULL;
extern DMA_HandleTypeDef hdma_tim1_ch4_trig_com;

/**
 * @brief 初始化 PWM+DMA 驱动并分配灯条数据缓冲
 *
 * @param[in]  led_num  灯珠数量
 */
void bsp_pwm_dma_init(uint16_t led_num) {
    s_bsp_ws2812_max_led_num = led_num;
    // 重复初始化前先释放旧缓冲
    if (buffer_ptr != NULL) {
        free(buffer_ptr);
        buffer_ptr = NULL;
    }
    buffer_ptr = (uint16_t *)malloc((led_num * 24 + 1) * sizeof(uint16_t));
    if (buffer_ptr == NULL) {
        log_error("Failed to allocate memory for WS2812 buffer");
    }
}

/**
 * @brief 释放 PWM+DMA 驱动资源
 */
void bsp_pwm_dma_deinit(void) {
    if (buffer_ptr != NULL) {
        free(buffer_ptr);
        buffer_ptr = NULL;
    }
    s_bsp_ws2812_max_led_num = 0;
}

/**
 * @brief 将 RGB 颜色转换为 WS2812 时序数据（GRB 顺序）
 *
 * @param[in]  r      红色分量
 * @param[in]  g      绿色分量
 * @param[in]  b      蓝色分量
 * @param[out] buffer 时序数据缓冲区，不可为 NULL
 */
static void convert_rgb_to_ws2812_data(uint8_t r, uint8_t g, uint8_t b,
                                       uint16_t *buffer) {
    // WS2812数据格式是GRB（绿色、红色、蓝色）
    if (buffer == NULL) {
        return;
    }

    // 正确的位处理：从高位到低位
    // 绿色数据 (8位)
    for (unsigned char i = 0; i < 8; i++) {
        buffer[i] = (g & (0x80 >> i)) ? CODE1 : CODE0;
        buffer[i + 8] = (r & (0x80 >> i)) ? CODE1 : CODE0;
        buffer[i + 16] = (b & (0x80 >> i)) ? CODE1 : CODE0;
    }
}

/**
 * @brief 将灯条颜色缓冲区通过 PWM+DMA 发送到物理灯条
 *
 * @return int  操作状态
 *              - 0: 传输成功
 *              - -1: 设备未初始化或启动 DMA 失败
 */
int bsp_pwm_dma_with_num(void) {
    // 确保设备、数据缓冲均有效
    if (g_axk_ws2812_strip_dev == NULL || g_axk_ws2812_strip_dev->dev == NULL ||
        buffer_ptr == NULL) {
        log_error("WS2812 device not initialized");
        return -1;
    }

    uint16_t total_bits = 0;
    uint16_t *current_pos = buffer_ptr;

    // 为每个LED生成对应的时序数据
    uint16_t led_count = (g_axk_ws2812_strip_dev->led_count < WS2812_MAX_NUM)
                             ? g_axk_ws2812_strip_dev->led_count
                             : WS2812_MAX_NUM;

    for (uint16_t i = 0; i < led_count; i++) {
        // 应用亮度调节
        uint8_t r = (uint8_t)(g_axk_ws2812_strip_dev->dev[i].color.r *
                              g_axk_ws2812_strip_dev->brightness);
        uint8_t g = (uint8_t)(g_axk_ws2812_strip_dev->dev[i].color.g *
                              g_axk_ws2812_strip_dev->brightness);
        uint8_t b = (uint8_t)(g_axk_ws2812_strip_dev->dev[i].color.b *
                              g_axk_ws2812_strip_dev->brightness);

        // 转换RGB颜色为WS2812时序数据
        convert_rgb_to_ws2812_data(r, g, b, current_pos);

        // 移动到下一个LED的位置（每个LED需要24个时隙）
        current_pos += 24;
        total_bits += 24;
    }

    // 添加RESET信号（至少50us低电平）
    buffer_ptr[total_bits] = CODE_RESET;
    total_bits++;

    // 确保在启动新的DMA传输前停止之前的传输
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);

    // 启动DMA传输
    if (HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)buffer_ptr,
                              total_bits) != HAL_OK) {
        log_error("Failed to start DMA transmission");
        return -1;
    }

    return 0;
}
