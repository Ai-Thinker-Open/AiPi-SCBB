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

/**
 * @brief HXD039B2 UART 协议抽象层。
 *        如果使用其他 MCU，请包含对应的 UART 头文件。
 */
#if __has_include("stm32f10x_bsp_uart.h")
#include "stm32f10x_bsp_uart.h"
#define AXK_HXD039B2_UART_ACLL(_func, ...) bsp_uart_##_func(__VA_ARGS__)
#define AXK_HXD039B2_GPIO_ACLL(_func, ...) bsp_gpio_##_func(__VA_ARGS__)
#define AXK_HXD039B2_DELAY_MS(x) delay_ms((x))
#pragma message "stm32f10x_bsp_uart.h included for HXD039B2"
#else
#error "Please include the appropriate UART header for HXD039B2."
#endif

/**
 * @brief HXD039B2 忙检测 GPIO 引脚。
 */
#define AXK_HXD039B2_BUSY_PIN 1

/**
 * @brief HXD039B2 时序常量。
 */
#define AXK_HXD039B2_START_TIME_MS 70

/**
 * @brief 红外码字节定义。
 */
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

/**
 * @brief 初始化 HXD039B2 模块。
 *
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_init(void);

/**
 * @brief 启动红外码学习。
 *
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_start_learn(void);

/**
 * @brief 设置空调电源状态。
 *
 *  @param[in]  power_state     1: 开，0: 关。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_power(int power_state);

/**
 * @brief 设置空调模式。
 *
 *  @param[in]  mode            0: 自动，1: 制冷，2: 除湿，3: 送风，4: 制热。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_mode(int mode);

/**
 * @brief 设置空调温度。
 *
 *  @param[in]  temperature     16-31 摄氏度。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_temperature(unsigned char temperature);

/**
 * @brief 设置空调风速。
 *
 *  @param[in]  fan_mode        0: 低速，1: 中速，2: 高速，3: 自动。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_fan_mode(unsigned char fan_mode);

/**
 * @brief 设置空调风向。
 *
 *  @param[in]  trend           0: 向上，1: 中间，2: 向下。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_trend(unsigned char trend);

/**
 * @brief 设置空调自动风向。
 *
 *  @param[in]  trend_auto      1: 开启，0: 关闭。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_trend_auto(unsigned char trend_auto);

/**
 * @brief 设置空调指示灯。
 *
 *  @param[in]  light_power     1: 开启，0: 关闭。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_set_light_power(unsigned char light_power);

/**
 * @brief UART 接收回调，在 MCU UART 中断中调用。
 *
 *  @param[in]  uart_data       接收到的字节。
 */
void axk_hxd039b2_uart_recv_cb(unsigned char uart_data);

/**
 * @brief 保存空调码组到持久化存储。
 *
 *  @param[in]  ac_code         空调码组缓冲区。
 *  @param[in]  len             码组长度。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_save_ac_code(unsigned char *ac_code, int len);

/**
 * @brief 从持久化存储读取空调码组。
 *
 *  @param[out] code            空调码组缓冲区。
 *  @return     码组长度，未找到返回 0。
 */
int axk_hxd039b2_get_ac_code(unsigned char *code);

#endif /* __AXK_HXD039B2_H__ */
