/**
 * @file axk_sht3x.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief SHT3x 温湿度传感器驱动（I2C 协议）。
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_SHT3X_H__
#define __AXK_SHT3X_H__

/**
 * @brief I2C BSP 头文件配置。
 *        如果需要使用自定义 BSP，请在 CMake 中设置 SCBB_BSP_I2C_HEADER。
 */
#ifdef SCBB_BSP_I2C_HEADER
#include SCBB_BSP_I2C_HEADER
#else
#if __has_include("stm32f10x_bsp_i2c.h")
#include "stm32f10x_bsp_i2c.h"
#else
#error "Please include the appropriate I2C header for SHT3x or set SCBB_BSP_I2C_HEADER."
#endif
#endif

#define AXK_SHT3X_DELAY_MS(x) delay_ms(x)
#define AXK_SHT3X_I2C_ACLL(_func, ...) bsp_i2c_##_func(__VA_ARGS__)

#define AXK_SHT3X_ADDRESS 0x44

#define AXK_SHT3X_WRITE_CMD 0
#define AXK_SHT3X_READ_CMD 1

#define AXK_SHT3X_ACK 0
#define AXK_SHT3X_NACK 1

unsigned char axk_sht3x_init(void);
unsigned char axk_sht3x_set_mode(unsigned int _mode);
unsigned char axk_sht3x_read(unsigned int _mode, double *temp,
                             double *humidity);
#endif /* __AXK_SHT3X_H__ */
