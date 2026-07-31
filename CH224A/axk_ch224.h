/**
 * @file axk_ch224.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief CH224A USB-PD 受电端控制器驱动（I2C 协议）。
 *
 * 支持 5V 到 28V 电压协商，含 PPS 和 AVS 模式。
 * 跨平台：通过 scbb_config.h 中的 ACLL 宏实现 BSP 抽象。
 *
 * @version 0.1
 * @date 2026-01-26
 * @copyright Copyright (c) 2026
 */
#ifndef _AXK_CH224_H_
#define _AXK_CH224_H_

#include "scbb_config.h"

#ifdef SCBB_CH224A_ENABLED

#ifdef SCBB_CH224A_I2C_HEADER
#include SCBB_CH224A_I2C_HEADER
#endif

#ifdef SCBB_CH224A_DELAY_HEADER
#include SCBB_CH224A_DELAY_HEADER
#endif

#define AXK_CH224_I2C_ADDR  0x22  /**< CH224A I2C 7 位设备地址 */
#define AXK_CH224_WRITE_CMD 0     /**< I2C 写命令位 */
#define AXK_CH224_READ_CMD  1     /**< I2C 读命令位 */
#define AXK_CH224_ACK       0     /**< I2C ACK 应答标志 */
#define AXK_CH224_NACK      1     /**< I2C NACK 非应答标志 */

/** @brief CH224A 寄存器地址 */
typedef enum {
    AXK_CH224_REG_STATUS  = 0x09,  /**< 状态寄存器（只读） */
    AXK_CH224_REG_VOUT    = 0x0A,  /**< 电压输出寄存器 */
    AXK_CH224_REG_I_DATA  = 0x50,  /**< 中间数据寄存器 */
    AXK_CH224_REG_AVS_MSB,         /**< AVS 电压 MSB 寄存器 */
    AXK_CH224_REG_AVS_LSB,         /**< AVS 电压 LSB 寄存器 */
    AXK_CH224_REG_PPS,             /**< PPS 电压寄存器 */
} axk_ch224_reg_t;

/** @brief CH224A 电压输出模式 */
typedef enum {
    AXK_CH224_VOUT_5V  = 0x00,  /**< 固定 5V 输出 */
    AXK_CH224_VOUT_9V,          /**< 固定 9V 输出 */
    AXK_CH224_VOUT_12V,         /**< 固定 12V 输出 */
    AXK_CH224_VOUT_15V,         /**< 固定 15V 输出 */
    AXK_CH224_VOUT_20V,         /**< 固定 20V 输出 */
    AXK_CH224_VOUT_28V,         /**< 固定 28V 输出 */
    AXK_CH224_VOUT_PPS,         /**< 可编程电源 PPS 模式 */
    AXK_CH224_VOUT_AVS,         /**< 自适应电压调节 AVS 模式 */
} axk_ch224_vout_t;

/**
 * @brief 初始化 CH224A 并通过 I2C 检测设备是否存在
 *
 * 发送起始信号，写入设备地址（写命令），等待 ACK 应答。
 *
 * @return int  0: 设备有应答，1: 无应答
 * @note  调用前需确保 BSP I2C 已初始化
 * @see   axk_ch224_set_vout()
 */
int axk_ch224_init(void);

/**
 * @brief 通过 I2C 读取 CH224A 寄存器的值
 *
 * 先写寄存器地址，重复起始后读一字节，NACK 结束。
 *
 * @param[in]  onlyRreadReg  要读取的寄存器地址
 * @return     int            寄存器值或错误码
 *              - >=0: 读取到的寄存器值
 *              - -1: 设备地址无应答
 *              - -2: 寄存器地址无应答
 *              - -3: 重复起始后设备无应答
 *              - -4: 该寄存器为只写寄存器，不可读
 */
int axk_ch224_get_status(axk_ch224_reg_t onlyRreadReg);

/**
 * @brief 设置 CH224A 输出电压模式
 *
 * 向 VOUT 寄存器写入电压模式值。固定电压（5V–28V）或 PPS/AVS 模式。
 *
 * @param[in]  VOUT  目标电压输出模式
 * @return     int   操作状态
 *              - 0: 写入成功
 *              - -1: 设备地址无应答
 *              - -2: VOUT 寄存器地址无应答
 *              - -3: 电压值写入无应答
 * @see   axk_ch224_set_pps_vout(), axk_ch224_set_avs_vout()
 */
int axk_ch224_set_vout(axk_ch224_vout_t VOUT);

/**
 * @brief 将 CH224A 配置为 PPS 或 AVS 模式
 *
 * 之后调用 axk_ch224_set_pps_vout() 或 axk_ch224_set_avs_vout() 设置具体电压。
 *
 * @param[in]  _mode  须为 AXK_CH224_VOUT_PPS 或 AXK_CH224_VOUT_AVS
 * @return     int    操作状态
 *              - 0: 配置成功
 *              - -1: 设备地址无应答
 *              - -2: 寄存器地址无应答
 *              - -3: 模式值写入无应答
 *              - -4: 无效模式参数
 */
int axk_ch224_set_mode(axk_ch224_vout_t _mode);

/**
 * @brief 设置 PPS 模式输出电压
 *
 * @param[in]  PPS_VOUT  目标电压（V），范围 5.0–28.0，精度 0.1V
 * @return     int        操作状态
 *              - 0: 设置成功
 *              - -1: 设备地址无应答
 *              - -4: PPS 寄存器地址无应答
 *              - -5: PPS 电压值写入无应答
 *              - -10: 电压超出范围
 */
int axk_ch224_set_pps_vout(float PPS_VOUT);

/**
 * @brief 设置 AVS 模式输出电压
 *
 * 以 MSB/LSB 双字节格式写入 AVS 电压值。
 *
 * @param[in]  AVS_VOUT  目标电压（V），范围 5.0–28.0，精度 0.1V
 * @return     int        操作状态
 *              - 0: 设置成功
 *              - -1: 设备地址无应答
 *              - -2: AVS MSB 寄存器地址无应答
 *              - -3: AVS LSB 数据写入无应答
 *              - -4: AVS MSB 数据写入无应答
 *              - -10: 电压超出范围
 */
int axk_ch224_set_avs_vout(float AVS_VOUT);

#endif /* SCBB_CH224A_ENABLED */
#endif /* _AXK_CH224_H_ */
