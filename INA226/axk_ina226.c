/**
 * @file axk_ina226.c
 * @brief INA226 — 纯 ACLL I2C 跨平台
 */
#include "axk_ina226.h"

#ifdef SCBB_INA226_ENABLED

#define I2C_INIT(n,c)  AXK_INA226_I2C_ACLL(init,n,c)
#define I2C_WR(n,a,d,l) AXK_INA226_I2C_ACLL(write,n,a,d,l)
#define I2C_RD(n,a,d,l) AXK_INA226_I2C_ACLL(read,n,a,d,l)
#define DMS(x)          AXK_INA226_DELAY_MS(x)

enum { REG_CFG=0x00, REG_SV=0x01, REG_BV=0x02, REG_PWR=0x03, REG_CUR=0x04, REG_CAL=0x05 };

static void wr16(uint8_t reg, uint16_t val) {
    uint8_t d[3] = { reg, val>>8, val&0xFF };
    I2C_WR(AXK_INA226_I2C, AXK_INA226_ADDR, d, 3);
}

static uint16_t rd16(uint8_t reg) {
    uint8_t d[2];
    I2C_WR(AXK_INA226_I2C, AXK_INA226_ADDR, &reg, 1);
    I2C_RD(AXK_INA226_I2C, AXK_INA226_ADDR, d, 2);
    return (d[0]<<8)|d[1];
}

int axk_ina226_init(void) {
    bsp_i2c_cfg_t c = { .freq = 100000 };
    I2C_INIT(AXK_INA226_I2C, &c);
    /* 校准: Current_LSB = 1mA, Cal = 0.00512/(0.001*0.01) = 512 */
    wr16(REG_CAL, 5120);
    wr16(REG_CFG, 0x4127); /* avg=1, Vbus=1.1ms, Vsh=1.1ms, mode=shunt+bus continuous */
    return 0;
}

static float sv_to_current(int16_t sv) {
    return sv * 2.5e-6f / AXK_INA226_SHUNT; /* LSB=2.5uV */
}

float axk_ina226_read_voltage(void) {
    int16_t raw = (int16_t)rd16(REG_BV);
    return raw * 1.25e-3f; /* LSB=1.25mV */
}

float axk_ina226_read_current(void) {
    int16_t raw = (int16_t)rd16(REG_CUR);
    return raw * 1e-3f; /* 1mA LSB */
}

float axk_ina226_read_power(void) {
    uint16_t raw = rd16(REG_PWR);
    return raw * 25e-3f; /* 25mW LSB */
}
#endif
