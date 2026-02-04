/**
 * @file ch224_i2c.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_ch224.h"
#include "log.h"
#include <stdint.h>
#include <sys/_intsup.h>
#include <sys/_types.h>
/**
 * @brief 初始化CH224 I2C
 *
 * @return unsigned char 0:成功 1:失败
 */
int axk_ch224_init(void) {
  int ack;
  // 初始化I2C外设
  AXK_CH224_I2C_ACLL(init);
  // 发送起始信号
  AXK_CH224_I2C_ACLL(start);
  // 发送设备地址
  AXK_CH224_I2C_ACLL(send_byte,
                     AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD); // 写操作
  // 等待ACK,检查 CH224 是否响应
  ack = (int)AXK_CH224_I2C_ACLL(wait_ack);
  AXK_CH224_I2C_ACLL(stop);
  return ack;
}
/**
 * @brief 获取CH224状态寄存器
 *
 * @return int 成功返回状态寄存器值，失败返回错误码
 *         -1: 设备无响应
 *         -2: 发送寄存器地址失败
 *         -3: 重复起始后设备无响应
 */
int axk_ch224_get_status(axk_ch224_reg_t onlyRreadReg) {
  unsigned char status = 0;
  // 判断是否为只读寄存器
  if (onlyRreadReg == AXK_CH224_REG_VOUT ||
      onlyRreadReg == AXK_CH224_REG_AVS_LSB ||
      onlyRreadReg == AXK_CH224_REG_AVS_MSB ||
      onlyRreadReg == AXK_CH224_REG_PPS) {
    return -4; // 寄存器不是只读寄存器
  }

  // 1. 发送起始信号
  AXK_CH224_I2C_ACLL(start);

  // 2. 发送设备地址+写命令 (0x22 << 1 | 0 = 0x44)
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);

  // 3. 等待设备ACK，检查设备是否响应
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -1;                // 设备无响应
  }

  // 4. 发送要读取的寄存器地址 (如状态寄存器地址: 0x09)
  AXK_CH224_I2C_ACLL(send_byte, onlyRreadReg);

  // 5. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -2;                // 发送寄存器地址失败
  }

  // 6. 发送重复起始信号 (Restart)
  AXK_CH224_I2C_ACLL(start);

  // 7. 发送设备地址+读命令 (0x22 << 1 | 1 = 0x45)
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_READ_CMD);

  // 8. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -3;                // 重复起始后设备无响应
  }

  // 9. 读取寄存器数据
  status = AXK_CH224_I2C_ACLL(read_byte);

  // 10. 发送NACK，表示不再读取更多数据
  AXK_CH224_I2C_ACLL(send_ack, AXK_CH224_NACK);

  // 11. 发送停止信号
  AXK_CH224_I2C_ACLL(stop);

  // 返回读取到的状态寄存器值
  return (int)status;
}
/**
 * @brief 设置CH224电压寄存器
 *
 * @param value 要设置的电压值
 */
int axk_ch224_set_vout(axk_ch224_vout_t value) {
  // 1. 发送起始信号
  AXK_CH224_I2C_ACLL(start);

  // 2. 发送设备地址+写命令 (0x22 << 1 | 0 = 0x44)
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);

  // 3. 等待设备ACK，检查设备是否响应
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -1;                // 设备无响应
  }

  // 4. 发送要写入的寄存器地址
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_REG_VOUT);

  // 5. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -2;                // 发送寄存器地址失败
  }

  // 6. 发送要写入的电压值
  AXK_CH224_I2C_ACLL(send_byte, value);

  // 7. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -3;                // 发送电压值失败
  }

  // 8. 发送停止信号
  AXK_CH224_I2C_ACLL(stop);

  return 0; // 成功
}
/**
 * @brief 设置CH224 PPS模式
 *
 * @return int
 */
int axk_ch224_set_mode(axk_ch224_vout_t _mode) {
  // 判断是否为模式设置
  if (_mode != AXK_CH224_VOUT_PPS && _mode != AXK_CH224_VOUT_AVS) {
    return -4; // 寄存器不是模式参数
  }
  AXK_CH224_I2C_ACLL(start);
  // 2. 发送设备地址+写命令 (0x22 << 1 | 0 = 0x44)
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  // 3. 等待设备ACK，检查设备是否响应
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -1;                // 设备无响应
  }
  // 8. 配置电压寄存器
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_REG_VOUT);
  // 9. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -2;                // 发送寄存器地址失败
  }
  // 10. 配置为模式
  AXK_CH224_I2C_ACLL(send_byte, _mode);
  // 11. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -3;                // 发送PPS模式失败
  }
  // 12. 发送停止信号
  AXK_CH224_I2C_ACLL(stop);
  return 0; // 成功
}
/**
 * @brief 设置CH224 PPS模式输出电压
 *
 * @param PPS_VOUT PPS模式输出电压值，范围5~20V
 * @return int
 */
int axk_ch224_set_pps_vout(float PPS_VOUT) {
  // 5. 检查 value 是否在 5~20V 范围内
  if (PPS_VOUT < 5.0 || PPS_VOUT > 28.0) {
    return -10; // 电压值超出范围
  }
  // 1. 发送起始信号
  AXK_CH224_I2C_ACLL(start);
  // 2. 发送设备地址+写命令 (0x22 << 1 | 0 = 0x44)
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  // 3. 等待设备ACK，检查设备是否响应
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -1;                // 设备无响应
  }
  // 4. 发送要写入的PPS电压值
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_REG_PPS);
  // 5. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -4;                // 发送PPS电压值失败
  }
  // 6. 发送PPS电压值
  unsigned char pps_vout = PPS_VOUT * 10.0;
  AXK_CH224_I2C_ACLL(send_byte, pps_vout);
  // 7. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -5;                // 发送PPS电压值失败
  }
  AXK_CH224_I2C_ACLL(stop);
  return 0; // 成功
}
/**
 * @brief 设置CH224 AVS模式输出电压
 *
 * @param AVS_VOUT AVS模式输出电压值，范围5~30V
 * @return int
 */
int axk_ch224_set_avs_vout(float AVS_VOUT) {
  // 1.判断 AVS_VOUT 是否在 5~30V 范围内
  if (AVS_VOUT < 5.0 || AVS_VOUT > 28.0) {
    return -10; // 电压值超出范围
  }
  unsigned int avs_vout = (unsigned char)(AVS_VOUT * 10.0);
  unsigned char AVS_LSB = 0X00;
  unsigned char AVS_MSB = 0X00;
  AVS_MSB = ((avs_vout >> 8) & 0XFF00) | 0X80;
  AVS_LSB = avs_vout & 0X00FF;
  // 2. 发送起始信号
  AXK_CH224_I2C_ACLL(start);
  // 3. 发送设备地址+写命令 (0x22 << 1 | 0 = 0x44)
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD);
  // 4. 等待设备ACK，检查设备是否响应
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -1;                // 设备无响应
  }

  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_REG_AVS_LSB);

  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -3;                // 发送AVS电压值LSB失败
  }
  AXK_CH224_I2C_ACLL(send_byte, AVS_LSB);
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -3;                // 发送AVS电压值MSB失败
  }
  // 5. 发送要写入的AVS电压值
  AXK_CH224_I2C_ACLL(send_byte, AXK_CH224_REG_AVS_MSB);
  // 6. 等待设备ACK
  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -2;                // 发送AVS电压值LSB失败
  }
  AXK_CH224_I2C_ACLL(send_byte, AVS_MSB);

  if (AXK_CH224_I2C_ACLL(wait_ack)) {
    AXK_CH224_I2C_ACLL(stop); // 发送停止信号
    return -4;                // 发送AVS
                              // 电压值LSB失败
  }
  // 13. 发送停止信号
  AXK_CH224_I2C_ACLL(stop);
  return 0; // 成功
}