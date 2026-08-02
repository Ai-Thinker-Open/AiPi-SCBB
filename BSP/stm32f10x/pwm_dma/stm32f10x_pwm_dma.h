/**
 * @file stm32f10x_pwm_dma.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef STM32F10X_PWM_DMA_H
#define STM32F10X_PWM_DMA_H
#include <stdint.h>

/**
 * @brief WS2812 颜色结构体（RGB）
 */
typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ws2812_color_t;

/** @brief 单 bit 0 的 PWM 占空比值 */
#define CODE0 30
/** @brief 单 bit 1 的 PWM 占空比值 */
#define CODE1 60
/** @brief RESET 信号的 PWM 占空比值（低电平） */
#define CODE_RESET 0
/** @brief WS2812 灯珠数量上限 */
#define WS2812_MAX_NUM 60

/**
 * @brief 将灯条颜色缓冲区通过 PWM+DMA 发送到物理灯条
 *
 * @return int  操作状态
 *              - 0: 传输成功
 *              - -1: 设备未初始化或启动 DMA 失败
 */
int bsp_pwm_dma_with_num(void);

/**
 * @brief 初始化 PWM+DMA 驱动并分配灯条数据缓冲
 *
 * @param[in]  led_num  灯珠数量
 */
void bsp_pwm_dma_init(uint16_t led_num);

/**
 * @brief 释放 PWM+DMA 驱动资源
 */
void bsp_pwm_dma_deinit(void);
#endif /* STM32F10X_PWM_DMA_H */
