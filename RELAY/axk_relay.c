/**
 * @file axk_relay.c
 * @brief 继电器驱动 — 纯 ACLL 跨平台实现。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "axk_relay.h"

#ifdef SCBB_RELAY_ENABLED

#define GPIO_INIT(p,m)  AXK_RELAY_GPIO_ACLL(init, p, m)
#define GPIO_SET(p)     AXK_RELAY_GPIO_ACLL(set, p)
#define GPIO_RESET(p)   AXK_RELAY_GPIO_ACLL(reset, p)
#define DELAY_MS(ms)    AXK_RELAY_DELAY_MS(ms)

static bool s_axk_relay_state;

/**
 * @brief 初始化继电器 GPIO 引脚，默认为释放状态
 *
 * @return int  0: 成功
 */
int axk_relay_init(void) {
    GPIO_INIT(AXK_RELAY_PIN, 1);
    axk_relay_off();
    return 0;
}

/**
 * @brief 吸合继电器（输出高电平）
 */
void axk_relay_on(void)  { GPIO_SET(AXK_RELAY_PIN);   s_axk_relay_state = true; }

/**
 * @brief 释放继电器（输出低电平）
 */
void axk_relay_off(void) { GPIO_RESET(AXK_RELAY_PIN); s_axk_relay_state = false; }

/**
 * @brief 翻转继电器状态
 */
void axk_relay_toggle(void) {
    if (s_axk_relay_state) axk_relay_off(); else axk_relay_on();
}

#endif /* SCBB_RELAY_ENABLED */
