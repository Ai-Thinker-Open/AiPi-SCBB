#ifndef _BL616_BSP_GPIO_H_
#define _BL616_BSP_GPIO_H_
#include <stdint.h>

#define BSP_GPIO_MODE_OUT  1   /* 推挽输出 */
#define BSP_GPIO_MODE_SPI_MOSI  2   /* SPI MOSI (PULLDOWN) */
#define BSP_GPIO_MODE_SPI_SCLK  3   /* SPI SCLK (PULLUP, Mode3 空闲高) */
#define BSP_GPIO_MODE_IN      0   /* 输入(浮空) */
#define BSP_GPIO_MODE_IN_PU   4   /* 输入(上拉) */

void bsp_gpio_init(uint8_t pin, uint32_t mode);
void bsp_gpio_set(uint8_t pin);
void bsp_gpio_reset(uint8_t pin);
int  bsp_gpio_read(uint8_t pin);
#endif
