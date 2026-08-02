/**
 * @file axk_ina226.c
 * @brief INA226 — 纯 ACLL I2C 跨平台实现。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-07-31
 *
 * @copyright Copyright (c) 2026
 */
#include "axk_ina226.h"

#ifdef SCBB_INA226_ENABLED

#define AXK_INA226_I2C_INIT(n,c)    AXK_INA226_I2C_ACLL(init,n,c)
#define AXK_INA226_I2C_WR(n,a,d,l)  AXK_INA226_I2C_ACLL(write,n,a,d,l)
#define AXK_INA226_I2C_RD(n,a,d,l)  AXK_INA226_I2C_ACLL(read,n,a,d,l)

/** @brief INA226 寄存器地址 */
enum {
    AXK_INA226_REG_CFG = 0x00,   /**< 配置寄存器 */
    AXK_INA226_REG_SV  = 0x01,   /**< 分流电压寄存器 */
    AXK_INA226_REG_BV  = 0x02,   /**< 总线电压寄存器 */
    AXK_INA226_REG_PWR = 0x03,   /**< 功率寄存器 */
    AXK_INA226_REG_CUR = 0x04,   /**< 电流寄存器 */
    AXK_INA226_REG_CAL = 0x05,   /**< 校准寄存器 */
};

/** @brief 校准寄存器值（Current_LSB = 1mA，10mΩ 采样电阻） */
#define AXK_INA226_CAL_VALUE 5120
/** @brief 配置寄存器值（连续测量，1.1ms 转换时间） */
#define AXK_INA226_CFG_CONTINUOUS 0x4127

/**
 * @brief 向寄存器写入 16 位值（寄存器地址 + MSB + LSB）
 *
 * @param[in]  reg  寄存器地址
 * @param[in]  val  16 位写入值
 * @return     int  操作状态
 *              - 0: 写入成功
 *              - 其他: 传输失败
 */
static int axk_ina226_wr16(uint8_t reg, uint16_t val) {
    uint8_t d[3] = {reg, (uint8_t)(val >> 8), (uint8_t)(val & 0xFF)};
    return AXK_INA226_I2C_WR(AXK_INA226_I2C, AXK_INA226_ADDR, d, 3);
}

/**
 * @brief 读取 16 位寄存器值
 *
 * @param[in]  reg      寄存器地址
 * @return     uint16_t 寄存器值（总线错误时返回 0）
 */
static uint16_t axk_ina226_rd16(uint8_t reg) {
    uint8_t d[2] = {0, 0};
    if (AXK_INA226_I2C_WR(AXK_INA226_I2C, AXK_INA226_ADDR, &reg, 1) != 0) {
        return 0;
    }
    AXK_INA226_I2C_RD(AXK_INA226_I2C, AXK_INA226_ADDR, d, 2);
    return (uint16_t)((d[0] << 8) | d[1]);
}

/**
 * @brief 初始化 INA226：I2C 初始化、校准、连续测量模式
 *
 * @return int  0: 成功
 */
int axk_ina226_init(void) {
    bsp_i2c_cfg_t c = { .freq = 100000 };
    if (AXK_INA226_I2C_INIT(AXK_INA226_I2C, &c) != 0) {
        return -1;
    }
    if (axk_ina226_wr16(AXK_INA226_REG_CAL, AXK_INA226_CAL_VALUE) != 0 ||
        axk_ina226_wr16(AXK_INA226_REG_CFG, AXK_INA226_CFG_CONTINUOUS) != 0) {
        return -2;
    }
    return 0;
}

/**
 * @brief 读取总线电压
 *
 * @return float  总线电压（V），LSB=1.25mV
 */
float axk_ina226_read_voltage(void) {
    int16_t raw = (int16_t)axk_ina226_rd16(AXK_INA226_REG_BV);
    return raw * 1.25e-3f;
}

/**
 * @brief 读取负载电流
 *
 * @return float  电流（A），LSB=1mA
 */
float axk_ina226_read_current(void) {
    int16_t raw = (int16_t)axk_ina226_rd16(AXK_INA226_REG_CUR);
    return raw * 1e-3f;
}

/**
 * @brief 读取功率
 *
 * @return float  功率（W），LSB=25mW
 */
float axk_ina226_read_power(void) {
    uint16_t raw = axk_ina226_rd16(AXK_INA226_REG_PWR);
    return raw * 25e-3f;
}
#endif /* SCBB_INA226_ENABLED */
