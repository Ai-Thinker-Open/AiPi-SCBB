/**
 * @file axk_rd03_v2.h
 * @brief Rd-03_V2 毫米波雷达传感器 — 仅 UART 接收。
 *
 * 解析 "distance:XXX" 帧（有人，距离 cm）和 "OFF" 帧（无人）。
 * 跨平台：纯 ACLL，UART + Delay BSP 通过 scbb_config.h 选取。
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef AXK_RD03_V2_H
#define AXK_RD03_V2_H

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

/** @brief UART 设备名（可覆盖） */
#ifndef AXK_RD03_V2_UART
#define AXK_RD03_V2_UART  "uart1"
#endif

/**
 * @brief 初始化雷达 UART，波特率 115200
 *
 * @return int  0: 成功
 */
int  axk_rd03_v2_init(void);

/**
 * @brief 从 UART 读取一行并解析距离
 *
 * @param[out] *distance  距离输出指针（cm），不可为 NULL
 * @return     int        检测结果
 *              - 1: 有人，距离写入 *distance
 *              - 0: 无人（接收到 "OFF"）
 *              - -1: 空指针或解析错误
 */
int  axk_rd03_v2_read(int *distance);

#endif /* SCBB_RD03_V2_ENABLED */
#endif /* AXK_RD03_V2_H */
