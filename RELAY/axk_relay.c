/**
 * @file axk_relay.c
 * @brief 继电器驱动 — 纯 ACLL 跨平台
 */
#include "axk_relay.h"

#ifdef SCBB_RELAY_ENABLED

#define GPIO_INIT(p,m)  AXK_RELAY_GPIO_ACLL(init, p, m)
#define GPIO_SET(p)     AXK_RELAY_GPIO_ACLL(set, p)
#define GPIO_RESET(p)   AXK_RELAY_GPIO_ACLL(reset, p)
#define DELAY_MS(ms)    AXK_RELAY_DELAY_MS(ms)

static bool state;

int axk_relay_init(void) {
    GPIO_INIT(AXK_RELAY_PIN, 1);
    axk_relay_off();
    return 0;
}

void axk_relay_on(void)  { GPIO_SET(AXK_RELAY_PIN);   state = true; }
void axk_relay_off(void) { GPIO_RESET(AXK_RELAY_PIN); state = false; }
void axk_relay_toggle(void) {
    if (state) axk_relay_off(); else axk_relay_on();
}

#endif
