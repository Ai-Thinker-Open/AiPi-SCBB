/**
 * @file axk_sht3x.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief SHT3x 温湿度传感器驱动（I2C 协议）。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_SHT3X_H__
#define __AXK_SHT3X_H__

#include "scbb_config.h"

#ifdef SCBB_SHT3X_ENABLED

/* BSP headers and ACLL macros are defined in scbb_config.h */
#ifdef SCBB_SHT3X_I2C_HEADER
#include SCBB_SHT3X_I2C_HEADER
#endif

#ifdef SCBB_SHT3X_DELAY_HEADER
#include SCBB_SHT3X_DELAY_HEADER
#endif

#define AXK_SHT3X_ADDRESS 0x44

#define AXK_SHT3X_WRITE_CMD 0
#define AXK_SHT3X_READ_CMD 1

#define AXK_SHT3X_ACK 0
#define AXK_SHT3X_NACK 1

unsigned char axk_sht3x_init(void);
unsigned char axk_sht3x_set_mode(unsigned int _mode);
unsigned char axk_sht3x_read(unsigned int _mode, double *temp,
                             double *humidity);

#endif /* SCBB_SHT3X_ENABLED */
#endif /* __AXK_SHT3X_H__ */
