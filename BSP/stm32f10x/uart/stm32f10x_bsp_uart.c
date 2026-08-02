/**
 * STM32F10x UART BSP — HAL_UART 封装
 */
#include "stm32f10x_bsp_uart.h"
#include "stm32f1xx_hal.h"

static UART_HandleTypeDef huart1;

int bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg) {
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

int bsp_uart_getchar(const char *name) {
    uint8_t ch;
    if (HAL_UART_Receive(&huart1, &ch, 1, 1) == HAL_OK) return ch;
    return -1;
}

void bsp_uart_putchar(const char *name, int ch) {
    uint8_t c = ch;
    HAL_UART_Transmit(&huart1, &c, 1, 100);
}
