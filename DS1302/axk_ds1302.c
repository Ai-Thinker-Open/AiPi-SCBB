/**
 * @file axk_ds1302.c
 * @brief DS1302 RTC — 纯 ACLL GPIO 位操作
 */
#include "axk_ds1302.h"

#ifdef SCBB_DS1302_ENABLED

#define GI(p,m) AXK_DS1302_GPIO_ACLL(init,p,m)
#define GS(p)   AXK_DS1302_GPIO_ACLL(set,p)
#define GR(p)   AXK_DS1302_GPIO_ACLL(reset,p)
#define GRD(p)  AXK_DS1302_GPIO_ACLL(read,p)
#define DUS(x)  AXK_DS1302_DELAY_US(x)

#define CE  AXK_DS1302_PIN_CE
#define CLK AXK_DS1302_PIN_SCLK
#define IO  AXK_DS1302_PIN_IO

static void io_out(void) { GI(IO, 1); }
static void io_in(void)  { GI(IO, 0); }
static void sclk(void)   { GS(CLK); DUS(1); GR(CLK); }

static uint8_t rw_byte(uint8_t d) {
    uint8_t r = 0;
    for (int i = 0; i < 8; i++) {
        if (d & 1) GS(IO); else GR(IO);
        d >>= 1;
        sclk();
        if (GRD(IO)) r |= (1 << i);
    }
    return r;
}

int axk_ds1302_init(void) {
    GI(CE, 1); GR(CE);
    GI(CLK, 1); GR(CLK);
    io_out(); GS(IO);
    return 0;
}

static uint8_t bcd2dec(uint8_t b) { return (b >> 4) * 10 + (b & 0x0F); }
static uint8_t dec2bcd(uint8_t d) { return ((d / 10) << 4) | (d % 10); }

int axk_ds1302_get_time(axk_ds1302_time_t *t) {
    if (!t) return -1;
    GS(CE);
    rw_byte(0xBF); /* burst read */
    uint8_t raw[7];
    for (int i = 0; i < 7; i++) { io_in(); raw[i] = rw_byte(0); }
    GR(CE);
    t->sec  = bcd2dec(raw[0] & 0x7F);
    t->min  = bcd2dec(raw[1]);
    t->hour = bcd2dec(raw[2] & 0x3F);
    t->day  = bcd2dec(raw[3]);
    t->mon  = bcd2dec(raw[4]);
    t->week = bcd2dec(raw[5]);
    t->year = bcd2dec(raw[6]);
    return 0;
}

int axk_ds1302_set_time(const axk_ds1302_time_t *t) {
    if (!t) return -1;
    GS(CE);
    rw_byte(0x8E); rw_byte(0x00);  /* 写保护关 */
    GR(CE);
    GS(CE);
    rw_byte(0xBE); /* burst write */
    uint8_t d[] = { dec2bcd(t->sec), dec2bcd(t->min), dec2bcd(t->hour),
                    dec2bcd(t->day), dec2bcd(t->mon), dec2bcd(t->week), dec2bcd(t->year) };
    io_out();
    for (int i = 0; i < 7; i++) rw_byte(d[i]);
    GR(CE);
    return 0;
}
#endif
