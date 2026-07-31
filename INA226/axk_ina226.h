/**
 * @file axk_ina226.h
 * @brief INA226 电压电流传感器 — I2C, 纯 ACLL
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

#ifndef AXK_INA226_ADDR
#define AXK_INA226_ADDR 0x40
#endif
#ifndef AXK_INA226_I2C
#define AXK_INA226_I2C  "i2c0"
#endif
#ifndef AXK_INA226_SHUNT
#define AXK_INA226_SHUNT 0.01f  /* 10mΩ 采样电阻 */
#endif

int   axk_ina226_init(void);
float axk_ina226_read_voltage(void);
float axk_ina226_read_current(void);
float axk_ina226_read_power(void);

#endif
#endif
