/**
 * @file axk_ina226.h
 * @brief INA226 电压/电流/功率监测驱动（I2C）。
 *
 * 测量总线电压、分流电压、电流和功率。
 * 默认: I2C 地址 0x40，采样电阻 10mΩ。
 * 跨平台：纯 ACLL，I2C + Delay BSP 通过 scbb_config.h 选取。
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef _AXK_INA226_H_
#define _AXK_INA226_H_

#include "scbb_config.h"

#ifdef SCBB_INA226_ENABLED

#ifdef SCBB_INA226_I2C_HEADER
#include SCBB_INA226_I2C_HEADER
#endif
#ifdef SCBB_INA226_DELAY_HEADER
#include SCBB_INA226_DELAY_HEADER
#endif

#include <stdint.h>

/** @brief INA226 I2C 地址（可覆盖，0x40–0x4F） */
#ifndef AXK_INA226_ADDR
#define AXK_INA226_ADDR 0x40
#endif
/** @brief I2C 总线设备名（可覆盖） */
#ifndef AXK_INA226_I2C
#define AXK_INA226_I2C  "i2c0"
#endif
/** @brief 采样电阻阻值（Ω，可覆盖） */
#ifndef AXK_INA226_SHUNT
#define AXK_INA226_SHUNT 0.01f
#endif

/**
 * @brief 初始化 INA226 并配置连续测量模式
 *
 * 设置校准寄存器（Current_LSB = 1mA），配置 1.1ms 转换时间，启动连续测量。
 *
 * @return int  0: 成功
 */
int   axk_ina226_init(void);

/**
 * @brief 读取总线电压（负载端）
 *
 * @return float  电压（V）
 */
float axk_ina226_read_voltage(void);

/**
 * @brief 读取负载电流
 *
 * @return float  电流（A）
 */
float axk_ina226_read_current(void);

/**
 * @brief 读取功率
 *
 * @return float  功率（W）
 */
float axk_ina226_read_power(void);

#endif /* SCBB_INA226_ENABLED */
#endif /* _AXK_INA226_H_ */
