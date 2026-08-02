/**
 * BL616/BL618 BSP UART — 包 bflb_uart
 */
#include "bl616_bsp_uart.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"

static struct bflb_device_s *dev;

int bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg) {
    dev = bflb_device_get_by_name(name);
    struct bflb_uart_config_s c = {
        .baudrate = cfg->baudrate,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .bit_order = UART_LSB_FIRST,
        .flow_ctrl = UART_FLOWCTRL_NONE,
    };
    bflb_uart_init(dev, &c);
    /* 默认 UART1: TX=IO16, RX=IO17 */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, 16, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, 17, GPIO_UART_FUNC_UART1_RX);
    return 0;
}

int bsp_uart_getchar(const char *name) {
    int c = bflb_uart_getchar(dev);
    return (c >= 0) ? c : -1;
}

void bsp_uart_putchar(const char *name, int ch) {
    bflb_uart_putchar(dev, ch);
}
