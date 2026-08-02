/**
 * @file bl616_bsp_i2c.h
 * @author Ai-Thinker-Open
 * @brief BL616/BL618 I2C 板级支持包（bflb_i2c 封装）。
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#ifndef BL616_BSP_I2C_H
#define BL616_BSP_I2C_H

#include <stdint.h>

/**
 * @brief I2C 配置结构体
 */
typedef struct {
    uint32_t freq;  /**< I2C 时钟频率（Hz） */
} bsp_i2c_cfg_t;

/**
 * @brief 初始化 I2C（默认 I2C0: SCL=IO28, SDA=IO27）
 *
 * @param[in]  name  I2C 设备名（如 "i2c0"）
 * @param[in]  cfg   I2C 配置指针，不可为 NULL
 * @return     int   操作状态
 *              - 0: 初始化成功
 *              - -1: cfg 为空
 */
int bsp_i2c_init(const char *name, bsp_i2c_cfg_t *cfg);

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
int bsp_i2c_write(const char *name, uint8_t addr, const uint8_t *data, uint32_t len);

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
int bsp_i2c_read(const char *name, uint8_t addr, uint8_t *data, uint32_t len);

#endif /* BL616_BSP_I2C_H */
