/**
 * @file axk_dht11.c
 * @brief DHT11 — 纯 ACLL 跨平台实现
 */
#include "axk_dht11.h"

#ifdef SCBB_DHT11_ENABLED

#define GI(p,m)  AXK_DHT11_GPIO_ACLL(init, p, m)
#define GS(p)    AXK_DHT11_GPIO_ACLL(set, p)
#define GR(p)    AXK_DHT11_GPIO_ACLL(reset, p)
#define GRD(p)   AXK_DHT11_GPIO_ACLL(read, p)
#define DMS(x)   AXK_DHT11_DELAY_MS(x)
#define DUS(x)   AXK_DHT11_DELAY_US(x)

#define PIN AXK_DHT11_PIN

static uint8_t data[5];

static int wait_level(int level, uint32_t timeout_us) {
    uint32_t t = 0;
    while (GRD(PIN) != level) {
        if (++t > timeout_us) return -1;
        DUS(1);
    }
    return 0;
}

int axk_dht11_read(float *temp, float *humi) {
    if (!temp || !humi) return -1;

    /* 起始信号: 拉低 18ms */
    GI(PIN, 1);
    GR(PIN); DMS(18);
    GS(PIN); DUS(30);
    GI(PIN, 0); /* 切换为输入 */

    /* 等待 DHT11 响应 */
    if (wait_level(0, 100) < 0) return -2;
    if (wait_level(1, 100) < 0) return -3;
    if (wait_level(0, 100) < 0) return -4;

    /* 读 40 bit */
    for (int i = 0; i < 5; i++) {
        uint8_t v = 0;
        for (int b = 7; b >= 0; b--) {
            if (wait_level(1, 100) < 0) return -5;
            DUS(30); /* 等 30us 再判断 */
            if (GRD(PIN)) v |= (1 << b);
            if (wait_level(0, 100) < 0) return -6;
        }
        data[i] = v;
    }

    /* 校验 */
    if ((uint8_t)(data[0] + data[1] + data[2] + data[3]) != data[4])
        return -7;

    *humi = (float)data[0] + (float)data[1] * 0.1f;
    *temp = (float)data[2] + (float)data[3] * 0.1f;
    return 0;
}
#endif
