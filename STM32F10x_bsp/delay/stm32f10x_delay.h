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
#ifndef __STM32F10X_DELAY_H__
#define __STM32F10X_DELAY_H__
#include "stm32f1xx_hal.h"

void HAL_Delay_us(uint32_t us);
void HAL_Delay(uint32_t Delay);
#endif /* __STM32F10X_DELAY_H__ */