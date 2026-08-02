/**
 * @file stm32f10x_bsp_i2c.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-29
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef STM32F10X_BSP_I2C_H
#define STM32F10X_BSP_I2C_H
#include "stm32f10x_delay.h"
#include "log.h"
#define u8 unsigned char
#define delay_us(x) HAL_Delay_us(x)
#define delay_ms(x) HAL_Delay(x)
// 端口移植
#define BSP_I2C_RCC RCC_APB2Periph_GPIOB
#define BSP_I2C_PORT GPIOB

#define GPIO_SDA GPIO_PIN_6
#define GPIO_SCL GPIO_PIN_7

#define BSP_I2C_WRITE 0x00
#define BSP_I2C_READ 0x01
// SDA切换为输入模式（浮空输入）
#define SDA_IN()                                                               \
  {                                                                            \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                    \
    GPIO_InitStruct.Pin = GPIO_SDA;                                            \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; /* 浮空输入模式 */                 \
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                        \
    HAL_GPIO_Init(BSP_I2C_PORT, &GPIO_InitStruct);                             \
  }

// SDA切换为输出模式（推挽输出）
#define SDA_OUT()                                                              \
  {                                                                            \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                    \
    GPIO_InitStruct.Pin = GPIO_SDA;                                            \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   /* 推挽输出模式 */           \
    GPIO_InitStruct.Pull = GPIO_NOPULL;           /* 无上下拉 */               \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速模式（对应50MHz） */  \
    HAL_GPIO_Init(BSP_I2C_PORT, &GPIO_InitStruct);                             \
  }
// 获取SDA引脚的电平变化
#define SDA_GET() HAL_GPIO_ReadPin(BSP_I2C_PORT, GPIO_SDA)

// SDA输出电平控制（x为真时输出高电平，假时输出低电平）
#define SDA(x)                                                                 \
  HAL_GPIO_WritePin(BSP_I2C_PORT, GPIO_SDA, (x ? GPIO_PIN_SET : GPIO_PIN_RESET))

// SCL输出电平控制（x为真时输出高电平，假时输出低电平）
#define SCL(x)                                                                 \
  HAL_GPIO_WritePin(BSP_I2C_PORT, GPIO_SCL, (x ? GPIO_PIN_SET : GPIO_PIN_RESET))

void bsp_i2c_init(void);

/**
 * @brief 发送 I2C 起始信号
 */
void bsp_i2c_start(void);

/**
 * @brief 发送 I2C 停止信号
 */
void bsp_i2c_stop(void);

/**
 * @brief 发送应答信号
 *
 * @param[in]  ack  应答电平（0: ACK，1: NACK）
 */
void bsp_i2c_send_ack(u8 ack);

/**
 * @brief 等待从设备应答
 *
 * @return u8  应答结果
 *              - 0: 收到 ACK
 *              - 1: 未收到 ACK（超时）
 */
u8 bsp_i2c_wait_ack(void);

/**
 * @brief 发送一个字节
 *
 * @param[in]  _dat  待发送的字节
 */
void bsp_i2c_send_byte(u8 _dat);

/**
 * @brief 接收一个字节
 *
 * @return u8  接收到的字节
 */
u8 bsp_i2c_read_byte(void);
#endif /* STM32F10X_BSP_I2C_H */
