/**
 * ST7789V LCD — 直接封装 SDK 已验证的 lcd.h 驱动
 */
#include "axk_st7789v_lcd.h"
#include "bflb_gpio.h"
#include "lcd.h"

#ifdef SCBB_ST7789V_LCD_ENABLED

void xemowo_lcd_spi_gpio_init(void)
{
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, AXK_ST7789V_LCD_PIN_SDA,
                   GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLDOWN | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, AXK_ST7789V_LCD_PIN_SCL,
                   GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

int axk_st7789v_lcd_init(void)           { return lcd_init(); }
void axk_st7789v_lcd_backlight(bool on)  { lcd_backlight_toggle(on); }
void axk_st7789v_lcd_draw_pixels(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t *px)
    { lcd_draw_picture_blocking(x1,y1,x2,y2,(lcd_color_t*)px); }
void axk_st7789v_lcd_fill(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t c)
    { lcd_draw_area(x1,y1,x2,y2,c); }
void axk_st7789v_lcd_clear(uint16_t c)
    { lcd_clear(c); }

#endif
