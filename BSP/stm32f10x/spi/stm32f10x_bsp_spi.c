/**
 * @file stm32f10x_bsp_spi.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x SPI 板级支持包实现。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 */
#include "stm32f10x_bsp_spi.h"
#include "stm32f1xx_hal.h"

static SPI_HandleTypeDef hspi1;

/**
 * @brief 初始化 SPI1（主机模式，MSB 优先）
 *
 * @param[in]  name   SPI 设备名（保留，当前固定使用 SPI1）
 * @param[in]  cfg    SPI 配置指针，不可为 NULL
 * @return     int    操作状态
 *              - 0: 初始化成功
 *              - -1: 初始化失败或 cfg 为空
 */
int bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg) {
    if (cfg == NULL) {
        return -1;
    }
    (void)name;

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

/**
 * @brief 发送一个 8 位数据（阻塞，100ms 超时）
 *
 * @param[in]  name  SPI 设备名（保留）
 * @param[in]  data  待发送的字节
 */
void bsp_spi_send8(const char *name, uint8_t data) {
    (void)name;

    HAL_SPI_Transmit(&hspi1, &data, 1, 100);
}

/**
 * @brief 发送 n 个 16 位数据（阻塞，1s 超时）
 *
 * @param[in]  name  SPI 设备名（保留）
 * @param[in]  data  数据缓冲区指针，不可为 NULL
 * @param[in]  n     16 位数据个数
 */
void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n) {
    (void)name;

    if (data == NULL || n == 0) {
        return;
    }
    HAL_SPI_Transmit(&hspi1, (uint8_t *)data, n * 2, 1000);
}
