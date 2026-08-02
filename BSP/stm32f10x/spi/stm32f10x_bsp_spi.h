/**
 * @file stm32f10x_bsp_spi.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief STM32F10x SPI 板级支持包（HAL_SPI 封装）。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 */
#ifndef STM32F10X_BSP_SPI_H
#define STM32F10X_BSP_SPI_H

#include <stdint.h>

/**
 * @brief SPI 配置结构体
 */
typedef struct {
    uint32_t freq;       /**< SPI 时钟频率（Hz） */
    uint8_t  mode;       /**< SPI 模式（0-3，CPOL/CPHA 组合） */
    uint8_t  data_width; /**< 数据宽度（8 或 16） */
} bsp_spi_cfg_t;

/**
 * @brief 初始化 SPI1（主机模式，MSB 优先）
 *
 * @param[in]  name   SPI 设备名（保留，当前固定使用 SPI1）
 * @param[in]  cfg    SPI 配置指针，不可为 NULL
 * @return     int    操作状态
 *              - 0: 初始化成功
 *              - -1: 初始化失败或 cfg 为空
 */
int bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg);

/**
 * @brief 发送一个 8 位数据（阻塞，100ms 超时）
 *
 * @param[in]  name  SPI 设备名（保留）
 * @param[in]  data  待发送的字节
 */
void bsp_spi_send8(const char *name, uint8_t data);

/**
 * @brief 发送 n 个 16 位数据（阻塞，1s 超时）
 *
 * @param[in]  name  SPI 设备名（保留）
 * @param[in]  data  数据缓冲区指针，不可为 NULL
 * @param[in]  n     16 位数据个数
 */
void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n);

#endif /* STM32F10X_BSP_SPI_H */
