/**
 * @file axk_st7789v_lcd.c
 * @brief ST7789V LCD — 直接封装 SDK 已验证的 lcd.h 驱动。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "bflb_gpio.h"
#include "lcd.h"

#include "axk_st7789v_lcd.h"

#ifdef SCBB_ST7789V_LCD_ENABLED

/**
 * @brief SPI GPIO 引脚初始化为 SPI0 复用功能
 */
void axk_st7789v_lcd_spi_gpio_init(void) {
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, AXK_ST7789V_LCD_PIN_SDA,
                   GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, AXK_ST7789V_LCD_PIN_SCL,
                   GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

/**
 * @brief 初始化 ST7789V LCD，发送初始化序列并清屏
 *
 * @return int  0: 成功
 */
int axk_st7789v_lcd_init(void) {
    return lcd_init();
}

/**
 * @brief 开关 LCD 背光
 *
 * @param[in]  on  true: 开，false: 关
 */
void axk_st7789v_lcd_backlight(bool on) {
    lcd_backlight_toggle(on);
}

/**
 * @brief 向指定窗口绘制像素块（阻塞传输）
 *
 * @param[in]  x1  起始 X
 * @param[in]  y1  起始 Y
 * @param[in]  x2  结束 X
 * @param[in]  y2  结束 Y
 * @param[in]  *px RGB565 像素数据
 */
void axk_st7789v_lcd_draw_pixels(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t *px) {
    if (px == NULL) {
        return;
    }
    lcd_draw_picture_blocking(x1, y1, x2, y2, (lcd_color_t *)px);
}

/**
 * @brief 用纯色填充矩形区域
 *
 * @param[in]  x1  起始 X
 * @param[in]  y1  起始 Y
 * @param[in]  x2  结束 X
 * @param[in]  y2  结束 Y
 * @param[in]  c   RGB565 填充颜色
 */
void axk_st7789v_lcd_fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t c) {
    lcd_draw_area(x1, y1, x2, y2, c);
}

/**
 * @brief 清屏为单一颜色
 *
 * @param[in]  c  RGB565 填充颜色
 */
void axk_st7789v_lcd_clear(uint16_t c) {
    lcd_clear(c);
}

#endif /* SCBB_ST7789V_LCD_ENABLED */
