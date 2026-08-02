/**
 * @file bl616_bsp_uart.c
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 UART 板级支持包实现（bflb_uart 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "bl616_bsp_uart.h"
#include "bflb_uart.h"
#include "bflb_gpio.h"

/** UART 设备句柄 */
static struct bflb_device_s *s_bsp_uart_dev;

/**
 * @brief 初始化 UART（默认 UART1: TX=IO16, RX=IO17，8N1）
 *
 * @param[in]  name  串口设备名（如 "uart1"）
 * @param[in]  cfg   串口配置指针，不可为 NULL
 * @return     int   操作状态
 *              - 0: 初始化成功
 *              - -1: cfg 为空
 */
int bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg) {
    if (cfg == NULL) {
        return -1;
    }

    s_bsp_uart_dev = bflb_device_get_by_name(name);
    struct bflb_uart_config_s c = {
        .baudrate = cfg->baudrate,
        .data_bits = UART_DATA_BITS_8,
        .stop_bits = UART_STOP_BITS_1,
        .parity = UART_PARITY_NONE,
        .bit_order = UART_LSB_FIRST,
        .flow_ctrl = UART_FLOWCTRL_NONE,
    };
    bflb_uart_init(s_bsp_uart_dev, &c);

    /* 默认 UART1: TX=IO16, RX=IO17 */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, 16, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, 17, GPIO_UART_FUNC_UART1_RX);
    return 0;
}

/**
 * @brief 从 UART 接收一个字节（非阻塞）
 *
 * @param[in]  name  串口设备名
 * @return     int   接收结果
 *              - 0-255: 成功，返回接收到的字节
 *              - -1: 无数据
 */
int bsp_uart_getchar(const char *name) {
    (void)name;

    int c = bflb_uart_getchar(s_bsp_uart_dev);
    return (c >= 0) ? c : -1;
}

/**
 * @brief 向 UART 发送一个字节
 *
 * @param[in]  name  串口设备名
 * @param[in]  ch    待发送的字节
 */
void bsp_uart_putchar(const char *name, int ch) {
    (void)name;

    bflb_uart_putchar(s_bsp_uart_dev, ch);
}
