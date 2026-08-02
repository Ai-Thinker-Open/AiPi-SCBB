/**
 * @file axk_ds1302.c
 * @brief DS1302 RTC — 纯 ACLL GPIO 位操作实现。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "axk_ds1302.h"

#ifdef SCBB_DS1302_ENABLED

#define AXK_DS1302_GPIO_INIT(p,m) AXK_DS1302_GPIO_ACLL(init,p,m)
#define AXK_DS1302_GPIO_SET(p)   AXK_DS1302_GPIO_ACLL(set,p)
#define AXK_DS1302_GPIO_RESET(p)   AXK_DS1302_GPIO_ACLL(reset,p)
#define AXK_DS1302_GPIO_READ(p)  AXK_DS1302_GPIO_ACLL(read,p)

/**
 * @brief 数据脚切换为输出模式
 */
static void axk_ds1302_io_out(void) {
    AXK_DS1302_GPIO_INIT(AXK_DS1302_PIN_IO, 1);
}

/**
 * @brief 数据脚切换为输入模式
 */
static void axk_ds1302_io_in(void) {
    AXK_DS1302_GPIO_INIT(AXK_DS1302_PIN_IO, 0);
}

/**
 * @brief 产生一个时钟脉冲（SCLK 拉高-延时-拉低）
 */
static void axk_ds1302_sclk(void) {
    AXK_DS1302_GPIO_SET(AXK_DS1302_PIN_SCLK);
    AXK_DS1302_DELAY_US(1);
    AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_SCLK);
}

/**
 * @brief 通过 3 线接口读写一个字节（LSB 优先）
 *
 * @param[in]  d       要写入的字节（写模式时）
 * @return     uint8_t 读取到的字节（读模式时）
 */
static uint8_t axk_ds1302_rw_byte(uint8_t d) {
    uint8_t r = 0;
    for (int i = 0; i < 8; i++) {
        if ((d & 1) != 0) {
            AXK_DS1302_GPIO_SET(AXK_DS1302_PIN_IO);
        } else {
            AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_IO);
        }
        d >>= 1;
        axk_ds1302_sclk();
        if (AXK_DS1302_GPIO_READ(AXK_DS1302_PIN_IO)) {
            r |= (1u << i);
        }
    }
    return r;
}

/**
 * @brief 初始化 DS1302 GPIO 引脚
 *
 * @return int  0: 成功
 */
int axk_ds1302_init(void) {
    AXK_DS1302_GPIO_INIT(AXK_DS1302_PIN_CE, 1); AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_CE);
    AXK_DS1302_GPIO_INIT(AXK_DS1302_PIN_SCLK, 1); AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_SCLK);
    axk_ds1302_io_out(); AXK_DS1302_GPIO_SET(AXK_DS1302_PIN_IO);
    return 0;
}

/**
 * @brief BCD 码转十进制
 *
 * @param[in]  b       BCD 码
 * @return     uint8_t 十进制值
 */
static uint8_t axk_ds1302_bcd2dec(uint8_t b) {
    return (b >> 4) * 10 + (b & 0x0F);
}

/**
 * @brief 十进制转 BCD 码
 *
 * @param[in]  d       十进制值（0-99）
 * @return     uint8_t BCD 码
 */
static uint8_t axk_ds1302_dec2bcd(uint8_t d) {
    return ((d / 10) << 4) | (d % 10);
}

/**
 * @brief 突发读取 DS1302 当前时间（7 字节）
 *
 * @param[out] *t   时间结构体指针
 * @return     int  0: 成功，-1: 空指针
 */
int axk_ds1302_get_time(axk_ds1302_time_t *t) {
    if (!t) {
        return -1;
    }
    AXK_DS1302_GPIO_SET(AXK_DS1302_PIN_CE);
    axk_ds1302_rw_byte(0xBF);
    uint8_t raw[7];
    for (int i = 0; i < 7; i++) {
        axk_ds1302_io_in();
        raw[i] = axk_ds1302_rw_byte(0);
    }
    AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_CE);
    t->sec  = axk_ds1302_bcd2dec(raw[0] & 0x7F);
    t->min  = axk_ds1302_bcd2dec(raw[1]);
    t->hour = axk_ds1302_bcd2dec(raw[2] & 0x3F);
    t->day  = axk_ds1302_bcd2dec(raw[3]);
    t->mon  = axk_ds1302_bcd2dec(raw[4]);
    t->week = axk_ds1302_bcd2dec(raw[5]);
    t->year = axk_ds1302_bcd2dec(raw[6]);
    return 0;
}

/**
 * @brief 突发写入 DS1302 时间（先关闭写保护）
 *
 * @param[in]  *t   时间结构体指针
 * @return     int  0: 成功，-1: 空指针
 */
int axk_ds1302_set_time(const axk_ds1302_time_t *t) {
    if (!t) {
        return -1;
    }
    /* 时间字段范围校验（BCD 编码前） */
    if (t->sec < 0 || t->sec > 59 || t->min < 0 || t->min > 59 ||
        t->hour < 0 || t->hour > 23 || t->day < 1 || t->day > 31 ||
        t->mon < 1 || t->mon > 12 || t->week < 1 || t->week > 7 ||
        t->year < 0 || t->year > 99) {
        return -2;
    }
    AXK_DS1302_GPIO_SET(AXK_DS1302_PIN_CE);
    axk_ds1302_rw_byte(0x8E);
    axk_ds1302_rw_byte(0x00);
    AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_CE);
    AXK_DS1302_GPIO_SET(AXK_DS1302_PIN_CE);
    axk_ds1302_rw_byte(0xBE);
    uint8_t d[] = { axk_ds1302_dec2bcd(t->sec), axk_ds1302_dec2bcd(t->min), axk_ds1302_dec2bcd(t->hour),
                    axk_ds1302_dec2bcd(t->day), axk_ds1302_dec2bcd(t->mon), axk_ds1302_dec2bcd(t->week), axk_ds1302_dec2bcd(t->year) };
    axk_ds1302_io_out();
    for (int i = 0; i < 7; i++) {
        axk_ds1302_rw_byte(d[i]);
    }
    AXK_DS1302_GPIO_RESET(AXK_DS1302_PIN_CE);
    return 0;
}
#endif /* SCBB_DS1302_ENABLED */
