/**
 * BL616/BL618 BSP I2C — 包 bflb_i2c
 */
#include "bl616_bsp_i2c.h"
#include "bflb_i2c.h"
#include "bflb_gpio.h"

static struct bflb_device_s *dev;

int bsp_i2c_init(const char *name, bsp_i2c_cfg_t *cfg) {
    dev = bflb_device_get_by_name(name);
    bflb_i2c_init(dev, cfg->freq);
    /* 默认 I2C0: SCL=IO28, SDA=IO27 */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, 28, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, 27, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    return 0;
}

int bsp_i2c_write(const char *name, uint8_t addr, const uint8_t *data, uint32_t len) {
    struct bflb_i2c_msg_s msgs[] = {{.addr=addr,.flags=0,.buffer=(uint8_t*)data,.length=len}};
    return bflb_i2c_transfer(dev, msgs, 1);
}

int bsp_i2c_read(const char *name, uint8_t addr, uint8_t *data, uint32_t len) {
    struct bflb_i2c_msg_s msgs[] = {{.addr=addr,.flags=I2C_M_READ,.buffer=data,.length=len}};
    return bflb_i2c_transfer(dev, msgs, 1);
}
