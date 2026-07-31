/**
 * @file axk_hxd039b2.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief HXD039B2 红外编解码模块驱动（UART + GPIO）。
 *
 * 通过 UART 控制红外学习/发射模块，用于空调遥控（开关、模式、温度、风速、风向、指示灯）。
 * 跨平台：通过 scbb_config.h 中的 ACLL 宏实现 BSP 抽象。
 *
 * @version 0.1
 * @date 2026-07-14
 * @copyright Copyright (c) 2026
 */
#ifndef __AXK_HXD039B2_H__
#define __AXK_HXD039B2_H__

#include "scbb_config.h"

#ifdef SCBB_HXD039B2_ENABLED

#ifdef SCBB_HXD039B2_UART_HEADER
#include SCBB_HXD039B2_UART_HEADER
#endif
#ifdef SCBB_HXD039B2_GPIO_HEADER
#include SCBB_HXD039B2_GPIO_HEADER
#endif
#ifdef SCBB_HXD039B2_DELAY_HEADER
#include SCBB_HXD039B2_DELAY_HEADER
#endif

#define AXK_HXD039B2_BUSY_PIN 1       /**< 忙检测脚 */
#define AXK_HXD039B2_START_TIME_MS 70 /**< 发送起始延时（ms） */

/** @brief HXD039B2 空调红外指令字节值 */
typedef enum {
    AXK_HXD039B2_IR_CODE_HANDLE          = 0x30, /**< 红外帧头 */
    AXK_HXD039B2_IR_CODE_AC_TYPE         = 0x06, /**< 空调类型字节 */
    AXK_HXD039B2_IR_CODE_AC_OFF          = 0x80, /**< 关机 */
    AXK_HXD039B2_IR_CODE_AC_ON,                  /**< 开机 */
    AXK_HXD039B2_IR_CODE_AC_MODE_AUTO    = 0xA1, /**< 自动模式 */
    AXK_HXD039B2_IR_CODE_AC_MODE_COOL,           /**< 制冷模式 */
    AXK_HXD039B2_IR_CODE_AC_MODE_DRY,            /**< 除湿模式 */
    AXK_HXD039B2_IR_CODE_AC_MODE_FAN_ONLY,       /**< 送风模式 */
    AXK_HXD039B2_IR_CODE_AC_MODE_HEAT,           /**< 制热模式 */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_16 = 0x40, /**< 16°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_17,      /**< 17°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_18,      /**< 18°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_19,      /**< 19°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_20,      /**< 20°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_21,      /**< 21°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_22,      /**< 22°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_23,      /**< 23°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_24,      /**< 24°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_25,      /**< 25°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_26,      /**< 26°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_27,      /**< 27°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_28,      /**< 28°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_29,      /**< 29°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_30,      /**< 30°C */
    AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_31,      /**< 31°C */
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_AUTO  = 0x51, /**< 风速自动 */
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_LOW,          /**< 风速低 */
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_MEDIUM,       /**< 风速中 */
    AXK_HXD039B2_IR_CODE_AC_FAN_MODE_HIGH,         /**< 风速高 */
    AXK_HXD039B2_IR_CODE_AC_TREND_UP       = 0x61, /**< 风向向上 */
    AXK_HXD039B2_IR_CODE_AC_TREND_MEDIUM,          /**< 风向中间 */
    AXK_HXD039B2_IR_CODE_AC_TREND_DOWN,            /**< 风向向下 */
    AXK_HXD039B2_IR_CODE_AC_TREND_AUTO_ON  = 0x70, /**< 自动风向开 */
    AXK_HXD039B2_IR_CODE_AC_TREND_AUTO_OFF,        /**< 自动风向关 */
    AXK_HXD039B2_IR_CODE_AC_LIGHT_ON       = 0xD0, /**< 指示灯开 */
    AXK_HXD039B2_IR_CODE_AC_LIGHT_OFF,             /**< 指示灯关 */
} axk_hxd039b2_ir_code_byte_t;

/**
 * @brief 初始化 HXD039B2 UART、GPIO，读取已存储空调码
 *
 * @return int  0: 成功
 */
int axk_hxd039b2_init(void);

/**
 * @brief 启动红外码学习模式
 *
 * @return int  0: 成功
 */
int axk_hxd039b2_start_learn(void);

/**
 * @brief 通过红外发送空调开关指令
 *
 * @param[in]  power_state  1: 开，0: 关
 * @return     int          0: 成功
 */
int axk_hxd039b2_set_power(int power_state);

/**
 * @brief 通过红外发送空调模式指令
 *
 * @param[in]  mode  0: 自动，1: 制冷，2: 除湿，3: 送风，4: 制热
 * @return     int   0: 成功
 */
int axk_hxd039b2_set_mode(int mode);

/**
 * @brief 通过红外发送空调温度指令
 *
 * @param[in]  temperature  16–31（摄氏度）
 * @return     int          0: 成功
 */
int axk_hxd039b2_set_temperature(unsigned char temperature);

/**
 * @brief 通过红外发送空调风速指令
 *
 * @param[in]  fan_mode  0: 自动，1: 低，2: 中，3: 高
 * @return     int       0: 成功
 */
int axk_hxd039b2_set_fan_mode(unsigned char fan_mode);

/**
 * @brief 通过红外发送空调风向指令
 *
 * @param[in]  trend  0: 向上，1: 中间，2: 向下
 * @return     int    0: 成功
 */
int axk_hxd039b2_set_trend(unsigned char trend);

/**
 * @brief 通过红外发送空调自动风向开关指令
 *
 * @param[in]  trend_auto  1: 开，0: 关
 * @return     int         0: 成功
 */
int axk_hxd039b2_set_trend_auto(unsigned char trend_auto);

/**
 * @brief 通过红外发送空调显示屏指示灯指令
 *
 * @param[in]  light_power  1: 开，0: 关
 * @return     int          0: 成功
 */
int axk_hxd039b2_set_light_power(unsigned char light_power);

/**
 * @brief UART 接收回调（在 UART 中断中调用）
 *
 * @param[in]  uart_data  接收到的字节
 */
void axk_hxd039b2_uart_recv_cb(unsigned char uart_data);

/**
 * @brief 保存空调码到持久化存储
 *
 * @param[in]  ac_code  空调码缓冲区
 * @param[in]  len      码长度（字节）
 * @return     int      0: 成功，-1: 参数无效
 */
int axk_hxd039b2_save_ac_code(unsigned char *ac_code, int len);

/**
 * @brief 从持久化存储读取空调码
 *
 * @param[out] code  空调码输出缓冲区
 * @return     int   码长度，0: 未找到
 */
int axk_hxd039b2_get_ac_code(unsigned char *code);

#endif /* SCBB_HXD039B2_ENABLED */
#endif /* __AXK_HXD039B2_H__ */
