/**
 * @file axk_sht3x.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_sht3x.h"
#include <stdint.h>
#include <stdio.h>
/**
 * @brief 初始化SHT3x温湿度传感器
 *
 * 该函数用于初始化SHT3x数字温湿度传感器，通过I2C总线与传感器建立通信连接。
 * 初始化过程遵循I2C总线通信协议，确保传感器能够正常响应。
 *
 * @return uint8_t 初始化结果
 *         - 0: 初始化成功，传感器正常响应
 *         - 1: 初始化失败，传感器无应答或通信异常
 *
 * @note 使用前需确保：
 *       1. I2C相关硬件和时钟已正确配置
 *       2. 传感器电源已稳定供电
 *       3. 硬件连接正确（SCL/SDA引脚）
 *
 * @warning 函数会直接操作I2C总线，确保调用时没有其他设备占用总线
 */
unsigned char axk_sht3x_init(void) {
  AXK_SHT3X_I2C_ACLL(init);  // 调用I2C初始化函数 (展开为bsp_i2c_init())
  AXK_SHT3X_I2C_ACLL(start); // 发送I2C起始信号 (展开为bsp_i2c_start())

  // 发送SHT3x传感器写地址
  // SHT3x默认7位地址为0x44，左移1位后加0表示写操作 (实际发送地址: 0x88)
  AXK_SHT3X_I2C_ACLL(send_byte, AXK_SHT3X_ADDRESS << 1 | AXK_SHT3X_WRITE_CMD);

  // 等待传感器应答信号
  if (AXK_SHT3X_I2C_ACLL(wait_ack)) // 展开为bsp_i2c_wait_ack()
  {
    return 1; // 无应答，返回失败
  }

  return 0; // 有应答，初始化成功
}
/**
 * @brief 设置SHT3x工作模式
 *
 * @param _mode 工作模式，参考axk_sht3x.h中的定义
 * @return unsigned char 操作结果
 *         - 0: 成功
 *         - 1: 失败，I2C无应答
 *         - 2: 失败，I2C发送高字节失败
 *         - 3: 失败，I2C发送低字节失败
 */
unsigned char axk_sht3x_set_mode(unsigned int _mode) {
  AXK_SHT3X_I2C_ACLL(start); // 发送I2C起始信号 (展开为bsp_i2c_start())
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
 * @brief 计算SHT3x校验和
 *
 * @param data 数据指针
 * @param len 数据长度
 * @return unsigned char 校验和
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
 * @brief 读取SHT3x温湿度数据
 *
 * @param _mode 工作模式，参考axk_sht3x.h中的定义
 * @param temp 温度指针，用于存储读取到的温度值（单位：摄氏度）
 * @param humidity 湿度指针，用于存储读取到的湿度值（单位：百分比）
 * @return unsigned char 操作结果
 *         - 0: 成功
 *         - 1: 失败，I2C无应答
 *         - 2: 失败，I2C发送高字节失败
 *         - 3: 失败，I2C发送低字节失败
 *         - 4: 失败，I2C读取超时
 *         - 5: 失败，校验和错误
 *         - 6: 失败，参数指针为空
 */
unsigned char axk_sht3x_read(unsigned int _mode, double *temp,
                             double *humidity) {
  if (temp == NULL || humidity == NULL)
    return 6;
  unsigned int i = 0;
  static unsigned char buff[6] = {0};
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
    if (i > 20)
      return 4;
    AXK_SHT3X_DELAY_MS(10);

    AXK_SHT3X_I2C_ACLL(start);
    AXK_SHT3X_I2C_ACLL(send_byte, AXK_SHT3X_ADDRESS << 1 | AXK_SHT3X_READ_CMD);
  } while (AXK_SHT3X_I2C_ACLL(wait_ack));

  buff[0] = AXK_SHT3X_I2C_ACLL(read_byte); // 展开为bsp_i2c_read_byte()
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  buff[1] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  buff[2] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  buff[3] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  buff[4] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_ACK);
  buff[5] = AXK_SHT3X_I2C_ACLL(read_byte);
  AXK_SHT3X_I2C_ACLL(send_ack, AXK_SHT3X_NACK);
  AXK_SHT3X_I2C_ACLL(stop);
  if (axk_sht3x_crc(buff, 2) == buff[2] &&
      (axk_sht3x_crc(buff + 3, 2) == buff[5])) {
    data_16 = (buff[0] << 8) | buff[1];
    *temp = -45 + 175 * ((double)data_16 / 65535.0);
    data_16 = (buff[3] << 8) | buff[4];
    *humidity = 100 * ((double)data_16 / 65535.0);
    return 0;
  } else {
    return 5;
  }
  return 0;
}
