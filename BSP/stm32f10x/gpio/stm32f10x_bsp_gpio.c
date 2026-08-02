/**
 * @file stm32f10x_bsp_gpio.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x GPIO 板级支持包实现。
 *
 * pin 参数直接映射到 GPIO_PIN_x（0=GPIO_PIN_0, 1=GPIO_PIN_1, ...），
 * 所有引脚默认使用 GPIOA。
 *
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 */
#include "stm32f10x_bsp_gpio.h"
#include "stm32f1xx_hal.h"

static GPIO_InitTypeDef gi;

/**
 * @brief 初始化 GPIO 引脚（默认 GPIOA）
 *
 * @param[in]  pin   GPIO 引脚号（0-15）
 * @param[in]  mode  模式（1-3: 推挽输出，4: 上拉输入，其他: 浮空输入）
 */
void bsp_gpio_init(uint8_t pin, uint32_t mode) {
    if (pin >= 16) {
        return;
    }

    gi.Pin = (1u << pin);
    if (mode == 1 || mode == 2 || mode == 3) {
        gi.Mode = GPIO_MODE_OUTPUT_PP;
        gi.Pull = (mode == 3) ? GPIO_PULLUP : GPIO_NOPULL;
        gi.Speed = GPIO_SPEED_FREQ_HIGH;
    } else {
        gi.Mode = GPIO_MODE_INPUT;
        gi.Pull = (mode == 4) ? GPIO_PULLUP : GPIO_NOPULL;
    }
    HAL_GPIO_Init(GPIOA, &gi);
}

/**
 * @brief 输出高电平
 *
 * @param[in]  pin  GPIO 引脚号（0-15）
 */
void bsp_gpio_set(uint8_t pin) {
    if (pin >= 16) {
        return;
    }
    GPIOA->BSRR = (1u << pin);
}

/**
 * @brief 输出低电平
 *
 * @param[in]  pin  GPIO 引脚号（0-15）
 */
void bsp_gpio_reset(uint8_t pin) {
    if (pin >= 16) {
        return;
    }
    GPIOA->BRR = (1u << pin);
}

/**
 * @brief 读取引脚电平
 *
 * @param[in]  pin  GPIO 引脚号（0-15）
 * @return     int  引脚电平
 *              - 1: 高电平
 *              - 0: 低电平
 */
int bsp_gpio_read(uint8_t pin) {
    if (pin >= 16) {
        return 0;
    }
    return (GPIOA->IDR >> pin) & 1;
}
