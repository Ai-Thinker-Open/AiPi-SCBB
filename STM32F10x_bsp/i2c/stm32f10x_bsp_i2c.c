/**
 * @file stm32f10x_bsp_i2c.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "stm32f10x_bsp_i2c.h"
#include "stdio.h"
#include "stm32_hal_legacy.h"
#include "stm32f1xx_hal_gpio.h"
void bsp_i2c_init(void) {

  __HAL_RCC_GPIOB_CLK_ENABLE(); // 注意：需替换为实际端口的时钟使能宏，参考下方说明

  GPIO_InitTypeDef GPIO_InitStructure = {0};

  GPIO_InitStructure.Pin = GPIO_SDA | GPIO_SCL;    // SDA和SCL引脚
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出模式
  GPIO_InitStructure.Pull = GPIO_NOPULL;           // 无上下拉
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速模式（对应50MHz）

  HAL_GPIO_Init(BSP_I2C_PORT, &GPIO_InitStructure); // HAL库GPIO初始化函数
}
/**
 * @brief 发送I2C起始信号
 *
 */
void bsp_i2c_start(void) {
  SDA_OUT();
  SCL(1);
  SDA(0);

  SDA(1);
  delay_us(5);
  SDA(0);
  delay_us(5);

  SCL(0);
}
/**
 * @brief 发送I2C停止信号
 *
 */
void bsp_i2c_stop(void) {
  SDA_OUT();
  SCL(0);
  SDA(0);

  SCL(1);
  delay_us(5);
  SDA(1);
  delay_us(5);
}
/**
 * @brief 发送应答信号
 *
 * @param ack 应答
 */
void bsp_i2c_send_ack(u8 ack) {
  SDA_OUT();
  SCL(0);
  SDA(0);
  delay_us(5);
  SDA(ack); // 发送ACK
  SCL(1);
  delay_us(5);
  SCL(0);
  SDA(1);
}
/**
 * @brief 等待I2C从设备应答
 *
 * @return u8 0: 成功 1: 失败
 */
u8 bsp_i2c_wait_ack(void) {

  char ack = 0;
  unsigned char ack_flag = 10;
  SCL(0);
  SDA(1);
  SDA_IN();

  SCL(1);
  while ((SDA_GET() == 1) && (ack_flag)) {
    ack_flag--;
    delay_us(5);
  }

  if (ack_flag <= 0) {
    bsp_i2c_stop();
    return 1;
  } else {
    SCL(0);
    SDA_OUT();
  }
  return ack;
}
/**
 * @brief 发送I2C字节数据
 *
 * @param _dat 字节数据
 */
void bsp_i2c_send_byte(u8 _dat) {
  u8 i = 0;
  SDA_OUT();
  SCL(0);
  for (i = 0; i < 8; i++) {
    SDA((_dat & 0x80) >> 7);
    delay_us(1);
    SCL(1);
    delay_us(5);
    SCL(0);
    delay_us(5);
    _dat <<= 1;
  }
}
/**
 * @brief   接收I2C字节数据
 *
 * @return u8
 */
u8 bsp_i2c_read_byte(void) {
  unsigned char i, receive = 0;
  SDA_IN(); // SDA设置为输入
  for (i = 0; i < 8; i++) {
    SCL(0);
    delay_us(5);
    SCL(1);
    delay_us(5);
    receive <<= 1;
    if (SDA_GET()) {
      receive |= 1;
    }
    delay_us(5);
  }
  SCL(0);
  return receive;
}
