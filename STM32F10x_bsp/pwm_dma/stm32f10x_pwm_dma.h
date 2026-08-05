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
#ifndef __STM32F10X_PWM_DMA_H__
#define __STM32F10X_PWM_DMA_H__
#include <stdint.h>
typedef struct {
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ws2812_color_t;

#define CODE0 30
#define CODE1 60
#define CODE_RESET 0
#define WS2812_MAX_NUM 60
int bsp_pwm_dma_with_num(void);
void bsp_pwm_dma_init(uint16_t led_num);
void bsp_pwm_dma_deinit(void);
#endif