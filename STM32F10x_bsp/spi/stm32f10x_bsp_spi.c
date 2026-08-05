/**
 * STM32F10x SPI BSP — HAL_SPI 封装
 */
#include "stm32f10x_bsp_spi.h"
#include "stm32f1xx_hal.h"

static SPI_HandleTypeDef hspi1;

int bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg) {
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = (cfg->data_width == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = (cfg->mode & 2) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = (cfg->mode & 1) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.CRCPolynomial = 7;
    return HAL_SPI_Init(&hspi1) == HAL_OK ? 0 : -1;
}

void bsp_spi_send8(const char *name, uint8_t data) {
    HAL_SPI_Transmit(&hspi1, &data, 1, 100);
}

void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n) {
    HAL_SPI_Transmit(&hspi1, (uint8_t*)data, n * 2, 1000);
}
