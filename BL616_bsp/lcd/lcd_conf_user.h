#ifndef _LCD_CONF_USER_H_
#define _LCD_CONF_USER_H_

/* Ai-M61-32S + 1.47" IPS LCD (ST7789V, 172x320), 硬件SPI0 */

#define LCD_SPI_ST7789V

/* ---------- LCD 面板 ---------- */
#if defined LCD_SPI_ST7789V
    #define LCD_SPI_INTERFACE_TYPE 1
    #define ST7789V_SPI_PIXEL_FORMAT 1   /* rgb565 */
    #define ST7789V_SPI_W 172
    #define ST7789V_SPI_H 320
    #define ST7789V_SPI_OFFSET_X 34
    #define ST7789V_SPI_OFFSET_Y 0
    #define ST7789V_SPI_COLOR_ORDER 0
    #define ST7789V_SPI_COLOR_REVERSAL 0
    #define ST7789V_SPI_DIR_MIRROR 0
#endif

/* ---------- 硬件SPI0: MOSI=IO11, SCLK=IO13 ---------- */
#define LCD_SPI_HARD_4_NAME             "spi0"
#define LCD_SPI_HARD_4_GPIO_INIT_FUNC   xemowo_lcd_spi_gpio_init
#define LCD_SPI_HARD_4_DMA_NAME         "dma0_ch3"
#define LCD_SPI_HARD_4_PIXEL_CNT_MAX    (320 * 480)

/* 软件控制引脚 (避开 RGB=12/14/15 和 NC=2/3/4/5/6/7/8/9/34) */
#define LCD_SPI_HARD_4_PIN_CS   GPIO_PIN_10   /* IO10 */
#define LCD_SPI_HARD_4_PIN_DC   GPIO_PIN_1    /* IO1  */

/* ---------- 复位 ---------- */
#define LCD_RESET_EN            1
#define LCD_RESET_PIN           GPIO_PIN_18   /* IO18 */
#define LCD_RESET_ACTIVE_LEVEL  0
#define LCD_RESET_HOLD_MS       10
#define LCD_RESET_DELAY         100

/* ---------- 背光 ---------- */
#define LCD_BACKLIGHT_EN            1
#define LCD_BACKLIGHT_PIN           GPIO_PIN_20   /* IO20 */
#define LCD_BACKLIGHT_ACTIVE_LEVEL  1

#endif
