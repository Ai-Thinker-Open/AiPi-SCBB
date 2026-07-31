/**
 * @file axk_rd03_v2.h
 * @brief Rd-03_V2 毫米波雷达 — 串口接收, "distance:XXX"=有人, "OFF"=无人
 */
#ifndef _AXK_RD03_V2_H_
#define _AXK_RD03_V2_H_

#include "scbb_config.h"

#ifdef SCBB_RD03_V2_ENABLED

#ifdef SCBB_RD03_V2_UART_HEADER
#include SCBB_RD03_V2_UART_HEADER
#endif
#ifdef SCBB_RD03_V2_DELAY_HEADER
#include SCBB_RD03_V2_DELAY_HEADER
#endif

#include <stdint.h>
#include <stdbool.h>

#ifndef AXK_RD03_V2_UART
#define AXK_RD03_V2_UART  "uart1"
#endif

int  axk_rd03_v2_init(void);
int  axk_rd03_v2_read(int *distance);  /* 返回0=无人, >0=距离cm */

#endif
#endif
