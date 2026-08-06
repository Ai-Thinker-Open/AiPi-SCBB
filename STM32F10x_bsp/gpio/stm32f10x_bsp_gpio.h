#ifndef _STM32F10X_BSP_GPIO_H_
#define _STM32F10X_BSP_GPIO_H_
#include <stdint.h>
/**
 * pin 参数编码:低 4 位为引脚号(0~15),高 4 位为端口号(0=GPIOA, 1=GPIOB, 2=GPIOC, 3=GPIOD)。
 * 例如:0x01 = PA1, 0x0B = PB11。取值 0~15 时默认使用 GPIOA。
 * mode 参数:1/2/3 = 推挽输出(3 带上拉),4 = 上拉输入,其他 = 浮空输入
 */
void bsp_gpio_init(uint8_t pin, uint32_t mode);
void bsp_gpio_set(uint8_t pin);
void bsp_gpio_reset(uint8_t pin);
int  bsp_gpio_read(uint8_t pin);
#endif
