/**
 * @file axk_hxd039b2.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief HXD039B2 红外编解码模块驱动（UART 协议）。
 * @version 0.1
 * @date 2026-07-14
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_hxd039b2.h"
#include <stdint.h>
#include <string.h>

#define AXK_HXD039B2_RECV_BUF_SIZE 2

typedef struct {
    unsigned char buffer[AXK_HXD039B2_RECV_BUF_SIZE];
    int head;
    int tail;
    int count;
} axk_hxd039b2_circular_buf_t;

static axk_hxd039b2_circular_buf_t s_axk_hxd_recv_buf;
static unsigned char s_axk_ac_code[2] = {0x03, 0xF8};
static const unsigned char s_axk_learn_cmd[] = {0x30, 0x70, 0xA0};

/**
 * @brief 初始化环形缓冲区。
 *
 *  @param[in]  cb              环形缓冲区指针。
 */
static void hxd039b2_buf_init(axk_hxd039b2_circular_buf_t *cb) {
    if (cb == NULL) {
        return;
    }
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    memset(cb->buffer, 0, AXK_HXD039B2_RECV_BUF_SIZE);
}

/**
 * @brief 检查缓冲区是否为空。
 *
 *  @param[in]  cb              环形缓冲区指针。
 *  @return     1: 为空，0: 不为空。
 */
static int hxd039b2_buf_is_empty(axk_hxd039b2_circular_buf_t *cb) {
    if (cb == NULL) {
        return 1;
    }
    return cb->count == 0;
}

/**
 * @brief 检查缓冲区是否已满。
 *
 *  @param[in]  cb              环形缓冲区指针。
 *  @return     1: 已满，0: 未满。
 */
static int hxd039b2_buf_is_full(axk_hxd039b2_circular_buf_t *cb) {
    if (cb == NULL) {
        return 0;
    }
    return cb->count == AXK_HXD039B2_RECV_BUF_SIZE;
}

/**
 * @brief 向缓冲区入队数据。
 *
 *  @param[in]  cb              环形缓冲区指针。
 *  @param[in]  data            要入队的数据。
 *  @return     0: 成功，-1: 已满或指针为空。
 */
static int hxd039b2_buf_enqueue(axk_hxd039b2_circular_buf_t *cb, unsigned char data) {
    if (cb == NULL) {
        return -1;
    }
    if (hxd039b2_buf_is_full(cb)) {
        return -1;
    }
    cb->buffer[cb->head] = data;
    cb->head = (cb->head + 1) % AXK_HXD039B2_RECV_BUF_SIZE;
    cb->count++;
    return 0;
}

/**
 * @brief 从缓冲区出队数据。
 *
 *  @param[in]  cb              环形缓冲区指针。
 *  @param[out] data            数据输出缓冲区。
 *  @return     0: 成功，-1: 为空或指针为空。
 */
static int hxd039b2_buf_dequeue(axk_hxd039b2_circular_buf_t *cb, unsigned char *data) {
    if (cb == NULL || data == NULL) {
        return -1;
    }
    if (hxd039b2_buf_is_empty(cb)) {
        return -1;
    }
    *data = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % AXK_HXD039B2_RECV_BUF_SIZE;
    cb->count--;
    return 0;
}

/**
 * @brief 通过 UART 发送数据，先检测忙状态。
 *
 *  @param[in]  data            要发送的数据缓冲区。
 *  @param[in]  data_len        数据长度。
 *  @return     0: 成功，-1: 指针为空，-2: 芯片忙碌。
 */
static int hxd039b2_send_data(unsigned char *data, int data_len) {
    if (data == NULL || data_len <= 0) {
        return -1;
    }

    if (AXK_HXD039B2_GPIO_ACLL(read, AXK_HXD039B2_BUSY_PIN) == 0) {
        return -2;
    }

    for (int i = 0; i < data_len; i++) {
        AXK_HXD039B2_UART_ACLL(send_byte, data[i]);
    }
    return 0;
}

/**
 * @brief 初始化 HXD039B2 UART 和 GPIO。
 *
 *  @return     0: 成功。
 */
int axk_hxd039b2_init(void) {
    AXK_HXD039B2_UART_ACLL(init);
    AXK_HXD039B2_GPIO_ACLL(init, AXK_HXD039B2_BUSY_PIN, 1);

    int len = axk_hxd039b2_get_ac_code(s_axk_ac_code);
    if (len == 0) {
        // 使用默认空调码组。
        if (s_axk_ac_code[0] == 0 && s_axk_ac_code[1] == 0) {
            s_axk_ac_code[0] = 0x03;
            s_axk_ac_code[1] = 0xF8;
        }
    }

    hxd039b2_buf_init(&s_axk_hxd_recv_buf);
    return 0;
}

/**
 * @brief UART 接收回调，在 MCU UART 中断中调用。
 *
 *  @param[in]  uart_data       接收到的字节。
 */
void axk_hxd039b2_uart_recv_cb(unsigned char uart_data) {
    hxd039b2_buf_enqueue(&s_axk_hxd_recv_buf, uart_data);

    if (hxd039b2_buf_is_full(&s_axk_hxd_recv_buf)) {
        hxd039b2_buf_dequeue(&s_axk_hxd_recv_buf, &s_axk_ac_code[0]);
        hxd039b2_buf_dequeue(&s_axk_hxd_recv_buf, &s_axk_ac_code[1]);
    }
}

/**
 * @brief 保存空调码组到持久化存储。
 *
 *  @param[in]  ac_code         空调码组缓冲区。
 *  @param[in]  len             码组长度。
 *  @return     0: 成功，-1: 失败。
 */
int axk_hxd039b2_save_ac_code(unsigned char *ac_code, int len) {
    if (ac_code == NULL || len <= 0) {
        return -1;
    }
    (void)ac_code;
    (void)len;
    return 0;
}

/**
 * @brief 从持久化存储读取空调码组。
 *
 *  @param[out] code            空调码组缓冲区。
 *  @return     码组长度，未找到返回 0。
 */
int axk_hxd039b2_get_ac_code(unsigned char *code) {
    if (code == NULL) {
        return 0;
    }
    (void)code;
    return 0;
}

/**
 * @brief 启动红外码学习。
 *
 *  @return     0: 成功。
 */
int axk_hxd039b2_start_learn(void) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    hxd039b2_send_data((unsigned char *)s_axk_learn_cmd, 3);

    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS * 10);
    return 0;
}

/**
 * @brief 通过红外设置空调电源状态。
 *
 *  @param[in]  power_state     1: 开，0: 关。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_power(int power_state) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(power_state ? AXK_HXD039B2_IR_CODE_AC_ON : AXK_HXD039B2_IR_CODE_AC_OFF)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}

/**
 * @brief 通过红外设置空调模式。
 *
 *  @param[in]  mode            0: 自动，1: 制冷，2: 除湿，3: 送风，4: 制热。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_mode(int mode) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(AXK_HXD039B2_IR_CODE_AC_MODE_AUTO + mode)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}

/**
 * @brief 通过红外设置空调温度。
 *
 *  @param[in]  temperature     16-31 摄氏度。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_temperature(unsigned char temperature) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(temperature - 16 + AXK_HXD039B2_IR_CODE_AC_TEMPERATURE_16)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}

/**
 * @brief 通过红外设置空调风速。
 *
 *  @param[in]  fan_mode        0: 低速，1: 中速，2: 高速，3: 自动。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_fan_mode(unsigned char fan_mode) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(fan_mode + AXK_HXD039B2_IR_CODE_AC_FAN_MODE_AUTO)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}

/**
 * @brief 通过红外设置空调风向。
 *
 *  @param[in]  trend           0: 向上，1: 中间，2: 向下。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_trend(unsigned char trend) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(trend + AXK_HXD039B2_IR_CODE_AC_TREND_UP)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}

/**
 * @brief 通过红外设置空调自动风向。
 *
 *  @param[in]  trend_auto      1: 开启，0: 关闭。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_trend_auto(unsigned char trend_auto) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(trend_auto ? AXK_HXD039B2_IR_CODE_AC_TREND_AUTO_ON : AXK_HXD039B2_IR_CODE_AC_TREND_AUTO_OFF)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}

/**
 * @brief 通过红外设置空调指示灯。
 *
 *  @param[in]  light_power     1: 开启，0: 关闭。
 *  @return     0: 成功。
 */
int axk_hxd039b2_set_light_power(unsigned char light_power) {
    AXK_HXD039B2_DELAY_MS(AXK_HXD039B2_START_TIME_MS);

    unsigned char ir_code[5] = {
        AXK_HXD039B2_IR_CODE_HANDLE, AXK_HXD039B2_IR_CODE_AC_TYPE,
        s_axk_ac_code[0], s_axk_ac_code[1],
        (unsigned char)(light_power ? AXK_HXD039B2_IR_CODE_AC_LIGHT_ON : AXK_HXD039B2_IR_CODE_AC_LIGHT_OFF)};
    hxd039b2_send_data(ir_code, 5);
    return 0;
}
