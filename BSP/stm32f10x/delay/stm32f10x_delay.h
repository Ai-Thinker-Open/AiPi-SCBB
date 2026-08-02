/**
 * @file stm32f10x_delay.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef STM32F10X_DELAY_H
#define STM32F10X_DELAY_H
#include "stm32f1xx_hal.h"

/**
 * @brief 微秒级延时（基于 SysTick 轮询）
 *
 * @param[in]  us  延时微秒数
 */
void HAL_Delay_us(uint32_t us);

/**
 * @brief 毫秒级延时
 *
 * @param[in]  Delay  延时毫秒数
 */
void HAL_Delay(uint32_t Delay);
#endif /* STM32F10X_DELAY_H */
