/**
 * @file ch224_i2c.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _AXK_CH224_H_
#define _AXK_CH224_H_
/**
 * @brief 定义CH224 I2C 接口
 * I2C操作函数，如果没有创建"stm32f10x_bsp_i2c.h"，则需要手动定义
 * 包含适当的I2C头文件，并定义以下宏：
 *
 */
#if __has_include("stm32f10x_bsp_i2c.h")
#include "stm32f10x_bsp_i2c.h"
// 定义I2C延时函数
#define AXK_CH224_DELAY_MS(x) delay_ms(x)
// 定义I2C操作函数
#define AXK_CH224_I2C_ACLL(_func, ...) bsp_i2c_##_func(__VA_ARGS__)
#pragma message "stm32f10x_bsp_i2c.h included for CH224 I2C operations"
#else
#error "Please include the appropriate I2C header for CH224."
#endif
// 定义CH224 I2C 地址
#define AXK_CH224_I2C_ADDR 0x22
// 定义CH224 I2C 命令
#define AXK_CH224_WRITE_CMD 0
// 定义CH224 I2C 读取命令
#define AXK_CH224_READ_CMD 1
// 定义CH224 I2C ACK
#define AXK_CH224_ACK 0
// 定义CH224 I2C NACK
#define AXK_CH224_NACK 1
// 定义CH224 I2C 寄存器
typedef enum {
  AXK_CH224_REG_STATUS = 0x09,
  AXK_CH224_REG_VOUT = 0x0A,
  AXK_CH224_REG_I_DATA = 0x50,
  AXK_CH224_REG_AVS_MSB,
  AXK_CH224_REG_AVS_LSB,
  AXK_CH224_REG_PPS,
} axk_ch224_reg_t;
// 定义CH224 I2C 输出电压
typedef enum {
  AXK_CH224_VOUT_5V = 0x00,
  AXK_CH224_VOUT_9V,
  AXK_CH224_VOUT_12V,
  AXK_CH224_VOUT_15V,
  AXK_CH224_VOUT_20V,
  AXK_CH224_VOUT_28V,
  AXK_CH224_VOUT_PPS, // 输出电压为PPS模式
  AXK_CH224_VOUT_AVS, // 输出电压为AVS模式
} axk_ch224_vout_t;

int axk_ch224_init(void);
int axk_ch224_get_status(axk_ch224_reg_t onlyRreadReg);
int axk_ch224_set_vout(axk_ch224_vout_t VOUT);
int axk_ch224_set_mode(axk_ch224_vout_t _mode);
int axk_ch224_set_pps_vout(float PPS_VOUT);
int axk_ch224_set_avs_vout(float AVS_VOUT);
#endif
