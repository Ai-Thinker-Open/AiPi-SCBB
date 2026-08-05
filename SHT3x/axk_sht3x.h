/**
 * @file axk_sht3x.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief SHT3x 温湿度传感器驱动（I2C 协议）。
 *
 * 支持 SHT30/SHT31/SHT35，含 CRC-8 数据校验。传感器 I2C 地址: 0x44。
 * 跨平台：通过 scbb_config.h 中的 ACLL 宏实现 BSP 抽象。
 *
 * @version 0.1
 * @date 2026-01-29
 * @copyright Copyright (c) 2026
 */
#ifndef __AXK_SHT3X_H__
#define __AXK_SHT3X_H__

#include "scbb_config.h"

#ifdef SCBB_SHT3X_ENABLED

#ifdef SCBB_SHT3X_I2C_HEADER
#include SCBB_SHT3X_I2C_HEADER
#endif

#ifdef SCBB_SHT3X_DELAY_HEADER
#include SCBB_SHT3X_DELAY_HEADER
#endif

#define AXK_SHT3X_ADDRESS 0x44  /**< SHT3x I2C 7 位设备地址 */

#define AXK_SHT3X_WRITE_CMD 0   /**< I2C 写命令位 */
#define AXK_SHT3X_READ_CMD  1   /**< I2C 读命令位 */

#define AXK_SHT3X_ACK       0   /**< I2C ACK 应答标志 */
#define AXK_SHT3X_NACK      1   /**< I2C NACK 非应答标志 */

/**
 * @brief 在 I2C 总线上探测 SHT3x 传感器
 *
 * 发送设备写地址并检查 ACK 应答。
 *
 * @return unsigned char  0: 传感器响应正常，1: 传感器无应答
 * @note  硬件初始化后调用一次即可
 * @see   axk_sht3x_read()
 */
unsigned char axk_sht3x_init(void);

/**
 * @brief 向 SHT3x 发送测量指令
 *
 * 写入 2 字节指令，触发一次测量周期。
 *
 * @param[in]  _mode          16 位测量指令码
 * @return     unsigned char  操作状态
 *              - 0: 写入成功
 *              - 1: 设备地址无应答
 *              - 2: 高字节无应答
 *              - 3: 低字节无应答
 */
unsigned char axk_sht3x_set_mode(unsigned int _mode);

/**
 * @brief 发送测量指令并读取温湿度数据
 *
 * 发送指令 → 等待转换 → 读 6 字节 → CRC-8 校验 → 公式换算。
 *
 * @param[in]  _mode          测量指令码
 * @param[out] *temp          温度输出指针（摄氏度），不可为 NULL
 * @param[out] *humidity      湿度输出指针（%RH），不可为 NULL
 * @return     unsigned char  操作状态
 *              - 0: 读取并校验成功
 *              - 1: 设备地址无 I2C ACK
 *              - 2: 指令高字节无 ACK
 *              - 3: 指令低字节无 ACK
 *              - 4: 等待转换超时
 *              - 5: CRC 校验失败
 *              - 6: temp 或 humidity 为空指针
 */
unsigned char axk_sht3x_read(unsigned int _mode, double *temp,
                             double *humidity);

#endif /* SCBB_SHT3X_ENABLED */
#endif /* __AXK_SHT3X_H__ */
