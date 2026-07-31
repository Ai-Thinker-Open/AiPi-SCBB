#ifndef _BL616_BSP_SPI_H_
#define _BL616_BSP_SPI_H_
#include <stdint.h>
typedef struct { uint32_t freq; uint8_t mode; uint8_t data_width; } bsp_spi_cfg_t;
int  bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg);
void bsp_spi_send8(const char *name, uint8_t data);
void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n);
#endif
