/**
 * @file stm32f10x_bsp_uart.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x UART 板级支持包实现。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 */
#include "stm32f10x_bsp_uart.h"
#include "stm32f1xx_hal.h"

static UART_HandleTypeDef huart1;

/**
 * @brief 初始化 UART1（8N1，无流控）
 *
 * @param[in]  name  串口设备名（保留，当前固定使用 UART1）
 * @param[in]  cfg   串口配置指针，不可为 NULL
 * @return     int   操作状态
 *              - 0: 初始化成功
 *              - -1: 初始化失败或 cfg 为空
 */
int bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg) {
    if (cfg == NULL) {
        return -1;
    }
    (void)name;

    huart1.Instance = USART1;
    huart1.Init.BaudRate = cfg->baudrate;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    return HAL_UART_Init(&huart1) == HAL_OK ? 0 : -1;
}

/**
 * @brief 从 UART 接收一个字节（1ms 超时）
 *
 * @param[in]  name  串口设备名（保留）
 * @return     int   接收结果
 *              - 0-255: 成功，返回接收到的字节
 *              - -1: 超时或未收到数据
 */
int bsp_uart_getchar(const char *name) {
    uint8_t ch;
    (void)name;

    if (HAL_UART_Receive(&huart1, &ch, 1, 1) == HAL_OK) {
        return ch;
    }
    return -1;
}

/**
 * @brief 向 UART 发送一个字节（100ms 超时）
 *
 * @param[in]  name  串口设备名（保留）
 * @param[in]  ch    待发送的字节
 */
void bsp_uart_putchar(const char *name, int ch) {
    uint8_t c = (uint8_t)ch;
    (void)name;

    HAL_UART_Transmit(&huart1, &c, 1, 100);
}
