#ifndef _STM32F10X_BSP_SPI_H_
#define _STM32F10X_BSP_SPI_H_
#include <stdint.h>
typedef struct { uint32_t freq; uint8_t mode; uint8_t data_width; } bsp_spi_cfg_t;
int  bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg);
void bsp_spi_send8(const char *name, uint8_t data);
void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n);

/**
 * @brief SPI 全双工交换一个字节(同时发送和接收)
 *
 * @param[in]  name   SPI 总线名称(保留参数)
 * @param[in]  tx     待发送的字节
 * @return     uint8_t  接收到的字节
 */
uint8_t bsp_spi_transfer8(const char *name, uint8_t tx);
#endif
