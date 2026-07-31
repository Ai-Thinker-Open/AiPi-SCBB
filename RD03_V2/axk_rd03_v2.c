/**
 * @file axk_rd03_v2.c
 * @brief Rd-03_V2 雷达 — 纯 ACLL 跨平台实现。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include <string.h>

#include "axk_rd03_v2.h"

#ifdef SCBB_RD03_V2_ENABLED

#define UART_INIT(n,c)  AXK_RD03_V2_UART_ACLL(init, n, c)
#define UART_GETC(n)    AXK_RD03_V2_UART_ACLL(getchar, n)
#define DELAY_MS(x)     AXK_RD03_V2_DELAY_MS(x)

/**
 * @brief 初始化雷达 UART，波特率 115200
 *
 * @return int  0: 成功
 */
int axk_rd03_v2_init(void) {
    bsp_uart_cfg_t cfg = { .baudrate = 115200 };
    UART_INIT(AXK_RD03_V2_UART, &cfg);
    return 0;
}

/**
 * @brief 从 UART 读取一行并解析距离
 *
 * @param[out] *distance  距离输出指针（cm），不可为 NULL
 * @return     int        检测结果
 *              - 1: 有人，距离写入 *distance
 *              - 0: 无人（接收到 "OFF"）
 *              - -1: 空指针或解析错误
 */
int axk_rd03_v2_read(int *distance) {
    if (!distance) return -1;
    char buf[32];
    int idx = 0;
    int timeout = 0;

    while (idx < 31) {
        int c = UART_GETC(AXK_RD03_V2_UART);
        if (c >= 0) {
            buf[idx++] = (char)c;
            if (c == '\n') break;
        } else {
            DELAY_MS(1);
            if (++timeout > 100) break;
        }
    }
    buf[idx] = '\0';

    if (strncmp(buf, "OFF", 3) == 0) return 0;

    char *p = strstr(buf, "distance:");
    if (p) {
        *distance = 0;
        for (p += 9; *p >= '0' && *p <= '9'; p++)
            *distance = *distance * 10 + (*p - '0');
        return 1;
    }
    return -1;
}
#endif /* SCBB_RD03_V2_ENABLED */
