/**
 * @file stm32f10x_delay.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "stm32f10x_delay.h"

void HAL_Delay_us(uint32_t us) {
  uint32_t ticks;
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD; // 获取SysTick重载值
  ticks = us * (SystemCoreClock / 1000000);
  told = SysTick->VAL; // 记录当前计数值
  while (tcnt < ticks) {
    tnow = SysTick->VAL;
    if (tnow != told) {
      // SysTick是向下计数，需处理溢出
      if (tnow < told)
        tcnt += told - tnow;
      else
        tcnt += reload - tnow + told;

      told = tnow;
    }
  }
}

void HAL_Delay(uint32_t Delay) { HAL_Delay_us(Delay * 1000); }