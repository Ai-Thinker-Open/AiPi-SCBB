/**
 * @file axk_relay.h
 * @brief 继电器驱动 — 高电平有效
 *
 * 跨平台：GPIO + Delay，通过 scbb_config.h 选 BSP。
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

#ifndef AXK_RELAY_PIN
#define AXK_RELAY_PIN  0
#endif

int  axk_relay_init(void);
void axk_relay_on(void);
void axk_relay_off(void);
void axk_relay_toggle(void);

#endif
#endif
