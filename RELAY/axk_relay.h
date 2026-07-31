/**
 * @file axk_relay.h
 * @brief 继电器驱动 — 高电平有效，GPIO 控制。
 *
 * 跨平台：GPIO + Delay，BSP 通过 scbb_config.h 选取。
 *
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef _AXK_RELAY_H_
#define _AXK_RELAY_H_

#include "scbb_config.h"

#ifdef SCBB_RELAY_ENABLED

#ifdef SCBB_RELAY_GPIO_HEADER
#include SCBB_RELAY_GPIO_HEADER
#endif
#ifdef SCBB_RELAY_DELAY_HEADER
#include SCBB_RELAY_DELAY_HEADER
#endif

#include <stdint.h>
#include <stdbool.h>

/** @brief 继电器控制脚（可覆盖） */
#ifndef AXK_RELAY_PIN
#define AXK_RELAY_PIN  0
#endif

/**
 * @brief 初始化继电器 GPIO 引脚，默认为释放状态
 *
 * @return int  0: 成功
 */
int  axk_relay_init(void);

/**
 * @brief 吸合继电器（输出高电平）
 */
void axk_relay_on(void);

/**
 * @brief 释放继电器（输出低电平）
 */
void axk_relay_off(void);

/**
 * @brief 翻转继电器状态（开→关 或 关→开）
 */
void axk_relay_toggle(void);

#endif /* SCBB_RELAY_ENABLED */
#endif /* _AXK_RELAY_H_ */
