/**
 * STM32F10x GPIO BSP — HAL_GPIO 封装
 * pin 参数直接映射到 GPIO_PIN_x (0=GPIO_PIN_0, 1=GPIO_PIN_1, ...)
 * 所有 pin 默认使用 GPIOA
 */
#include "stm32f10x_bsp_gpio.h"
#include "stm32f1xx_hal.h"

static GPIO_InitTypeDef gi;

void bsp_gpio_init(uint8_t pin, uint32_t mode) {
    gi.Pin = (1 << pin);
    if (mode == 1 || mode == 2 || mode == 3) {
        gi.Mode = GPIO_MODE_OUTPUT_PP;
        gi.Pull = (mode == 3) ? GPIO_PULLUP : GPIO_NOPULL;
        gi.Speed = GPIO_SPEED_FREQ_HIGH;
    } else {
        gi.Mode = (mode == 4) ? GPIO_MODE_INPUT : GPIO_MODE_INPUT;
        gi.Pull = (mode == 4) ? GPIO_PULLUP : GPIO_NOPULL;
    }
    HAL_GPIO_Init(GPIOA, &gi);
}

void bsp_gpio_set(uint8_t pin)    { GPIOA->BSRR = (1 << pin); }
void bsp_gpio_reset(uint8_t pin)  { GPIOA->BRR = (1 << pin); }
int  bsp_gpio_read(uint8_t pin)   { return (GPIOA->IDR >> pin) & 1; }
