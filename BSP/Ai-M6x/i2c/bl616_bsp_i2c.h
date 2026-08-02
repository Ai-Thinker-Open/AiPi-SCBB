#ifndef _BL616_BSP_I2C_H_
#define _BL616_BSP_I2C_H_
#include <stdint.h>

typedef struct { uint32_t freq; } bsp_i2c_cfg_t;

int  bsp_i2c_init(const char *name, bsp_i2c_cfg_t *cfg);
int  bsp_i2c_write(const char *name, uint8_t addr, const uint8_t *data, uint32_t len);
int  bsp_i2c_read(const char *name, uint8_t addr, uint8_t *data, uint32_t len);
#endif
