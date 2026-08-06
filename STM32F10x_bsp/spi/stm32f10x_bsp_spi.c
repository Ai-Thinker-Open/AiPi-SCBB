/**
 * STM32F10x SPI BSP — HAL_SPI 封装
 */
#include "stm32f10x_bsp_spi.h"
#include "stm32f1xx_hal.h"

static SPI_HandleTypeDef hspi1;

int bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg) {
    GPIO_InitTypeDef gi = {0};

    /* 使能 SPI1 与 GPIOA 时钟 */
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* 配置 SPI1 引脚:SCK(PA5)/MOSI(PA7)复用推挽,MISO(PA6)浮空输入 */
    gi.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    gi.Mode = GPIO_MODE_AF_PP;
    gi.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &gi);
    gi.Pin = GPIO_PIN_6;
    gi.Mode = GPIO_MODE_INPUT;
    gi.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gi);

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = (cfg->data_width == 16) ? SPI_DATASIZE_16BIT : SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = (cfg->mode & 2) ? SPI_POLARITY_HIGH : SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = (cfg->mode & 1) ? SPI_PHASE_2EDGE : SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    /* 预分频 8:APB2 72MHz / 8 = 9MHz,满足 LLCC68 16MHz 上限 */
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
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

uint8_t bsp_spi_transfer8(const char *name, uint8_t tx) {
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, 100);
    return rx;
}
