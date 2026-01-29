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
#ifndef __STM32F10X_BSP_I2C_H__
#define __STM32F10X_BSP_I2C_H__
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
void bsp_i2c_start(void);
void bsp_i2c_stop(void);
void bsp_i2c_send_ack(u8 ack);
u8 bsp_i2c_wait_ack(void);
void bsp_i2c_send_byte(u8 _dat);
u8 bsp_i2c_read_byte(void);
#endif /* __STM32F10X_BSP_I2C_H__ */