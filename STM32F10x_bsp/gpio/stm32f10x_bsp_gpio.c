/**
 * STM32F10x GPIO BSP — HAL_GPIO 封装
 * pin 参数编码:低 4 位为引脚号(0~15),高 4 位为端口号(0=GPIOA, 1=GPIOB, 2=GPIOC, 3=GPIOD)。
 * 例如:0x01 = PA1, 0x0B = PB11。取值 0~15 时默认使用 GPIOA(向后兼容)。
 * mode 参数:1/2/3 = 推挽输出(3 带上拉),4 = 上拉输入,其他 = 浮空输入
 */
#include "stm32f10x_bsp_gpio.h"
#include "stm32f1xx_hal.h"

static GPIO_InitTypeDef gi;

/** @brief 根据 pin 编码获取 GPIO 端口(并使能对应端口时钟)。
 *
 *  @param[in]  pin  引脚编码(高 4 位端口号,低 4 位引脚号)
 *  @return     GPIO_TypeDef*  GPIO 端口指针
 */
static GPIO_TypeDef *gpio_get_port(uint8_t pin) {
    switch (pin >> 4) {
        case 1: __HAL_RCC_GPIOB_CLK_ENABLE(); return GPIOB;
        case 2: __HAL_RCC_GPIOC_CLK_ENABLE(); return GPIOC;
        case 3: __HAL_RCC_GPIOD_CLK_ENABLE(); return GPIOD;
        default: __HAL_RCC_GPIOA_CLK_ENABLE(); return GPIOA;
    }
}

void bsp_gpio_init(uint8_t pin, uint32_t mode) {
    gi.Pin = (1 << (pin & 0x0F));
    if (mode == 1 || mode == 2 || mode == 3) {
        gi.Mode = GPIO_MODE_OUTPUT_PP;
        gi.Pull = (mode == 3) ? GPIO_PULLUP : GPIO_NOPULL;
        gi.Speed = GPIO_SPEED_FREQ_HIGH;
    } else {
        gi.Mode = (mode == 4) ? GPIO_MODE_INPUT : GPIO_MODE_INPUT;
        gi.Pull = (mode == 4) ? GPIO_PULLUP : GPIO_NOPULL;
    }
    HAL_GPIO_Init(gpio_get_port(pin), &gi);
}

void bsp_gpio_set(uint8_t pin) {
    gpio_get_port(pin)->BSRR = (1 << (pin & 0x0F));
}

void bsp_gpio_reset(uint8_t pin) {
    gpio_get_port(pin)->BRR = (1 << (pin & 0x0F));
}

int bsp_gpio_read(uint8_t pin) {
    return (gpio_get_port(pin)->IDR >> (pin & 0x0F)) & 1;
}
