/**
 * @file axk_dht11.h
 * @brief DHT11 温湿度传感器 — 单总线 (One-Wire) 协议
 *
 * 跨平台：GPIO + Delay BSP
 */
#ifndef _AXK_DHT11_H_
#define _AXK_DHT11_H_

#include "scbb_config.h"

#ifdef SCBB_DHT11_ENABLED

#ifdef SCBB_DHT11_GPIO_HEADER
#include SCBB_DHT11_GPIO_HEADER
#endif
#ifdef SCBB_DHT11_DELAY_HEADER
#include SCBB_DHT11_DELAY_HEADER
#endif

#include <stdint.h>

#ifndef AXK_DHT11_PIN
#define AXK_DHT11_PIN  0
#endif

int  axk_dht11_read(float *temp, float *humi);

#endif
#endif
