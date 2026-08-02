/**
 * @file bl616_bsp_gpio.h
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 GPIO 板级支持包（bflb_gpio 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef BL616_BSP_GPIO_H
#define BL616_BSP_GPIO_H

#include <stdint.h>

/** @brief 推挽输出模式 */
#define BSP_GPIO_MODE_OUT 1
/** @brief SPI MOSI 复用模式（下拉） */
#define BSP_GPIO_MODE_SPI_MOSI 2
/** @brief SPI SCLK 复用模式（上拉，Mode3 空闲高） */
#define BSP_GPIO_MODE_SPI_SCLK 3
/** @brief 浮空输入模式 */
#define BSP_GPIO_MODE_IN 0
/** @brief 上拉输入模式 */
#define BSP_GPIO_MODE_IN_PU 4

/**
 * @brief 初始化 GPIO 引脚
 *
 * @param[in]  pin   GPIO 引脚号
 * @param[in]  mode  模式（BSP_GPIO_MODE_*）
 */
void bsp_gpio_init(uint8_t pin, uint32_t mode);

/**
 * @brief 输出高电平
 *
 * @param[in]  pin  GPIO 引脚号
 */
void bsp_gpio_set(uint8_t pin);

/**
 * @brief 输出低电平
 *
 * @param[in]  pin  GPIO 引脚号
 */
void bsp_gpio_reset(uint8_t pin);

/**
 * @brief 读取引脚电平
 *
 * @param[in]  pin  GPIO 引脚号
 * @return     int  引脚电平
 *              - 1: 高电平
 *              - 0: 低电平
 */
int bsp_gpio_read(uint8_t pin);

#endif /* BL616_BSP_GPIO_H */
