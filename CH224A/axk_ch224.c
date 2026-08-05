/**
 * @file axk_ch224.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief CH224A USB-PD 受电端控制器驱动实现（I2C）。
 * @version 0.1
 * @date 2026-01-26
 * @copyright Copyright (c) 2026
 */
#include <stdint.h>

#include "log.h"

#include "axk_ch224.h"

#ifdef SCBB_CH224A_ENABLED

/**
 * @brief 初始化 I2C，发送设备地址并等待 ACK 应答
 *
 * @return int 0: 成功，1: 无 ACK 应答
 */
int axk_ch224_init(void) {
  int ack;
  AXK_CH224A_I2C_ACLL(init);
  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte,
                     AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  ack = (int)AXK_CH224A_I2C_ACLL(wait_ack);
  AXK_CH224A_I2C_ACLL(stop);
  return ack;
}

/**
 * @brief 通过 I2C 读取 CH224A 寄存器值
 *
 * @param[in]  onlyRreadReg  要读取的寄存器地址
 * @return     int            寄存器值或错误码
 *              - >=0: 读取到的寄存器值
 *              - -1: 设备地址无应答
 *              - -2: 寄存器地址无应答
 *              - -3: 重复起始后设备无应答
 *              - -4: 寄存器不可读（只写寄存器）
 */
int axk_ch224_get_status(axk_ch224_reg_t onlyRreadReg) {
  unsigned char status = 0;
  if (onlyRreadReg == AXK_CH224_REG_VOUT ||
      onlyRreadReg == AXK_CH224_REG_AVS_LSB ||
      onlyRreadReg == AXK_CH224_REG_AVS_MSB ||
      onlyRreadReg == AXK_CH224_REG_PPS) {
    return -4;
  }

  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -1;
  }

  AXK_CH224A_I2C_ACLL(send_byte, onlyRreadReg);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -2;
  }

  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_READ_CMD);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -3;
  }

  status = AXK_CH224A_I2C_ACLL(read_byte);
  AXK_CH224A_I2C_ACLL(send_ack, AXK_CH224_NACK);
  AXK_CH224A_I2C_ACLL(stop);
  return (int)status;
}

/**
 * @brief 设置 CH224A 输出电压模式
 *
 * @param[in]  value  目标电压模式（axk_ch224_vout_t 枚举值）
 * @return     int    操作状态
 *              - 0: 写入成功
 *              - -1: 设备地址无应答
 *              - -2: 寄存器地址无应答
 *              - -3: 电压值写入无应答
 */
int axk_ch224_set_vout(axk_ch224_vout_t value) {
  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -1;
  }

  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_REG_VOUT);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -2;
  }

  AXK_CH224A_I2C_ACLL(send_byte, value);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -3;
  }

  AXK_CH224A_I2C_ACLL(stop);
  return 0;
}

/**
 * @brief 配置 CH224A 为 PPS 或 AVS 模式
 *
 * @param[in]  _mode  目标模式，须为 AXK_CH224_VOUT_PPS 或 AXK_CH224_VOUT_AVS
 * @return     int    操作状态
 *              - 0: 配置成功
 *              - -1: 设备地址无应答
 *              - -2: 寄存器地址无应答
 *              - -3: 模式值写入无应答
 *              - -4: 无效模式参数
 */
int axk_ch224_set_mode(axk_ch224_vout_t _mode) {
  if (_mode != AXK_CH224_VOUT_PPS && _mode != AXK_CH224_VOUT_AVS) {
    return -4;
  }
  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -1;
  }
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_REG_VOUT);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -2;
  }
  AXK_CH224A_I2C_ACLL(send_byte, _mode);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -3;
  }
  AXK_CH224A_I2C_ACLL(stop);
  return 0;
}

/**
 * @brief 设置 PPS 模式输出电压
 *
 * @param[in]  PPS_VOUT  目标电压（V），范围 5.0–28.0，精度 0.1V
 * @return     int        操作状态
 *              - 0: 设置成功
 *              - -1: 设备地址无应答
 *              - -4: PPS 寄存器地址无应答
 *              - -5: PPS 电压值写入无应答
 *              - -10: 电压超出有效范围（5.0–28.0V）
 */
int axk_ch224_set_pps_vout(float PPS_VOUT) {
  if (PPS_VOUT < 5.0 || PPS_VOUT > 28.0) {
    return -10;
  }
  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -1;
  }
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_REG_PPS);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -4;
  }
  unsigned char pps_vout = PPS_VOUT * 10.0;
  AXK_CH224A_I2C_ACLL(send_byte, pps_vout);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -5;
  }
  AXK_CH224A_I2C_ACLL(stop);
  return 0;
}

/**
 * @brief 设置 AVS 模式输出电压
 *
 * @param[in]  AVS_VOUT  目标电压（V），范围 5.0–28.0，精度 0.1V
 * @return     int        操作状态
 *              - 0: 设置成功
 *              - -1: 设备地址无应答
 *              - -2: AVS MSB 寄存器地址无应答
 *              - -3: AVS LSB 数据写入无应答
 *              - -4: AVS MSB 数据写入无应答
 *              - -10: 电压超出有效范围（5.0–28.0V）
 */
int axk_ch224_set_avs_vout(float AVS_VOUT) {
  if (AVS_VOUT < 5.0 || AVS_VOUT > 28.0) {
    return -10;
  }
  unsigned int avs_vout = (unsigned char)(AVS_VOUT * 10.0);
  unsigned char AVS_LSB = 0X00;
  unsigned char AVS_MSB = 0X00;
  AVS_MSB = ((avs_vout >> 8) & 0XFF00) | 0X80;
  AVS_LSB = avs_vout & 0X00FF;

  AXK_CH224A_I2C_ACLL(start);
  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -1;
  }

  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_REG_AVS_LSB);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -3;
  }
  AXK_CH224A_I2C_ACLL(send_byte, AVS_LSB);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -3;
  }

  AXK_CH224A_I2C_ACLL(send_byte, AXK_CH224_REG_AVS_MSB);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -2;
  }
  AXK_CH224A_I2C_ACLL(send_byte, AVS_MSB);
  if (AXK_CH224A_I2C_ACLL(wait_ack)) {
    AXK_CH224A_I2C_ACLL(stop);
    return -4;
  }
  AXK_CH224A_I2C_ACLL(stop);
  return 0;
}

#endif /* SCBB_CH224A_ENABLED */
