/**
 * @file bl616_bsp_i2c.c
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 I2C 板级支持包实现（bflb_i2c 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "bl616_bsp_i2c.h"
#include "bflb_i2c.h"
#include "bflb_gpio.h"

/** I2C 设备句柄 */
static struct bflb_device_s *s_bsp_i2c_dev;

/**
 * @brief 初始化 I2C（默认 I2C0: SCL=IO28, SDA=IO27）
 *
 * @param[in]  name  I2C 设备名（如 "i2c0"）
 * @param[in]  cfg   I2C 配置指针，不可为 NULL
 * @return     int   操作状态
 *              - 0: 初始化成功
 *              - -1: cfg 为空
 */
int bsp_i2c_init(const char *name, bsp_i2c_cfg_t *cfg) {
    if (cfg == NULL) {
        return -1;
    }

    s_bsp_i2c_dev = bflb_device_get_by_name(name);
    bflb_i2c_init(s_bsp_i2c_dev, cfg->freq);

    /* 默认 I2C0: SCL=IO28, SDA=IO27 */
    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, 28,
                   GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, 27,
                   GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    return 0;
}

/**
 * @brief 向从设备写入数据
 *
 * @param[in]  name  I2C 设备名
 * @param[in]  addr  从设备 7 位地址
 * @param[in]  data  待写数据指针，不可为 NULL
 * @param[in]  len   数据长度
 * @return     int   操作状态
 *              - 0: 写入成功
 *              - 其他: 传输失败
 */
int bsp_i2c_write(const char *name, uint8_t addr, const uint8_t *data, uint32_t len) {
    (void)name;

    if (data == NULL || len == 0) {
        return -1;
    }
    struct bflb_i2c_msg_s msgs[] = {
        {.addr = addr, .flags = 0, .buffer = (uint8_t *)data, .length = len}};
    return bflb_i2c_transfer(s_bsp_i2c_dev, msgs, 1);
}

/**
 * @brief 从从设备读取数据
 *
 * @param[in]  name  I2C 设备名
 * @param[in]  addr  从设备 7 位地址
 * @param[out] data  数据输出缓冲区指针，不可为 NULL
 * @param[in]  len   数据长度
 * @return     int   操作状态
 *              - 0: 读取成功
 *              - 其他: 传输失败
 */
int bsp_i2c_read(const char *name, uint8_t addr, uint8_t *data, uint32_t len) {
    (void)name;

    if (data == NULL || len == 0) {
        return -1;
    }
    struct bflb_i2c_msg_s msgs[] = {
        {.addr = addr, .flags = I2C_M_READ, .buffer = data, .length = len}};
    return bflb_i2c_transfer(s_bsp_i2c_dev, msgs, 1);
}
