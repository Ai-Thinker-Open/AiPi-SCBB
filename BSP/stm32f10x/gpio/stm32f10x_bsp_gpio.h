/**
 * @file stm32f10x_bsp_gpio.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x GPIO 板级支持包（默认 GPIOA）。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 */
#ifndef STM32F10X_BSP_GPIO_H
#define STM32F10X_BSP_GPIO_H

#include <stdint.h>

/**
 * @brief 初始化 GPIO 引脚（默认 GPIOA）
 *
 * @param[in]  pin   GPIO 引脚号（0-15）
 * @param[in]  mode  模式（1-3: 推挽输出，4: 上拉输入，其他: 浮空输入）
 */
void bsp_gpio_init(uint8_t pin, uint32_t mode);

/**
 * @brief 输出高电平
 *
 * @param[in]  pin  GPIO 引脚号（0-15）
 */
void bsp_gpio_set(uint8_t pin);

/**
 * @brief 输出低电平
 *
 * @param[in]  pin  GPIO 引脚号（0-15）
 */
void bsp_gpio_reset(uint8_t pin);

/**
 * @brief 读取引脚电平
 *
 * @param[in]  pin  GPIO 引脚号（0-15）
 * @return     int  引脚电平
 *              - 1: 高电平
 *              - 0: 低电平
 */
int bsp_gpio_read(uint8_t pin);

#endif /* STM32F10X_BSP_GPIO_H */
