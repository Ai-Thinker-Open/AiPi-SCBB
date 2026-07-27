/**
 * @file axk_1_47_lcd.c
 * @brief 1.47" IPS LCD (ST7789V 172x320) 硬件SPI驱动 — Bouffalo BL616/BL618
 */

#include "axk_1_47_lcd.h"

#ifdef SCBB_1_47_LCD_ENABLED

#include "bflb_gpio.h"
#include "bflb_spi.h"
#include "bflb_mtimer.h"
#include "bflb_l1c.h"

/* ─── 内部变量 ─── */
static struct bflb_device_s *spi_dev;
static struct bflb_device_s *gpio_dev;

/* ─── 硬件 SPI GPIO 初始化 ─── */
static void spi_gpio_init(void)
{
    gpio_dev = bflb_device_get_by_name("gpio");
    /* SPI0 MOSI & SCLK */
    bflb_gpio_init(gpio_dev, AXK_1_47_LCD_PIN_SDA, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio_dev, AXK_1_47_LCD_PIN_SCL, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* CS & DC: 软件控制 */
    bflb_gpio_init(gpio_dev, AXK_1_47_LCD_PIN_CS,  GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio_dev, AXK_1_47_LCD_PIN_DC,  GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_CS);
    bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_DC);
}

/* ─── SPI 字节收发 ─── */
static inline void spi_cs_low(void)  { bflb_gpio_reset(gpio_dev, AXK_1_47_LCD_PIN_CS); }
static inline void spi_cs_high(void) { bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_CS); }
static inline void spi_dc_low(void)  { bflb_gpio_reset(gpio_dev, AXK_1_47_LCD_PIN_DC); }
static inline void spi_dc_high(void) { bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_DC); }

static void spi_send_byte(uint8_t data)
{
    bflb_spi_transfer(spi_dev, &data, NULL, 1);
}

static void spi_send_data(const uint8_t *data, uint32_t len)
{
    bflb_spi_transfer(spi_dev, (uint8_t *)data, NULL, len);
}

/* ─── LCD 命令 ─── */
static void lcd_write_cmd(uint8_t cmd)
{
    spi_dc_low();
    spi_cs_low();
    spi_send_byte(cmd);
    spi_cs_high();
}

static void lcd_write_data(uint8_t data)
{
    spi_dc_high();
    spi_cs_low();
    spi_send_byte(data);
    spi_cs_high();
}

static void lcd_write_data_bulk(const uint8_t *data, uint32_t len)
{
    spi_dc_high();
    spi_cs_low();
    spi_send_data(data, len);
    spi_cs_high();
}

/* ─── ST7789V 初始化命令序列 ─── */
typedef struct {
    uint8_t  cmd;
    const uint8_t *data;
    uint8_t  len;
    uint8_t  delay_ms;
} lcd_init_cmd_t;

static const lcd_init_cmd_t init_cmds[] = {
    { 0x01, NULL, 0, 0 },
    { 0x00, NULL, 0, 10 },       /* delay 10ms */
    { 0x11, NULL, 0, 0 },        /* sleep out */
    { 0x00, NULL, 0, 120 },      /* delay 120ms */
    { 0xB2, (uint8_t[]){0x0C,0x0C,0x00,0x33,0x33}, 5, 0 },
    { 0xB7, (uint8_t[]){0x35}, 1, 0 },
    { 0xBB, (uint8_t[]){0x32}, 1, 0 },
    { 0xC2, (uint8_t[]){0x01}, 1, 0 },
    { 0xC3, (uint8_t[]){0x15}, 1, 0 },
    { 0xC4, (uint8_t[]){0x20}, 1, 0 },
    { 0xC6, (uint8_t[]){0x0F}, 1, 0 },
    { 0xD0, (uint8_t[]){0xA4,0xA1}, 2, 0 },
    { 0xE0, (uint8_t[]){0xD0,0x0A,0x10,0x0C,0x0C,0x18,0x35,0x43,0x4D,0x39,0x13,0x13,0x2D,0x34}, 14, 0 },
    { 0xE1, (uint8_t[]){0xD0,0x05,0x0B,0x06,0x05,0x02,0x35,0x43,0x4D,0x16,0x15,0x15,0x2E,0x32}, 14, 0 },
    { 0x3A, (uint8_t[]){0x55}, 1, 0 },  /* RGB565 */
    { 0x21, NULL, 0, 0 },                /* display inversion on */
    { 0x29, NULL, 0, 0 },                /* display on */
};

/* ─── 公开 API ─── */

int axk_1_47_lcd_init(void)
{
    /* GPIO 初始化 */
    spi_gpio_init();

    /* 复位 */
    bflb_gpio_init(gpio_dev, AXK_1_47_LCD_PIN_RST, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_reset(gpio_dev, AXK_1_47_LCD_PIN_RST);   /* 拉低复位 */
    bflb_mtimer_delay_ms(10);
    bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_RST);     /* 释放 */
    bflb_mtimer_delay_ms(100);

    /* 背光 */
    bflb_gpio_init(gpio_dev, AXK_1_47_LCD_PIN_BLK, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_2);
    bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_BLK);     /* 默认关，等用户调 backlight(true) */

    /* SPI 初始化 */
    struct bflb_spi_config_s spi_cfg = {
        .freq       = AXK_1_47_LCD_SPI_FREQ,
        .role       = SPI_ROLE_MASTER,
        .mode       = SPI_MODE_0,
        .data_width = SPI_DATA_WIDTH_8BIT,
    };
    spi_dev = bflb_device_get_by_name(AXK_1_47_LCD_SPI);
    bflb_spi_init(spi_dev, &spi_cfg);

    /* 发送初始化命令 */
    for (int i = 0; i < sizeof(init_cmds) / sizeof(init_cmds[0]); i++) {
        if (init_cmds[i].cmd == 0x00 && init_cmds[i].data == NULL) {
            bflb_mtimer_delay_ms(init_cmds[i].delay_ms);
        } else {
            lcd_write_cmd(init_cmds[i].cmd);
            if (init_cmds[i].len > 0) {
                lcd_write_data_bulk(init_cmds[i].data, init_cmds[i].len);
            }
        }
    }

    /* MADCTL: 竖屏，RGB顺序 */
    uint8_t madctl = 0x00;
    lcd_write_cmd(0x36);
    lcd_write_data(madctl);

    /* 清屏 */
    axk_1_47_lcd_clear(0x0000);

    return 0;
}

void axk_1_47_lcd_backlight(bool on)
{
    if (on) {
        bflb_gpio_set(gpio_dev, AXK_1_47_LCD_PIN_BLK);
    } else {
        bflb_gpio_reset(gpio_dev, AXK_1_47_LCD_PIN_BLK);
    }
}

void axk_1_47_lcd_set_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* X offset 34: 居中 172 宽在 240 宽的 RAM 中 */
    x1 += 34;
    x2 += 34;

    uint8_t param[4];

    /* Column Address Set (0x2A) */
    param[0] = (x1 >> 8) & 0xFF;
    param[1] = x1 & 0xFF;
    param[2] = (x2 >> 8) & 0xFF;
    param[3] = x2 & 0xFF;
    lcd_write_cmd(0x2A);
    lcd_write_data_bulk(param, 4);

    /* Row Address Set (0x2B) */
    param[0] = (y1 >> 8) & 0xFF;
    param[1] = y1 & 0xFF;
    param[2] = (y2 >> 8) & 0xFF;
    param[3] = y2 & 0xFF;
    lcd_write_cmd(0x2B);
    lcd_write_data_bulk(param, 4);
}

void axk_1_47_lcd_draw_pixels(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *pixels)
{
    uint32_t cnt = (x2 - x1 + 1) * (y2 - y1 + 1);
    axk_1_47_lcd_set_window(x1, y1, x2, y2);
    lcd_write_cmd(0x2C);  /* Memory Write */
    bflb_l1c_dcache_clean_range(pixels, cnt * 2);
    spi_dc_high();
    spi_cs_low();
    spi_send_data((uint8_t *)pixels, cnt * 2);
    spi_cs_high();
}

void axk_1_47_lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint32_t cnt = (x2 - x1 + 1) * (y2 - y1 + 1);
    axk_1_47_lcd_set_window(x1, y1, x2, y2);
    lcd_write_cmd(0x2C);  /* Memory Write */
    spi_dc_high();
    spi_cs_low();
    /* 逐像素填充 */
    uint8_t hi = (color >> 8) & 0xFF;
    uint8_t lo = color & 0xFF;
    for (uint32_t i = 0; i < cnt; i++) {
        spi_send_byte(hi);
        spi_send_byte(lo);
    }
    spi_cs_high();
}

void axk_1_47_lcd_clear(uint16_t color)
{
    axk_1_47_lcd_fill(0, 0, AXK_1_47_LCD_WIDTH - 1, AXK_1_47_LCD_HEIGHT - 1, color);
}

#endif /* SCBB_1_47_LCD_ENABLED */
