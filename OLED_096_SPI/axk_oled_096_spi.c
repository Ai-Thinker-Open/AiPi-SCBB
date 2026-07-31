/**
 * @file axk_oled_096_spi.c
 * @brief SSD1306 0.96" OLED 128x64 SPI — 纯 ACLL 跨平台
 */
#include "axk_oled_096_spi.h"
#include <string.h>

#ifdef SCBB_OLED_096_SPI_ENABLED

#define SI(n,c)  AXK_OLED_096_SPI_SPI_ACLL(init,n,c)
#define S8(n,b)  AXK_OLED_096_SPI_SPI_ACLL(send8,n,b)
#define GI(p,m)  AXK_OLED_096_SPI_GPIO_ACLL(init,p,m)
#define GS(p)    AXK_OLED_096_SPI_GPIO_ACLL(set,p)
#define GR(p)    AXK_OLED_096_SPI_GPIO_ACLL(reset,p)
#define DMS(x)   AXK_OLED_096_SPI_DELAY_MS(x)

#define CS AXK_OLED_096_SPI_PIN_CS
#define DC AXK_OLED_096_SPI_PIN_DC
#define RS AXK_OLED_096_SPI_PIN_RST
#define SDA AXK_OLED_096_SPI_PIN_SDA
#define SCL AXK_OLED_096_SPI_PIN_SCL
#define DEV AXK_OLED_096_SPI_SPI

static void cL(void){GR(CS);} static void cH(void){GS(CS);}
static void dL(void){GR(DC);} static void dH(void){GS(DC);}

static void cmd(uint8_t c) { dL();cL();S8(DEV,c);cH(); }
static void dat(uint8_t d) { dH();cL();S8(DEV,d);cH(); }

static uint8_t fb[128][8]; /* 128x64 framebuffer */

int axk_oled_096_spi_init(void) {
    GI(CS,1);cH();GI(DC,1);dH();
    GI(RS,1);GR(RS);DMS(10);GS(RS);DMS(100);

    GI(SDA,2);GI(SCL,3);
    bsp_spi_cfg_t c={.freq=4000000,.mode=3,.data_width=8};
    SI(DEV,&c);

    /* SSD1306 初始化命令 */
    uint8_t init[] = {
        0xAE,        /* display off */
        0xD5,0x80,   /* clock div */
        0xA8,0x3F,   /* mux ratio 64 */
        0xD3,0x00,   /* display offset */
        0x40,        /* start line */
        0x8D,0x14,   /* charge pump */
        0x20,0x00,   /* horizontal addressing */
        0xA1,        /* segment remap */
        0xC8,        /* COM scan direction */
        0xDA,0x12,   /* COM pins */
        0x81,0xCF,   /* contrast */
        0xD9,0xF1,   /* pre-charge */
        0xDB,0x40,   /* VCOM detect */
        0xA4,        /* display on resume */
        0xA6,        /* normal display */
        0xAF,        /* display on */
    };
    for(int i=0;i<sizeof(init);i++)cmd(init[i]);
    axk_oled_096_spi_clear(0);
    axk_oled_096_spi_flush();
    return 0;
}

void axk_oled_096_spi_clear(uint8_t color) {
    memset(fb, color ? 0xFF : 0x00, sizeof(fb));
}

void axk_oled_096_spi_set_pixel(int x, int y, int on) {
    if(x<0||x>=128||y<0||y>=64)return;
    if(on) fb[x][y/8] |= (1<<(y%8));
    else   fb[x][y/8] &= ~(1<<(y%8));
}

void axk_oled_096_spi_flush(void) {
    for(int page=0; page<8; page++) {
        cmd(0xB0+page);   /* set page */
        cmd(0x00);        /* set low column = 0 */
        cmd(0x10);        /* set high column = 0 */
        for(int x=0; x<128; x++)dat(fb[x][page]);
    }
}
#endif
