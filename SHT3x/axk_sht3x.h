/**
 * @file axk_sht3x.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_SHT3X_H__
#define __AXK_SHT3X_H__
/**
 * @brief 定义SHT3x
 * I2C操作函数，如果没有创建"stm32f10x_bsp_i2c.h"，则需要手动定义
 * 包含适当的I2C头文件，并定义以下宏：
 *
 */
#if __has_include("stm32f10x_bsp_i2c.h")
#include "stm32f10x_bsp_i2c.h"
#define AXK_SHT3X_DELAY_MS(x) delay_ms(x)
#define AXK_SHT3X_I2C_ACLL(_func, ...) bsp_i2c_##_func(__VA_ARGS__)
#pragma message "stm32f10x_bsp_i2c.h included for AXK_SHT3X I2C operations"
#else
#error "Please include the appropriate I2C header for AXK_SHT3X."
#endif

#define AXK_SHT3X_ADDRESS 0x44 // SHT3x默认I2C地址

#define AXK_SHT3X_WRITE_CMD 0 // SHT3x写操作指令
#define AXK_SHT3X_READ_CMD 1  // SHT3x读操作指令

#define AXK_SHT3X_ACK 0  // 应答
#define AXK_SHT3X_NACK 1 // 非应答

unsigned char axk_sht3x_init(void);
unsigned char axk_sht3x_set_mode(unsigned int _mode);
unsigned char axk_sht3x_read(unsigned int _mode, double *temp,
                             double *humidity);
#endif /* __AXK_SHT3X_H__ */