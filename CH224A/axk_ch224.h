/**
 * @file axk_ch224.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief CH224A USB-PD 模块驱动（I2C 协议）。
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _AXK_CH224_H_
#define _AXK_CH224_H_

#include "scbb_config.h"

#ifdef SCBB_CH224A_ENABLED

/* BSP headers and ACLL macros are defined in scbb_config.h */
#ifdef SCBB_CH224A_I2C_HEADER
#include SCBB_CH224A_I2C_HEADER
#endif

#ifdef SCBB_CH224A_DELAY_HEADER
#include SCBB_CH224A_DELAY_HEADER
#endif

#define AXK_CH224_I2C_ADDR 0x22
#define AXK_CH224_WRITE_CMD 0
#define AXK_CH224_READ_CMD 1
#define AXK_CH224_ACK 0
#define AXK_CH224_NACK 1

typedef enum {
    AXK_CH224_REG_STATUS = 0x09,
    AXK_CH224_REG_VOUT = 0x0A,
    AXK_CH224_REG_I_DATA = 0x50,
    AXK_CH224_REG_AVS_MSB,
    AXK_CH224_REG_AVS_LSB,
    AXK_CH224_REG_PPS,
} axk_ch224_reg_t;

typedef enum {
    AXK_CH224_VOUT_5V = 0x00,
    AXK_CH224_VOUT_9V,
    AXK_CH224_VOUT_12V,
    AXK_CH224_VOUT_15V,
    AXK_CH224_VOUT_20V,
    AXK_CH224_VOUT_28V,
    AXK_CH224_VOUT_PPS,
    AXK_CH224_VOUT_AVS,
} axk_ch224_vout_t;

int axk_ch224_init(void);
int axk_ch224_get_status(axk_ch224_reg_t onlyRreadReg);
int axk_ch224_set_vout(axk_ch224_vout_t VOUT);
int axk_ch224_set_mode(axk_ch224_vout_t _mode);
int axk_ch224_set_pps_vout(float PPS_VOUT);
int axk_ch224_set_avs_vout(float AVS_VOUT);

#endif /* SCBB_CH224A_ENABLED */
#endif
