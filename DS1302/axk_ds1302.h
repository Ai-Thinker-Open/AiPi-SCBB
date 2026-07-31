/**
 * @file axk_ds1302.h
 * @brief DS1302 RTC 实时时钟 — 纯 ACLL 跨平台
 *
 * 接线: CE(IO16), SCLK(IO15), IO(IO14)
 */
#ifndef _AXK_DS1302_H_
#define _AXK_DS1302_H_

#include "scbb_config.h"

#ifdef SCBB_DS1302_ENABLED

#ifdef SCBB_DS1302_GPIO_HEADER
#include SCBB_DS1302_GPIO_HEADER
#endif
#ifdef SCBB_DS1302_DELAY_HEADER
#include SCBB_DS1302_DELAY_HEADER
#endif

#include <stdint.h>
#include <stdbool.h>

#ifndef AXK_DS1302_PIN_CE
#define AXK_DS1302_PIN_CE   16
#endif
#ifndef AXK_DS1302_PIN_SCLK
#define AXK_DS1302_PIN_SCLK 15
#endif
#ifndef AXK_DS1302_PIN_IO
#define AXK_DS1302_PIN_IO   14
#endif

typedef struct { int sec, min, hour, day, mon, week, year; } axk_ds1302_time_t;

int  axk_ds1302_init(void);
int  axk_ds1302_get_time(axk_ds1302_time_t *t);
int  axk_ds1302_set_time(const axk_ds1302_time_t *t);

#endif
#endif
