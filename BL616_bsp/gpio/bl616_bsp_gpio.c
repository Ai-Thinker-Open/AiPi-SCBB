#include "bl616_bsp_gpio.h"
#include "bflb_gpio.h"
static struct bflb_device_s *g;
void bsp_gpio_init(uint8_t pin, uint32_t mode) {
    if(!g) g=bflb_device_get_by_name("gpio");
    if(mode==BSP_GPIO_MODE_SPI_MOSI) bflb_gpio_init(g, pin, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    else if(mode==BSP_GPIO_MODE_SPI_SCLK) bflb_gpio_init(g, pin, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    else if(mode&1) bflb_gpio_init(g, pin, GPIO_OUTPUT | GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    else if(mode==BSP_GPIO_MODE_IN_PU) bflb_gpio_init(g, pin, GPIO_INPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    else bflb_gpio_init(g, pin, GPIO_INPUT);
}
void bsp_gpio_set(uint8_t pin)   { bflb_gpio_set(g,pin); }
void bsp_gpio_reset(uint8_t pin) { bflb_gpio_reset(g,pin); }
int  bsp_gpio_read(uint8_t pin)  { return bflb_gpio_read(g,pin) ? 1 : 0; }
