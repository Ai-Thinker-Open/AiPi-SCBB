/**
 * @file axk_dht11.h
 * @brief DHT11 温湿度传感器 — 单总线 (One-Wire) 协议。
 *
 * 跨平台：GPIO + Delay BSP，通过 scbb_config.h 选取。
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef AXK_DHT11_H
#define AXK_DHT11_H

#include "scbb_config.h"

#ifdef SCBB_DHT11_ENABLED

#ifdef SCBB_DHT11_GPIO_HEADER
#include SCBB_DHT11_GPIO_HEADER
#endif
#ifdef SCBB_DHT11_DELAY_HEADER
#include SCBB_DHT11_DELAY_HEADER
#endif

#include <stdint.h>

/** @brief DHT11 数据脚（可覆盖） */
#ifndef AXK_DHT11_PIN
#define AXK_DHT11_PIN  0
#endif

/**
 * @brief 执行 DHT11 完整读取周期并返回温湿度
 *
 * 起始信号（拉低 18ms）→ 等待响应 → 读取 40 位 → 校验和验证。
 *
 * @param[out] *temp  温度输出指针（摄氏度），不可为 NULL
 * @param[out] *humi  湿度输出指针（%RH），不可为 NULL
 * @return     int    操作状态
 *              - 0: 读取并校验成功
 *              - -1: temp 或 humi 为空指针
 *              - -2: 等待传感器拉低超时
 *              - -3: 等待传感器拉高超时
 *              - -4: 等待数据起始超时
 *              - -5: 等待位高电平超时
 *              - -6: 等待位低电平超时
 *              - -7: 校验和不匹配
 */
int  axk_dht11_read(float *temp, float *humi);

#endif /* SCBB_DHT11_ENABLED */
#endif /* AXK_DHT11_H */
