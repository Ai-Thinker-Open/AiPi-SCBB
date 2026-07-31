/**
 * @file axk_dht11.c
 * @brief DHT11 — 纯 ACLL 跨平台实现（单总线 GPIO）。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
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

static uint8_t s_axk_dht11_data[5];

/**
 * @brief 等待 GPIO 达到指定电平，微秒级超时
 *
 * @param[in]  level       目标电平（0 或 1）
 * @param[in]  timeout_us  超时时间（微秒）
 * @return     int         0: 达到目标电平，-1: 超时
 */
static int wait_level(int level, uint32_t timeout_us) {
    uint32_t t = 0;
    while (GRD(PIN) != level) {
        if (++t > timeout_us) return -1;
        DUS(1);
    }
    return 0;
}

/**
 * @brief 执行 DHT11 完整读取周期并返回温湿度
 *
 * @param[out] *temp  温度输出指针（摄氏度），不可为 NULL
 * @param[out] *humi  湿度输出指针（%RH），不可为 NULL
 * @return     int    操作状态
 *              - 0: 数据读取并校验成功
 *              - -1: temp 或 humi 为空指针
 *              - -2: 等待传感器拉低超时
 *              - -3: 等待传感器拉高超时
 *              - -4: 等待数据起始超时
 *              - -5: 等待位高电平超时
 *              - -6: 等待位低电平超时
 *              - -7: 校验和不匹配
 */
int axk_dht11_read(float *temp, float *humi) {
    if (!temp || !humi) return -1;

    GI(PIN, 1);
    GR(PIN); DMS(18);
    GS(PIN); DUS(30);
    GI(PIN, 0);

    if (wait_level(0, 100) < 0) return -2;
    if (wait_level(1, 100) < 0) return -3;
    if (wait_level(0, 100) < 0) return -4;

    for (int i = 0; i < 5; i++) {
        uint8_t v = 0;
        for (int b = 7; b >= 0; b--) {
            if (wait_level(1, 100) < 0) return -5;
            DUS(30);
            if (GRD(PIN)) v |= (1 << b);
            if (wait_level(0, 100) < 0) return -6;
        }
        s_axk_dht11_data[i] = v;
    }

    if ((uint8_t)(s_axk_dht11_data[0] + s_axk_dht11_data[1] +
                  s_axk_dht11_data[2] + s_axk_dht11_data[3]) != s_axk_dht11_data[4]) {
        return -7;
    }

    *humi = (float)s_axk_dht11_data[0] + (float)s_axk_dht11_data[1] * 0.1f;
    *temp = (float)s_axk_dht11_data[2] + (float)s_axk_dht11_data[3] * 0.1f;
    return 0;
}
#endif /* SCBB_DHT11_ENABLED */
