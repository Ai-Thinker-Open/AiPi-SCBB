/**
 * @file bl616_bsp_spi.c
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 SPI 板级支持包实现（bflb_spi 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "bl616_bsp_spi.h"
#include "bflb_spi.h"

/** SPI 设备句柄 */
static struct bflb_device_s *s_bsp_spi_dev;

/**
 * @brief 初始化 SPI（主机模式，MSB 优先，和 SDK lcd_spi_hard_4 配置一致）
 *
 * @param[in]  name   SPI 设备名（如 "spi0"）
 * @param[in]  cfg    SPI 配置指针，不可为 NULL
 * @return     int    操作状态
 *              - 0: 初始化成功
 *              - -1: cfg 为空
 */
int bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg) {
    if (cfg == NULL) {
        return -1;
    }

    s_bsp_spi_dev = bflb_device_get_by_name(name);
    struct bflb_spi_config_s c = {
        .freq = cfg->freq,
        .role = SPI_ROLE_MASTER,
        .mode = cfg->mode,
        .data_width = cfg->data_width,
        .bit_order = SPI_BIT_MSB,
        .byte_order = SPI_BYTE_MSB,
        .tx_fifo_threshold = (2 * 4 - 1), /* FIFO 阈值，配合 4 字节突发 */
        .rx_fifo_threshold = (2 * 4 - 1),
    };
    bflb_spi_init(s_bsp_spi_dev, &c);
    return 0;
}

/**
 * @brief 发送一个 8 位数据（轮询）
 *
 * @param[in]  name  SPI 设备名
 * @param[in]  data  待发送的字节
 */
void bsp_spi_send8(const char *name, uint8_t data) {
    (void)name;

    bflb_spi_feature_control(s_bsp_spi_dev, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
    bflb_spi_poll_send(s_bsp_spi_dev, data);
}

/**
 * @brief 发送 n 个 16 位数据（轮询交换）
 *
 * @param[in]  name  SPI 设备名
 * @param[in]  data  数据缓冲区指针，不可为 NULL
 * @param[in]  n     16 位数据个数
 */
void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n) {
    (void)name;

    if (data == NULL || n == 0) {
        return;
    }
    bflb_spi_feature_control(s_bsp_spi_dev, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);
    bflb_spi_poll_exchange(s_bsp_spi_dev, data, NULL, n * 2);
    bflb_spi_feature_control(s_bsp_spi_dev, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
}
