/**
 * @file stm32f10x_bsp_uart.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x UART 板级支持包（HAL_UART 封装）。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 */
#ifndef STM32F10X_BSP_UART_H
#define STM32F10X_BSP_UART_H

#include <stdint.h>

/**
 * @brief UART 配置结构体
 */
typedef struct {
    uint32_t baudrate;  /**< 波特率（如 115200） */
} bsp_uart_cfg_t;

/**
 * @brief 初始化 UART1（8N1，无流控）
 *
 * @param[in]  name  串口设备名（保留，当前固定使用 UART1）
 * @param[in]  cfg   串口配置指针，不可为 NULL
 * @return     int   操作状态
 *              - 0: 初始化成功
 *              - -1: 初始化失败或 cfg 为空
 */
int bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg);

/**
 * @brief 从 UART 接收一个字节（1ms 超时）
 *
 * @param[in]  name  串口设备名（保留）
 * @return     int   接收结果
 *              - 0-255: 成功，返回接收到的字节
 *              - -1: 超时或未收到数据
 */
int bsp_uart_getchar(const char *name);

/**
 * @brief 向 UART 发送一个字节（100ms 超时）
 *
 * @param[in]  name  串口设备名（保留）
 * @param[in]  ch    待发送的字节
 */
void bsp_uart_putchar(const char *name, int ch);

#endif /* STM32F10X_BSP_UART_H */
