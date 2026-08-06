/**
 * @file stm32f10x_bsp_gpio.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x GPIO 板级支持包实现。
 *
 * pin 参数编码:低 4 位为引脚号(0~15),高 4 位为端口号(0=GPIOA, 1=GPIOB, 2=GPIOC, 3=GPIOD)。
 * 例如:0x01 = PA1, 0x1E = PB14。取值 0~15 时默认使用 GPIOA(向后兼容)。
 * mode 参数:1/2/3 = 推挽输出(3 带上拉),4 = 上拉输入,其他 = 浮空输入
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
 * @brief 根据 pin 编码获取 GPIO 端口（并使能对应端口时钟）。
 *
 * @param[in]  pin  引脚编码（高 4 位端口号，低 4 位引脚号）
 * @return     GPIO_TypeDef*  GPIO 端口指针
 */
static GPIO_TypeDef *gpio_get_port(uint8_t pin) {
    switch (pin >> 4) {
        case 1: __HAL_RCC_GPIOB_CLK_ENABLE(); return GPIOB;
        case 2: __HAL_RCC_GPIOC_CLK_ENABLE(); return GPIOC;
        case 3: __HAL_RCC_GPIOD_CLK_ENABLE(); return GPIOD;
        default: __HAL_RCC_GPIOA_CLK_ENABLE(); return GPIOA;
    }
}

/**
 * @brief 初始化 GPIO 引脚
 *
 * @param[in]  pin   引脚编码（低 4 位引脚号，高 4 位端口号；0-15 默认 GPIOA）
 * @param[in]  mode  模式（1-3: 推挽输出，4: 上拉输入，其他: 浮空输入）
 */
void bsp_gpio_init(uint8_t pin, uint32_t mode) {
    gi.Pin = (1u << (pin & 0x0F));
    if (mode == 1 || mode == 2 || mode == 3) {
        gi.Mode = GPIO_MODE_OUTPUT_PP;
        gi.Pull = (mode == 3) ? GPIO_PULLUP : GPIO_NOPULL;
        gi.Speed = GPIO_SPEED_FREQ_HIGH;
    } else {
        gi.Mode = GPIO_MODE_INPUT;
        gi.Pull = (mode == 4) ? GPIO_PULLUP : GPIO_NOPULL;
    }
    HAL_GPIO_Init(gpio_get_port(pin), &gi);
}

/**
 * @brief 输出高电平
 *
 * @param[in]  pin  引脚编码（低 4 位引脚号，高 4 位端口号；0-15 默认 GPIOA）
 */
void bsp_gpio_set(uint8_t pin) {
    gpio_get_port(pin)->BSRR = (1u << (pin & 0x0F));
}

/**
 * @brief 输出低电平
 *
 * @param[in]  pin  引脚编码（低 4 位引脚号，高 4 位端口号；0-15 默认 GPIOA）
 */
void bsp_gpio_reset(uint8_t pin) {
    gpio_get_port(pin)->BRR = (1u << (pin & 0x0F));
}

/**
 * @brief 读取引脚电平
 *
 * @param[in]  pin  引脚编码（低 4 位引脚号，高 4 位端口号；0-15 默认 GPIOA）
 * @return     int  引脚电平
 *              - 1: 高电平
 *              - 0: 低电平
 */
int bsp_gpio_read(uint8_t pin) {
    return (gpio_get_port(pin)->IDR >> (pin & 0x0F)) & 1;
}
