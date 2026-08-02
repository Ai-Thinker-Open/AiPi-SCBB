/**
 * @file bl616_bsp_uart.h
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 UART 板级支持包（bflb_uart 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef BL616_BSP_UART_H
#define BL616_BSP_UART_H

#include <stdint.h>

/**
 * @brief UART 配置结构体
 */
typedef struct {
    uint32_t baudrate;  /**< 波特率（如 115200） */
} bsp_uart_cfg_t;

/**
 * @brief 初始化 UART（默认 UART1: TX=IO16, RX=IO17，8N1）
 *
 * @param[in]  name  串口设备名（如 "uart1"）
 * @param[in]  cfg   串口配置指针，不可为 NULL
 * @return     int   操作状态
 *              - 0: 初始化成功
 *              - -1: cfg 为空或初始化失败
 */
int bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg);

/**
 * @brief 从 UART 接收一个字节（非阻塞）
 *
 * @param[in]  name  串口设备名
 * @return     int   接收结果
 *              - 0-255: 成功，返回接收到的字节
 *              - -1: 无数据
 */
int bsp_uart_getchar(const char *name);

/**
 * @brief 向 UART 发送一个字节
 *
 * @param[in]  name  串口设备名
 * @param[in]  ch    待发送的字节
 */
void bsp_uart_putchar(const char *name, int ch);

#endif /* BL616_BSP_UART_H */
