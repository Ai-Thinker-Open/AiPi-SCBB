/**
 * @file axk_ds1302.h
 * @brief DS1302 RTC 实时时钟驱动 — GPIO 位操作 3 线协议。
 *
 * 接线: CE(IO16), SCLK(IO15), IO(IO14)。引脚宏可覆盖。
 * 跨平台：纯 ACLL，GPIO + Delay BSP 通过 scbb_config.h 选取。
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
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

/** @brief 使能脚（可覆盖） */
#ifndef AXK_DS1302_PIN_CE
#define AXK_DS1302_PIN_CE   16
#endif
/** @brief 时钟脚（可覆盖） */
#ifndef AXK_DS1302_PIN_SCLK
#define AXK_DS1302_PIN_SCLK 15
#endif
/** @brief 数据 I/O 脚（可覆盖） */
#ifndef AXK_DS1302_PIN_IO
#define AXK_DS1302_PIN_IO   14
#endif

/** @brief DS1302 时间表示（BCD 与二进制由内部转换） */
typedef struct {
    int sec;    /**< 秒（0–59） */
    int min;    /**< 分（0–59） */
    int hour;   /**< 时（0–23） */
    int day;    /**< 日（1–31） */
    int mon;    /**< 月（1–12） */
    int week;   /**< 星期（1–7） */
    int year;   /**< 年（0–99，基于 2000 年） */
} axk_ds1302_time_t;

/**
 * @brief 初始化 DS1302 GPIO 引脚
 *
 * @return int  0: 成功
 */
int  axk_ds1302_init(void);

/**
 * @brief 突发读取 DS1302 当前时间
 *
 * @param[out] *t   时间结构体指针
 * @return     int  0: 成功，-1: 空指针
 */
int  axk_ds1302_get_time(axk_ds1302_time_t *t);

/**
 * @brief 突发写入 DS1302 时间（先关闭写保护）
 *
 * @param[in]  *t   时间结构体指针
 * @return     int  0: 成功，-1: 空指针
 */
int  axk_ds1302_set_time(const axk_ds1302_time_t *t);

#endif /* SCBB_DS1302_ENABLED */
#endif /* _AXK_DS1302_H_ */
