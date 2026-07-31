#ifndef _STM32F10X_BSP_GPIO_H_
#define _STM32F10X_BSP_GPIO_H_
#include <stdint.h>
void bsp_gpio_init(uint8_t pin, uint32_t mode);
void bsp_gpio_set(uint8_t pin);
void bsp_gpio_reset(uint8_t pin);
int  bsp_gpio_read(uint8_t pin);
#endif
