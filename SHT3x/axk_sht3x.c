/**
 * @file axk_sht3x.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief SHT3x 温湿度传感器驱动实现（I2C）。
 * @version 0.1
 * @date 2026-01-29
 * @copyright Copyright (c) 2026
 */
#include <stdint.h>
#include <stdio.h>

#include "axk_sht3x.h"

#ifdef SCBB_SHT3X_ENABLED

/**
 * @brief 在 I2C 总线上探测 SHT3x 传感器
 *
 * @return unsigned char  0: 传感器响应正常，1: 传感器无应答
 */
unsigned char axk_sht3x_init(void) {
  AXK_SHT3X_I2C_ACLL(init);
  AXK_SHT3X_I2C_ACLL(start);
  AXK_SHT3X_I2C_ACLL(send_byte, AXK_SHT3X_ADDRESS << 1 | AXK_SHT3X_WRITE_CMD);
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 1;
  }
  return 0;
}

/**
 * @brief 向 SHT3x 写入 2 字节测量指令
 *
 * @param[in]  _mode          16 位测量指令码
 * @return     unsigned char  操作状态
 *              - 0: 写入成功
 *              - 1: 设备地址无应答
 *              - 2: 高字节无应答
 *              - 3: 低字节无应答
 */
unsigned char axk_sht3x_set_mode(unsigned int _mode) {
  AXK_SHT3X_I2C_ACLL(start);
  AXK_SHT3X_I2C_ACLL(send_byte, AXK_SHT3X_ADDRESS << 1 | AXK_SHT3X_WRITE_CMD);
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 1;
  }
  AXK_SHT3X_I2C_ACLL(send_byte, (_mode >> 8));
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 2;
  }
  AXK_SHT3X_I2C_ACLL(send_byte, (_mode & 0xFF));
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 3;
  }
  return 0;
}

/**
 * @brief CRC-8 校验计算，多项式 0x31（参照 SHT3x 数据手册）
 *
 * @param[in]  data           数据缓冲区指针
 * @param[in]  len            数据长度（字节）
 * @return     unsigned char  计算得到的 CRC-8 校验值
 */
static unsigned char axk_sht3x_crc(const unsigned char *data,
                                   unsigned char len) {
  const unsigned char POLYNOMIAL = 0x31;
  unsigned char crc = 0xFF;
  int j, i;

  for (j = 0; j < len; j++) {
    crc ^= *data++;
    for (i = 0; i < 8; i++) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

/**
 * @brief 发送测量指令并读取 SHT3x 温湿度数据
 *
 * @param[in]  _mode          测量指令码
 * @param[out] *temp          温度输出指针（摄氏度），不可为 NULL
 * @param[out] *humidity      湿度输出指针（%RH），不可为 NULL
 * @return     unsigned char  操作状态
 *              - 0: 读取并校验成功
 *              - 1: 设备地址无 I2C ACK
 *              - 2: 指令高字节无 ACK
 *              - 3: 指令低字节无 ACK
 *              - 4: 等待转换超时（传感器未就绪）
 *              - 5: CRC 校验失败（数据完整性错误）
 *              - 6: temp 或 humidity 为空指针
 */
unsigned char axk_sht3x_read(unsigned int _mode, double *temp,
                             double *humidity) {
  if (temp == NULL || humidity == NULL) {
    return 6;
  }
  unsigned int i = 0;
  static unsigned char s_axk_sht3x_buff[6] = {0};
  unsigned int data_16 = 0;

  AXK_SHT3X_I2C_ACLL(start);
  AXK_SHT3X_I2C_ACLL(send_byte, AXK_SHT3X_ADDRESS << 1 | AXK_SHT3X_WRITE_CMD);
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 1;
  }
  AXK_SHT3X_I2C_ACLL(send_byte, (_mode >> 8));
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 2;
  }
  AXK_SHT3X_I2C_ACLL(send_byte, (_mode & 0xFF));
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) {
    return 3;
  }

  do {
    i++;
    if (i > 20) {
      return 4;
    }
    AXK_SHT3X_DELAY_MS(10);
    AXK_SHT3X_I2C_ACLL(start);
    AXK_SHT3X_I2C_ACLL(send_byte, AXK_SHT3X_ADDRESS << 1 | AXK_SHT3X_READ_CMD);
  } while (AXK_SHT3X_I2C_ACLL(wait_ack));

  s_axk_sht3x_buff[0] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  s_axk_sht3x_buff[1] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  s_axk_sht3x_buff[2] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  s_axk_sht3x_buff[3] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  s_axk_sht3x_buff[4] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  s_axk_sht3x_buff[5] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_NACK);
  AXK_SHT3X_I2C_ACLL(stop);

  if (axk_sht3x_crc(s_axk_sht3x_buff, 2) == s_axk_sht3x_buff[2] &&
      (axk_sht3x_crc(s_axk_sht3x_buff + 3, 2) == s_axk_sht3x_buff[5])) {
    data_16 = (s_axk_sht3x_buff[0] << 8) | s_axk_sht3x_buff[1];
    *temp = -45 + 175 * ((double)data_16 / 65535.0);
    data_16 = (s_axk_sht3x_buff[3] << 8) | s_axk_sht3x_buff[4];
    *humidity = 100 * ((double)data_16 / 65535.0);
    return 0;
  } else {
    return 5;
  }
}

#endif /* SCBB_SHT3X_ENABLED */
