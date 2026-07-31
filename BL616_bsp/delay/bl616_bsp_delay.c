#include "bl616_bsp_delay.h"
#include "bflb_mtimer.h"
#include "bl616_bsp_delay.h"
#include "bflb_mtimer.h"
void bsp_delay_ms(uint32_t ms) { bflb_mtimer_delay_ms(ms); }
void bsp_delay_us(uint32_t us) { bflb_mtimer_delay_us(us); }
