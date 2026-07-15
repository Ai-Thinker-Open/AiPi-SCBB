/**
 * @file axk_hxd039b2.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief HXD039B2 红外编解码模块驱动（UART 协议）。
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_HXD039B2_H__
#define __AXK_HXD039B2_H__

#include "scbb_config.h"

#ifdef SCBB_HXD039B2_ENABLED

/* BSP headers and ACLL macros are defined in scbb_config.h */
#ifdef SCBB_HXD039B2_UART_HEADER
#include SCBB_HXD039B2_UART_HEADER
#endif

#ifdef SCBB_HXD039B2_GPIO_HEADER
#include SCBB_HXD039B2_GPIO_HEADER
#endif

#ifdef SCBB_HXD039B2_DELAY_HEADER
#include SCBB_HXD039B2_DELAY_HEADER
#endif

#define AXK_HXD039B2_BUSY_PIN 1
#define AXK_HXD039B2_START_TIME_MS 70

typedef enum {
    AXK_HXD039B2_IR_CODE_HANDLE = 0x30,
    AXK_HXD039B2_IR_CODE_AC_TYPE = 0x06,
    AXK_HXD039B2_IR_CODE_AC_OFF = 0x80,
    AXK_HXD039B2_IR_CODE_AC_ON,
    AXK_HXD039B2_IR_CODE_AC_MODE_AUTO = 0xA1,
    AXK_HXD039B2_IR_CODE_AC_MODE_COOL,
    AXK_HXD039B2_IR_CODE_AC_MODE_DRY,
    AXK_HXD039B2_IR_CODE_AC_MODE_FAN_ONLY,
    AXK_HXD039B2_IR_CODE_AC_MODE_HEAT,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_16 = 0x40,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_17,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_18,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_19,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_20,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_21,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_22,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_23,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_24,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_25,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_26,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_27,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_28,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_29,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_30,
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_31,
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_AUTO = 0x51,
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_LOW,
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_MEDIUM,
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_HIGH,
    AXK_HXD039B2_IR_CODE_AC_TREND_UP = 0x61,
    AXK_HXD039B2_IR_CODE_AC_TREND_MEDIUM,
    AXK_HXD039B2_IR_CODE_AC_TREND_DOWN,
    AXK_HXD039B2_IR_CODE_AC_TREND_AUTO_ON = 0x70,
    AXK_HXD039B2_IR_CODE_AC_TREND_AUTO_OFF,
    AXK_HXD039B2_IR_CODE_AC_LIGHT_ON = 0xD0,
    AXK_HXD039B2_IR_CODE_AC_LIGHT_OFF,
} axk_hxd039b2_ir_code_byte_t;

int axk_hxd039b2_init(void);
int axk_hxd039b2_start_learn(void);
int axk_hxd039b2_set_power(int power_state);
int axk_hxd039b2_set_mode(int mode);
int axk_hxd039b2_set_temperature(unsigned char temperature);
int axk_hxd039b2_set_fan_mode(unsigned char fan_mode);
int axk_hxd039b2_set_trend(unsigned char trend);
int axk_hxd039b2_set_trend_auto(unsigned char trend_auto);
int axk_hxd039b2_set_light_power(unsigned char light_power);
void axk_hxd039b2_uart_recv_cb(unsigned char uart_data);
int axk_hxd039b2_save_ac_code(unsigned char *ac_code, int len);
int axk_hxd039b2_get_ac_code(unsigned char *code);

#endif /* SCBB_HXD039B2_ENABLED */
#endif /* __AXK_HXD039B2_H__ */
