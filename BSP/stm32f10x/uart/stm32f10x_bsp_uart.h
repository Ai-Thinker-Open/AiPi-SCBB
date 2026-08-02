#ifndef _STM32F10X_BSP_UART_H_
#define _STM32F10X_BSP_UART_H_
#include <stdint.h>
typedef struct { uint32_t baudrate; } bsp_uart_cfg_t;
int  bsp_uart_init(const char *name, bsp_uart_cfg_t *cfg);
int  bsp_uart_getchar(const char *name);
void bsp_uart_putchar(const char *name, int ch);
#endif
