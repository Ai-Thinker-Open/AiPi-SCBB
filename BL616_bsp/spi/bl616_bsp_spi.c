/**
 * BL616/BL618 BSP SPI — 包 bflb_spi，和 SDK lcd_spi_hard_4 一样的配置
 */
#include "bl616_bsp_spi.h"
#include "bflb_spi.h"

static struct bflb_device_s *spi_hd;

int bsp_spi_init(const char *name, bsp_spi_cfg_t *cfg) {
    spi_hd = bflb_device_get_by_name(name);
    struct bflb_spi_config_s c = {
        .freq=cfg->freq,.role=SPI_ROLE_MASTER,.mode=cfg->mode,
        .data_width=cfg->data_width,.bit_order=SPI_BIT_MSB,.byte_order=SPI_BYTE_MSB,
        .tx_fifo_threshold=(2*4-1),.rx_fifo_threshold=(2*4-1)
    };
    bflb_spi_init(spi_hd, &c);
    return 0;
}

void bsp_spi_send8(const char *name, uint8_t data) {
    bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
    bflb_spi_poll_send(spi_hd, data);
}

void bsp_spi_send16(const char *name, const uint16_t *data, uint32_t n) {
    bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_16BIT);
    bflb_spi_poll_exchange(spi_hd, data, NULL, n*2);
    bflb_spi_feature_control(spi_hd, SPI_CMD_SET_DATA_WIDTH, SPI_DATA_WIDTH_8BIT);
}
