/**
 * @file axk_rd03_v2.c
 * @brief Rd-03_V2 雷达 — 纯 ACLL 跨平台
 */
#include "axk_rd03_v2.h"
#include <string.h>

#ifdef SCBB_RD03_V2_ENABLED

#define UART_INIT(n,c)  AXK_RD03_V2_UART_ACLL(init, n, c)
#define UART_GETC(n)    AXK_RD03_V2_UART_ACLL(getchar, n)
#define DELAY_MS(x)     AXK_RD03_V2_DELAY_MS(x)

int axk_rd03_v2_init(void) {
    bsp_uart_cfg_t cfg = { .baudrate = 115200 };
    UART_INIT(AXK_RD03_V2_UART, &cfg);
    return 0;
}

int axk_rd03_v2_read(int *distance) {
    if (!distance) return -1;
    char buf[32];
    int idx = 0;
    int timeout = 0;

    /* 读一行（以 \n 结尾） */
    while (idx < 31) {
        int c = UART_GETC(AXK_RD03_V2_UART);
        if (c >= 0) {
            buf[idx++] = (char)c;
            if (c == '\n') break;
        } else {
            DELAY_MS(1);
            if (++timeout > 100) break; /* 100ms 超时 */
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
#endif
