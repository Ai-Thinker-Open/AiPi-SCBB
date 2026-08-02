/**
 * @file bl616_bsp_delay.c
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 延时板级支持包实现（bflb_mtimer 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "bl616_bsp_delay.h"
#include "bflb_mtimer.h"

/**
 * @brief 毫秒级延时
 *
 * @param[in]  ms  延时毫秒数
 */
void bsp_delay_ms(uint32_t ms) {
    bflb_mtimer_delay_ms(ms);
}

/**
 * @brief 微秒级延时
 *
 * @param[in]  us  延时微秒数
 */
void bsp_delay_us(uint32_t us) {
    bflb_mtimer_delay_us(us);
}
