/**
 * @file bl616_bsp_gpio.c
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 GPIO 板级支持包实现（bflb_gpio 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "bl616_bsp_gpio.h"
#include "bflb_gpio.h"

/** GPIO 设备句柄 */
static struct bflb_device_s *s_bsp_gpio_dev;

/**
 * @brief 初始化 GPIO 引脚
 *
 * @param[in]  pin   GPIO 引脚号
 * @param[in]  mode  模式（BSP_GPIO_MODE_*）
 */
void bsp_gpio_init(uint8_t pin, uint32_t mode) {
    if (s_bsp_gpio_dev == NULL) {
        s_bsp_gpio_dev = bflb_device_get_by_name("gpio");
    }
    if (mode == BSP_GPIO_MODE_SPI_MOSI) {
        bflb_gpio_init(s_bsp_gpio_dev, pin,
                       GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    } else if (mode == BSP_GPIO_MODE_SPI_SCLK) {
        bflb_gpio_init(s_bsp_gpio_dev, pin,
                       GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    } else if ((mode & 1) != 0) {
        /* 输出模式：推挽输出，带上拉 */
        bflb_gpio_init(s_bsp_gpio_dev, pin,
                       GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    } else if (mode == BSP_GPIO_MODE_IN_PU) {
        bflb_gpio_init(s_bsp_gpio_dev, pin,
                       GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    } else {
        /* 浮空输入 */
        bflb_gpio_init(s_bsp_gpio_dev, pin, GPIO_INPUT);
    }
}

/**
 * @brief 输出高电平
 *
 * @param[in]  pin  GPIO 引脚号
 */
void bsp_gpio_set(uint8_t pin) {
    bflb_gpio_set(s_bsp_gpio_dev, pin);
}

/**
 * @brief 输出低电平
 *
 * @param[in]  pin  GPIO 引脚号
 */
void bsp_gpio_reset(uint8_t pin) {
    bflb_gpio_reset(s_bsp_gpio_dev, pin);
}

/**
 * @brief 读取引脚电平
 *
 * @param[in]  pin  GPIO 引脚号
 * @return     int  引脚电平
 *              - 1: 高电平
 *              - 0: 低电平
 */
int bsp_gpio_read(uint8_t pin) {
    return bflb_gpio_read(s_bsp_gpio_dev, pin) ? 1 : 0;
}
