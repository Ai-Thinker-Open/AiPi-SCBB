/**
 * @file axk_oled_096_spi.c
 * @brief SSD1306 0.96" OLED 128x64 SPI — 纯 ACLL 跨平台实现。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include <string.h>

#include "axk_oled_096_spi.h"

#ifdef SCBB_OLED_096_SPI_ENABLED

#define AXK_OLED_096_SPI_SPI_INIT(n,c)  AXK_OLED_096_SPI_SPI_ACLL(init,n,c)
#define AXK_OLED_096_SPI_SEND8(n,b)  AXK_OLED_096_SPI_SPI_ACLL(send8,n,b)
#define AXK_OLED_096_SPI_GPIO_INIT(p,m)  AXK_OLED_096_SPI_GPIO_ACLL(init,p,m)
#define AXK_OLED_096_SPI_GPIO_SET(p)    AXK_OLED_096_SPI_GPIO_ACLL(set,p)
#define AXK_OLED_096_SPI_GPIO_RESET(p)    AXK_OLED_096_SPI_GPIO_ACLL(reset,p)

/**
 * @brief 片选拉低
 */
static void axk_oled_cs_low(void) {
    AXK_OLED_096_SPI_GPIO_RESET(AXK_OLED_096_SPI_PIN_CS);
}

/**
 * @brief 片选拉高
 */
static void axk_oled_cs_high(void) {
    AXK_OLED_096_SPI_GPIO_SET(AXK_OLED_096_SPI_PIN_CS);
}

/**
 * @brief 数据/命令脚拉低（命令模式）
 */
static void axk_oled_dc_low(void) {
    AXK_OLED_096_SPI_GPIO_RESET(AXK_OLED_096_SPI_PIN_DC);
}

/**
 * @brief 数据/命令脚拉高（数据模式）
 */
static void axk_oled_dc_high(void) {
    AXK_OLED_096_SPI_GPIO_SET(AXK_OLED_096_SPI_PIN_DC);
}

/**
 * @brief 向 SSD1306 发送命令字节
 *
 * @param[in]  c  命令字节值
 */
static void axk_oled_cmd(uint8_t c) {
    axk_oled_dc_low();
    axk_oled_cs_low();
    AXK_OLED_096_SPI_SEND8(AXK_OLED_096_SPI_SPI, c);
    axk_oled_cs_high();
}

/**
 * @brief 向 SSD1306 发送数据字节
 *
 * @param[in]  d  数据字节值
 */
static void axk_oled_dat(uint8_t d) {
    axk_oled_dc_high();
    axk_oled_cs_low();
    AXK_OLED_096_SPI_SEND8(AXK_OLED_096_SPI_SPI, d);
    axk_oled_cs_high();
}

static uint8_t s_axk_oled_fb[128][8];

/**
 * @brief 初始化 SSD1306 OLED：GPIO、SPI、初始化序列、清屏
 *
 * @return int  0: 成功
 */
int axk_oled_096_spi_init(void) {
    AXK_OLED_096_SPI_GPIO_INIT(AXK_OLED_096_SPI_PIN_CS, 1);
    axk_oled_cs_high();
    AXK_OLED_096_SPI_GPIO_INIT(AXK_OLED_096_SPI_PIN_DC, 1);
    axk_oled_dc_high();
    AXK_OLED_096_SPI_GPIO_INIT(AXK_OLED_096_SPI_PIN_RST, 1);
    AXK_OLED_096_SPI_GPIO_RESET(AXK_OLED_096_SPI_PIN_RST);
    AXK_OLED_096_SPI_DELAY_MS(10);
    AXK_OLED_096_SPI_GPIO_SET(AXK_OLED_096_SPI_PIN_RST);
    AXK_OLED_096_SPI_DELAY_MS(100);

    AXK_OLED_096_SPI_GPIO_INIT(AXK_OLED_096_SPI_PIN_SDA, 2);
    AXK_OLED_096_SPI_GPIO_INIT(AXK_OLED_096_SPI_PIN_SCL, 3);
    bsp_spi_cfg_t c = {.freq = 4000000, .mode = 3, .data_width = 8};
    AXK_OLED_096_SPI_SPI_INIT(AXK_OLED_096_SPI_SPI, &c);

    uint8_t init[] = {
        0xAE,        /* 关闭显示 */
        0xD5,0x80,   /* 时钟分频 */
        0xA8,0x3F,   /* 多路复用比 64 */
        0xD3,0x00,   /* 显示偏移 */
        0x40,        /* 起始行 */
        0x8D,0x14,   /* 电荷泵 */
        0x20,0x00,   /* 水平寻址 */
        0xA1,        /* 段重映射 */
        0xC8,        /* COM 扫描方向 */
        0xDA,0x12,   /* COM 引脚配置 */
        0x81,0xCF,   /* 对比度 */
        0xD9,0xF1,   /* 预充电 */
        0xDB,0x40,   /* VCOM 检测 */
        0xA4,        /* 显示恢复 */
        0xA6,        /* 正常显示 */
        0xAF,        /* 开启显示 */
    };
    for (unsigned int i = 0; i < sizeof(init); i++) {
        axk_oled_cmd(init[i]);
    }
    axk_oled_096_spi_clear(0);
    axk_oled_096_spi_flush();
    return 0;
}

/**
 * @brief 将帧缓冲清为空色
 *
 * @param[in]  color  0: 全黑，1: 全白
 */
void axk_oled_096_spi_clear(uint8_t color) {
    memset(s_axk_oled_fb, color ? 0xFF : 0x00, sizeof(s_axk_oled_fb));
}

/**
 * @brief 在帧缓冲中设置或清除单个像素
 *
 * @param[in]  x   X 坐标（0–127）
 * @param[in]  y   Y 坐标（0–63）
 * @param[in]  on  1: 点亮，0: 熄灭
 */
void axk_oled_096_spi_set_pixel(int x, int y, int on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) {
        return;
    }
    if (on) {
        s_axk_oled_fb[x][y / 8] |= (1u << (y % 8));
    } else {
        s_axk_oled_fb[x][y / 8] &= ~(1u << (y % 8));
    }
}

/**
 * @brief 通过 SPI 将 8 页全部刷新到 OLED 显示屏
 */
void axk_oled_096_spi_flush(void) {
    for (int page = 0; page < 8; page++) {
        axk_oled_cmd(0xB0 + page);
        axk_oled_cmd(0x00);
        axk_oled_cmd(0x10);
        for (int x = 0; x < 128; x++) {
            axk_oled_dat(s_axk_oled_fb[x][page]);
        }
    }
}
#endif /* SCBB_OLED_096_SPI_ENABLED */
