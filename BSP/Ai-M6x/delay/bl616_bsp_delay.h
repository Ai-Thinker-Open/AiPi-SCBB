/**
 * @file bl616_bsp_delay.h
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 延时板级支持包（bflb_mtimer 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef BL616_BSP_DELAY_H
#define BL616_BSP_DELAY_H

#include <stdint.h>

/**
 * @brief 毫秒级延时
 *
 * @param[in]  ms  延时毫秒数
 */
void bsp_delay_ms(uint32_t ms);

/**
 * @brief 微秒级延时
 *
 * @param[in]  us  延时微秒数
 */
void bsp_delay_us(uint32_t us);

#endif /* BL616_BSP_DELAY_H */
