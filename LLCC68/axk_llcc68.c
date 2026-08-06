/**
 * @file axk_llcc68.c
 * @brief LLCC68 LoRa 射频收发模块驱动实现(纯 ACLL 跨平台,两文件合并版)。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-08-05
 *
 * @copyright Copyright (c) 2026
 *
 * 驱动逻辑为 Semtech LoRaMac-node 官方驱动(radio.c + llcc68.c)原样封装,
 * 仅将板级硬件原语(SPI/NSS/BUSY/复位/延时)通过 ACLL 宏抽象到 BSP,
 * 芯片命令层与状态机逻辑与官方驱动完全一致。
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "axk_llcc68.h"

/*! DIO1 中断回调(由用户在 EXTI 中断中调用 axk_llcc68_irq_cb 触发) */
static void (*s_axk_dio_irq)(void *context);

/**
 * @brief 等待芯片 BUSY 引脚拉低(带超时打印,防止死等)
 *
 * @return     void  无返回值
 * @note       每次 SPI 操作前必须调用;若 BUSY 长时间为高,
 *             每隔约 1 秒打印一次超时提示并继续等待
 */
void LLCC68WaitOnBusy(void)
{
    uint32_t cnt = 0;

    while (AXK_LLCC68_GPIO_ACLL(read, AXK_LLCC68_BUSY_PIN) == 1) {
        if ((++cnt) > 1000) {
            printf("wait busy pin timeout\r\n");
            cnt = 0;
        }
        AXK_LLCC68_DELAY_MS(1);
    }
}

/**
 * @brief 控制 NSS 片选引脚电平
 *
 * @param[in]  lev  电平:1 拉高(释放片选),0 拉低(选中芯片)
 * @return     void 无返回值
 */
void LLCC68SetNss(uint8_t lev)
{
    if (lev) {
        AXK_LLCC68_GPIO_ACLL(set, AXK_LLCC68_NSS_PIN);
    } else {
        AXK_LLCC68_GPIO_ACLL(reset, AXK_LLCC68_NSS_PIN);
    }
}

/**
 * @brief SPI 全双工交换一个字节
 *
 * @param[in]  data  待发送字节
 * @return     uint8_t  接收到的字节
 */
uint8_t LLCC68SpiInOut(uint8_t data)
{
    return AXK_LLCC68_SPI_ACLL(transfer8, AXK_LLCC68_SPI_NAME, data);
}

/**
 * @brief 毫秒级延时
 *
 * @param[in]  ms  延时毫秒数
 * @return     void 无返回值
 */
void LLCC68DelayMs(uint32_t ms)
{
    AXK_LLCC68_DELAY_MS(ms);
}

/**
 * @brief 射频频率合法性检查
 *
 * @param[in]  frequency  频率值(Hz)
 * @return     bool  恒为 true(全部频率均支持)
 */
bool LLCC68CheckRfFrequency(uint32_t frequency)
{
    (void)frequency;
    return true;
}

/**
 * @brief 初始化 GPIO 与 SPI(板级原语,ACLL 抽象)
 *
 * @return     void 无返回值
 * @note       初始化 NSS/复位为推挽输出,BUSY 为上拉输入,
 *             DIO1 为浮空输入;SPI 工作于模式 0
 */
void LLCC68IoInit(void)
{
    bsp_spi_cfg_t spi_cfg;

    /* NSS/复位:推挽输出;BUSY:上拉输入;DIO1:浮空输入 */
    AXK_LLCC68_GPIO_ACLL(init, AXK_LLCC68_NSS_PIN, 1);
    AXK_LLCC68_GPIO_ACLL(set, AXK_LLCC68_NSS_PIN);
    AXK_LLCC68_GPIO_ACLL(init, AXK_LLCC68_NRESET_PIN, 1);
    AXK_LLCC68_GPIO_ACLL(init, AXK_LLCC68_BUSY_PIN, 4);
    AXK_LLCC68_GPIO_ACLL(init, AXK_LLCC68_DIO1_PIN, 0);

    /* SPI 模式 0:CPOL=0, CPHA=0 */
    spi_cfg.freq = 8000000;
    spi_cfg.mode = 0;
    spi_cfg.data_width = 8;
    if (AXK_LLCC68_SPI_ACLL(init, AXK_LLCC68_SPI_NAME, &spi_cfg) != 0) {
        printf("spi1 init error\r\n");
        return;
    }
    printf("LLCC68IoInit init sucess\r\n");
}

/**
 * @brief DIO1 中断初始化(仅记录回调,EXTI 由用户自行配置)
 *
 * @param[in]  dioIrq  DIO1 中断回调函数指针
 * @return     void    无返回值
 * @note       SCBB 模式不直接配置 MCU 外部中断,用户需在
 *             自己的 EXTI 中断服务函数中调用 axk_llcc68_irq_cb()
 */
void LLCC68IoIrqInit(DioIrqHandler dioIrq)
{
    s_axk_dio_irq = dioIrq;
}

/**
 * @brief 复位芯片:RESET 拉低 20ms 后释放
 *
 * @return     void 无返回值
 * @note       复位后芯片进入默认状态,需重新初始化配置
 */
void LLCC68ResetInit(void)
{
    AXK_LLCC68_DELAY_MS(10);
    AXK_LLCC68_GPIO_ACLL(reset, AXK_LLCC68_NRESET_PIN);
    AXK_LLCC68_DELAY_MS(20);
    AXK_LLCC68_GPIO_ACLL(set, AXK_LLCC68_NRESET_PIN);
    AXK_LLCC68_DELAY_MS(10);
}

/* ================== 定时器原语(SCBB 模式置空,超时由芯片 IRQ 处理) ================== */

/**
 * @brief 定时器初始化(SCBB 模式空实现)
 *
 * @return void 无返回值
 */
void LLCC68TimerInit(void) { }

/**
 * @brief 设置发送定时器值(SCBB 模式空实现)
 *
 * @param[in]  value  定时器值(ms)
 * @return     void   无返回值
 */
void LLCC68SetTxTimerValue(uint32_t value) { (void)value; }

/**
 * @brief 启动发送定时器(SCBB 模式空实现)
 *
 * @return void 无返回值
 */
void LLCC68TxTimerStart(void) { }

/**
 * @brief 停止发送定时器(SCBB 模式空实现)
 *
 * @return void 无返回值
 */
void LLCC68TxTimerStop(void) { }

/**
 * @brief 设置接收定时器值(SCBB 模式空实现)
 *
 * @param[in]  value  定时器值(ms)
 * @return     void   无返回值
 */
void LLCC68SetRxTimerValue(uint32_t value) { (void)value; }

/**
 * @brief 启动接收定时器(SCBB 模式空实现)
 *
 * @return void 无返回值
 */
void LLCC68RxTimerStart(void) { }

/**
 * @brief 停止接收定时器(SCBB 模式空实现)
 *
 * @return void 无返回值
 */
void LLCC68RxTimerStop(void) { }

/* ================== SCBB 公共 API ================== */

/**
 * @brief 初始化 LLCC68(引脚/SPI/复位/默认收发参数)
 *
 * @param[in]  events  事件回调结构指针,可为 NULL(忽略事件)
 * @return     int     操作状态
 *              - 0: 初始化成功
 * @note       初始化完成即按 axk_llcc68.h 顶部默认参数配置好
 *             频率与收发参数,可直接调用 send/start_rx
 */
int axk_llcc68_init(const axk_llcc68_events_t *events)
{
    Radio.Init((RadioEvents_t *)events);
    Radio.SetChannel(AXK_LLCC68_FREQUENCY_HZ);
    Radio.SetTxConfig(MODEM_LORA, AXK_LLCC68_TX_POWER_DBM, 0, AXK_LLCC68_BANDWIDTH,
                      AXK_LLCC68_SPREADING_FACTOR, AXK_LLCC68_CODING_RATE,
                      AXK_LLCC68_PREAMBLE_LENGTH, AXK_LLCC68_FIX_LEN,
                      AXK_LLCC68_CRC_ON, 0, 0, AXK_LLCC68_IQ_INVERTED, 3000);
    return 0;
}

/**
 * @brief 设置射频收发频率
 *
 * @param[in]  freq_hz  频率值(Hz),如 470500000UL
 * @return     int      操作状态
 *              - 0: 设置成功
 */
int axk_llcc68_set_frequency(uint32_t freq_hz)
{
    Radio.SetChannel(freq_hz);
    return 0;
}

/**
 * @brief 非阻塞发送数据(结果通过 TxDone/TxTimeout 回调返回)
 *
 * @param[in]  payload  待发送数据缓冲区
 * @param[in]  size     数据长度(字节)
 * @return     int      操作状态
 *              - 0: 发送已启动
 *              - -1: 参数无效(payload 为空或 size 为 0)
 * @note       发送期间请勿再次调用本函数;发送完成由 DIO1 中断触发
 */
int axk_llcc68_send(const uint8_t *payload, uint8_t size)
{
    if (payload == NULL || size == 0) {
        return -1;
    }
    Radio.Send((uint8_t *)payload, size);
    return 0;
}

/**
 * @brief 进入接收模式(结果通过 RxDone/RxTimeout/RxError 回调返回)
 *
 * @param[in]  timeout_ms  接收超时时间(ms),0 表示连续接收
 * @return     int         操作状态
 *              - 0: 接收已启动
 * @note       超时/收到数据后需在回调中重新调用本函数继续接收
 */
int axk_llcc68_start_rx(uint32_t timeout_ms)
{
    uint32_t timeout;

    Radio.SetRxConfig(MODEM_LORA, AXK_LLCC68_BANDWIDTH,
                      AXK_LLCC68_SPREADING_FACTOR, AXK_LLCC68_CODING_RATE,
                      0, AXK_LLCC68_PREAMBLE_LENGTH,
                      AXK_LLCC68_SYMB_TIMEOUT, AXK_LLCC68_FIX_LEN,
                      255, AXK_LLCC68_CRC_ON, 0, 0, AXK_LLCC68_IQ_INVERTED,
                      (timeout_ms == 0) ? true : false);
    Radio.SetChannel(AXK_LLCC68_FREQUENCY_HZ);

    /* 换算为芯片时基:1 时基 = 15.625us,即 1ms = 64 时基 */
    if (timeout_ms == 0) {
        timeout = 0; /* 连续接收 */
    } else {
        timeout = (uint32_t)((uint64_t)timeout_ms * 64);
    }
    Radio.Rx(timeout);
    return 0;
}

/**
 * @brief 读取瞬时 RSSI(接收链路信号强度)
 *
 * @return     int   RSSI 值(dBm)
 */
int axk_llcc68_get_rssi(void)
{
    return Radio.Rssi(MODEM_LORA);
}

/**
 * @brief DIO1 中断回调处理函数,在 MCU 的 DIO1 外部中断服务函数中调用
 *
 * @return     void 无返回值
 * @note       内部读取并清除芯片中断状态,分发到对应事件回调;
 *             此函数应在中断上下文执行,内部无阻塞等待
 */
void axk_llcc68_irq_cb(void)
{
    if (s_axk_dio_irq != NULL) {
        s_axk_dio_irq(NULL); /* 触发 RadioOnDioIrq,设置 IrqFired */
    }
    RadioIrqProcess(); /* 处理 IRQ 并分发事件回调 */
}

/* ================== Semtech 官方驱动实现(原样) ================== */

/*!
 * \file      LLCC68.c
 *
 * \brief     LLCC68 driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */

/*!
 * \brief Internal frequency of the radio
 */
#define LLCC68_XTAL_FREQ                            32000000UL

/*!
 * \brief Scaling factor used to perform fixed-point operations
 */
#define LLCC68_PLL_STEP_SHIFT_AMOUNT                ( 14 )

/*!
 * \brief PLL step - scaled with LLCC68_PLL_STEP_SHIFT_AMOUNT
 */
#define LLCC68_PLL_STEP_SCALED                      ( LLCC68_XTAL_FREQ >> ( 25 - LLCC68_PLL_STEP_SHIFT_AMOUNT ) )

/*!
 * \brief Maximum value for parameter symbNum in \ref LLCC68SetLoRaSymbNumTimeout
 */
#define LLCC68_MAX_LORA_SYMB_NUM_TIMEOUT            248

/*!
 * \brief Radio registers definition
 */
typedef struct
{
    uint16_t      Addr;                             //!< The address of the register
    uint8_t       Value;                            //!< The value of the register
}RadioRegisters_t;

/*!
 * \brief Stores the current packet type set in the radio
 */
static RadioPacketTypes_t PacketType;

/*!
 * \brief Stores the current packet header type set in the radio
 */
static volatile RadioLoRaPacketLengthsMode_t LoRaHeaderType;

/*!
 * \brief Stores the last frequency error measured on LoRa received packet
 */
volatile uint32_t FrequencyError = 0;

/*!
 * \brief Hold the status of the Image calibration
 */
static bool ImageCalibrated = false;

/*!
 * \brief Get the number of PLL steps for a given frequency in Hertz
 *
 * \param [in] freqInHz Frequency in Hertz
 *
 * \returns Number of PLL steps
 */
static uint32_t LLCC68ConvertFreqInHzToPllStep( uint32_t freqInHz );

/*
 * LLCC68 DIO IRQ callback functions prototype
 */

/*!
 * \brief DIO 0 IRQ callback
 */
void LLCC68OnDioIrq( void );

/*!
 * \brief DIO 0 IRQ callback
 */
void LLCC68SetPollingMode( void );

/*!
 * \brief DIO 0 IRQ callback
 */
void LLCC68SetInterruptMode( void );

/*
 * \brief Process the IRQ if handled by the driver
 */
void LLCC68ProcessIrqs( void );

void LLCC68Init( DioIrqHandler dioIrq )
{	
	  LLCC68ResetInit( );

    LLCC68IoIrqInit( dioIrq );

    LLCC68Wakeup( );
    LLCC68SetStandby( STDBY_RC );
    
		// Initialize TCXO control
    LLCC68IoTcxoInit( );

    // Initialize RF switch control
    LLCC68IoRfSwitchInit( );

    LLCC68SetOperatingMode( MODE_STDBY_RC );
}

void LLCC68CheckDeviceReady( void )
{
    if( ( LLCC68GetOperatingMode( ) == MODE_SLEEP ) || ( LLCC68GetOperatingMode( ) == MODE_RX_DC ) )
    {
        LLCC68Wakeup( );
        // Switch is turned off when device is in sleep mode and turned on is all other modes
        LLCC68AntSwOn( );
    }
    LLCC68WaitOnBusy( );
}

void LLCC68SetPayload( uint8_t *payload, uint8_t size )
{
    LLCC68WriteBuffer( 0x00, payload, size );
}

uint8_t LLCC68GetPayload( uint8_t *buffer, uint8_t *size,  uint8_t maxSize )
{
    uint8_t offset = 0;

    LLCC68GetRxBufferStatus( size, &offset );
    if( *size > maxSize )
    {
        return 1;
    }
    LLCC68ReadBuffer( offset, buffer, *size );
    return 0;
}

void LLCC68SendPayload( uint8_t *payload, uint8_t size, uint32_t timeout )
{
    LLCC68SetPayload( payload, size );
    LLCC68SetTx( timeout );
}

uint8_t LLCC68SetSyncWord( uint8_t *syncWord )
{
    LLCC68WriteRegisters( REG_LR_SYNCWORDBASEADDRESS, syncWord, 8 );
    return 0;
}

void LLCC68SetCrcSeed( uint16_t seed )
{
    uint8_t buf[2];

    buf[0] = ( uint8_t )( ( seed >> 8 ) & 0xFF );
    buf[1] = ( uint8_t )( seed & 0xFF );

    switch( LLCC68GetPacketType( ) )
    {
        case PACKET_TYPE_GFSK:
            LLCC68WriteRegisters( REG_LR_CRCSEEDBASEADDR, buf, 2 );
            break;

        default:
            break;
    }
}

void LLCC68SetCrcPolynomial( uint16_t polynomial )
{
    uint8_t buf[2];

    buf[0] = ( uint8_t )( ( polynomial >> 8 ) & 0xFF );
    buf[1] = ( uint8_t )( polynomial & 0xFF );

    switch( LLCC68GetPacketType( ) )
    {
        case PACKET_TYPE_GFSK:
            LLCC68WriteRegisters( REG_LR_CRCPOLYBASEADDR, buf, 2 );
            break;

        default:
            break;
    }
}

void LLCC68SetWhiteningSeed( uint16_t seed )
{
    uint8_t regValue = 0;
    
    switch( LLCC68GetPacketType( ) )
    {
        case PACKET_TYPE_GFSK:
            regValue = LLCC68ReadRegister( REG_LR_WHITSEEDBASEADDR_MSB ) & 0xFE;
            regValue = ( ( seed >> 8 ) & 0x01 ) | regValue;
            LLCC68WriteRegister( REG_LR_WHITSEEDBASEADDR_MSB, regValue ); // only 1 bit.
            LLCC68WriteRegister( REG_LR_WHITSEEDBASEADDR_LSB, ( uint8_t )seed );
            break;

        default:
            break;
    }
}

uint32_t LLCC68GetRandom( void )
{
    uint32_t number = 0;
    uint8_t regAnaLna = 0;
    uint8_t regAnaMixer = 0;

    regAnaLna = LLCC68ReadRegister( REG_ANA_LNA );
    LLCC68WriteRegister( REG_ANA_LNA, regAnaLna & ~( 1 << 0 ) );

    regAnaMixer = LLCC68ReadRegister( REG_ANA_MIXER );
    LLCC68WriteRegister( REG_ANA_MIXER, regAnaMixer & ~( 1 << 7 ) );

    // Set radio in continuous reception
    LLCC68SetRx( 0xFFFFFF ); // Rx Continuous

    LLCC68ReadRegisters( RANDOM_NUMBER_GENERATORBASEADDR, ( uint8_t* )&number, 4 );

    LLCC68SetStandby( STDBY_RC );

    LLCC68WriteRegister( REG_ANA_LNA, regAnaLna );
    LLCC68WriteRegister( REG_ANA_MIXER, regAnaMixer );

    return number;
}

void LLCC68SetSleep( SleepParams_t sleepConfig )
{
    uint8_t value=0;

    LLCC68AntSwOff( );

    value = ( ( ( uint8_t )sleepConfig.Fields.WarmStart << 2 ) |
                      ( ( uint8_t )sleepConfig.Fields.Reset << 1 ) |
                      ( ( uint8_t )sleepConfig.Fields.WakeUpRTC ) );
    LLCC68WriteCommand( RADIO_SET_SLEEP, &value, 1 );
    LLCC68SetOperatingMode( MODE_SLEEP );
}

void LLCC68SetStandby( RadioStandbyModes_t standbyConfig )
{
    LLCC68WriteCommand( RADIO_SET_STANDBY, ( uint8_t* )&standbyConfig, 1 );
    if( standbyConfig == STDBY_RC )
    {
        LLCC68SetOperatingMode( MODE_STDBY_RC );
    }
    else
    {
        LLCC68SetOperatingMode( MODE_STDBY_XOSC );
    }
}

void LLCC68SetFs( void )
{
    LLCC68WriteCommand( RADIO_SET_FS, 0, 0 );
    LLCC68SetOperatingMode( MODE_FS );
}

void LLCC68SetTx( uint32_t timeout )
{
    uint8_t buf[3];

    LLCC68SetOperatingMode( MODE_TX );

    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );
    LLCC68WriteCommand( RADIO_SET_TX, buf, 3 );
}

void LLCC68SetRx( uint32_t timeout )
{
    uint8_t buf[3];

    LLCC68SetOperatingMode( MODE_RX );

    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );
    LLCC68WriteCommand( RADIO_SET_RX, buf, 3 );
}

void LLCC68SetRxBoosted( uint32_t timeout )
{
    uint8_t buf[3];

    LLCC68SetOperatingMode( MODE_RX );

    LLCC68WriteRegister( REG_RX_GAIN, 0x96 ); // max LNA gain, increase current by ~2mA for around ~3dB in sensivity

    buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout & 0xFF );
    LLCC68WriteCommand( RADIO_SET_RX, buf, 3 );
}

void LLCC68SetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
    uint8_t buf[6];

    buf[0] = ( uint8_t )( ( rxTime >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( rxTime >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( rxTime & 0xFF );
    buf[3] = ( uint8_t )( ( sleepTime >> 16 ) & 0xFF );
    buf[4] = ( uint8_t )( ( sleepTime >> 8 ) & 0xFF );
    buf[5] = ( uint8_t )( sleepTime & 0xFF );
    LLCC68WriteCommand( RADIO_SET_RXDUTYCYCLE, buf, 6 );
    LLCC68SetOperatingMode( MODE_RX_DC );
}

void LLCC68SetCad( void )
{
    LLCC68WriteCommand( RADIO_SET_CAD, 0, 0 );
    LLCC68SetOperatingMode( MODE_CAD );
}

void LLCC68SetTxContinuousWave( void )
{
    LLCC68WriteCommand( RADIO_SET_TXCONTINUOUSWAVE, 0, 0 );
    LLCC68SetOperatingMode( MODE_TX );
}

void LLCC68SetTxInfinitePreamble( void )
{
    LLCC68WriteCommand( RADIO_SET_TXCONTINUOUSPREAMBLE, 0, 0 );
    LLCC68SetOperatingMode( MODE_TX );
}

void LLCC68SetStopRxTimerOnPreambleDetect( bool enable )
{
    LLCC68WriteCommand( RADIO_SET_STOPRXTIMERONPREAMBLE, ( uint8_t* )&enable, 1 );
}

void LLCC68SetLoRaSymbNumTimeout( uint8_t symbNum )
{
    uint8_t mant = ( ( ( symbNum > LLCC68_MAX_LORA_SYMB_NUM_TIMEOUT ) ?
                       LLCC68_MAX_LORA_SYMB_NUM_TIMEOUT : 
                       symbNum ) + 1 ) >> 1;
    uint8_t exp  = 0;
    uint8_t reg  = 0;

    while( mant > 31 )
    {
        mant = ( mant + 3 ) >> 2;
        exp++;
    }

    reg = mant << ( 2 * exp + 1 );
    LLCC68WriteCommand( RADIO_SET_LORASYMBTIMEOUT, &reg, 1 );

    if( symbNum != 0 )
    {
        reg = exp + ( mant << 3 );
        LLCC68WriteRegister( REG_LR_SYNCH_TIMEOUT, reg );
    }
}

void LLCC68SetRegulatorMode( RadioRegulatorMode_t mode )
{
    LLCC68WriteCommand( RADIO_SET_REGULATORMODE, ( uint8_t* )&mode, 1 );
}

void LLCC68Calibrate( CalibrationParams_t calibParam )
{
    uint8_t value = ( ( ( uint8_t )calibParam.Fields.ImgEnable << 6 ) |
                      ( ( uint8_t )calibParam.Fields.ADCBulkPEnable << 5 ) |
                      ( ( uint8_t )calibParam.Fields.ADCBulkNEnable << 4 ) |
                      ( ( uint8_t )calibParam.Fields.ADCPulseEnable << 3 ) |
                      ( ( uint8_t )calibParam.Fields.PLLEnable << 2 ) |
                      ( ( uint8_t )calibParam.Fields.RC13MEnable << 1 ) |
                      ( ( uint8_t )calibParam.Fields.RC64KEnable ) );

    LLCC68WriteCommand( RADIO_CALIBRATE, &value, 1 );
}

void LLCC68CalibrateImage( uint32_t freq )
{
    uint8_t calFreq[2];

    if( freq > 900000000 )
    {
        calFreq[0] = 0xE1;
        calFreq[1] = 0xE9;
    }
    else if( freq > 850000000 )
    {
        calFreq[0] = 0xD7;
        calFreq[1] = 0xDB;
    }
    else if( freq > 770000000 )
    {
        calFreq[0] = 0xC1;
        calFreq[1] = 0xC5;
    }
    else if( freq > 460000000 )
    {
        calFreq[0] = 0x75;
        calFreq[1] = 0x81;
    }
    else if( freq > 425000000 )
    {
        calFreq[0] = 0x6B;
        calFreq[1] = 0x6F;
    }
    LLCC68WriteCommand( RADIO_CALIBRATEIMAGE, calFreq, 2 );
}

void LLCC68SetPaConfig( uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut )
{
    uint8_t buf[4];

    buf[0] = paDutyCycle;
    buf[1] = hpMax;
    buf[2] = deviceSel;
    buf[3] = paLut;
    LLCC68WriteCommand( RADIO_SET_PACONFIG, buf, 4 );
}

void LLCC68SetRxTxFallbackMode( uint8_t fallbackMode )
{
    LLCC68WriteCommand( RADIO_SET_TXFALLBACKMODE, &fallbackMode, 1 );
}

void LLCC68SetDioIrqParams( uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask )
{
    uint8_t buf[8];

    buf[0] = ( uint8_t )( ( irqMask >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( irqMask & 0x00FF );
    buf[2] = ( uint8_t )( ( dio1Mask >> 8 ) & 0x00FF );
    buf[3] = ( uint8_t )( dio1Mask & 0x00FF );
    buf[4] = ( uint8_t )( ( dio2Mask >> 8 ) & 0x00FF );
    buf[5] = ( uint8_t )( dio2Mask & 0x00FF );
    buf[6] = ( uint8_t )( ( dio3Mask >> 8 ) & 0x00FF );
    buf[7] = ( uint8_t )( dio3Mask & 0x00FF );
    LLCC68WriteCommand( RADIO_CFG_DIOIRQ, buf, 8 );
}

uint16_t LLCC68GetIrqStatus( void )
{
    uint8_t irqStatus[2];

    LLCC68ReadCommand( RADIO_GET_IRQSTATUS, irqStatus, 2 );
    return ( irqStatus[0] << 8 ) | irqStatus[1];
}

void LLCC68SetDio2AsRfSwitchCtrl( uint8_t enable )
{
    LLCC68WriteCommand( RADIO_SET_RFSWITCHMODE, &enable, 1 );
}

void LLCC68SetDio3AsTcxoCtrl( RadioTcxoCtrlVoltage_t tcxoVoltage, uint32_t timeout )
{
    uint8_t buf[4];

    buf[0] = tcxoVoltage & 0x07;
    buf[1] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( timeout & 0xFF );

    LLCC68WriteCommand( RADIO_SET_TCXOMODE, buf, 4 );
}

void LLCC68SetRfFrequency( uint32_t frequency )
{
    uint8_t buf[4];
	uint32_t freqInPllSteps = 0;

    if( ImageCalibrated == false )
    {
        LLCC68CalibrateImage( frequency );
        ImageCalibrated = true;
    }

    freqInPllSteps = LLCC68ConvertFreqInHzToPllStep( frequency );

    buf[0] = ( uint8_t )( ( freqInPllSteps >> 24 ) & 0xFF );
    buf[1] = ( uint8_t )( ( freqInPllSteps >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( freqInPllSteps >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( freqInPllSteps & 0xFF );
    LLCC68WriteCommand( RADIO_SET_RFFREQUENCY, buf, 4 );
}

void LLCC68SetPacketType( RadioPacketTypes_t packetType )
{
    // Save packet type internally to avoid questioning the radio
    PacketType = packetType;
    LLCC68WriteCommand( RADIO_SET_PACKETTYPE, ( uint8_t* )&packetType, 1 );
}

RadioPacketTypes_t LLCC68GetPacketType( void )
{
    return PacketType;
}

void LLCC68SetTxParams( int8_t power, RadioRampTimes_t rampTime )
{
    uint8_t buf[2];

    if( LLCC68GetDeviceId( ) == SX1261 )
    {
        if( power == 15 )
        {
            LLCC68SetPaConfig( 0x06, 0x00, 0x01, 0x01 );
        }
        else
        {
            LLCC68SetPaConfig( 0x04, 0x00, 0x01, 0x01 );
        }
        if( power >= 14 )
        {
            power = 14;
        }
        else if( power < -17 )
        {
            power = -17;
        }
    }
    else // sx1262
    {
        // WORKAROUND - Better Resistance of the SX1262 Tx to Antenna Mismatch, see DS_SX1261-2_V1.2 datasheet chapter 15.2
        // RegTxClampConfig = @address 0x08D8
        LLCC68WriteRegister( 0x08D8, LLCC68ReadRegister( 0x08D8 ) | ( 0x0F << 1 ) );
        // WORKAROUND END

        LLCC68SetPaConfig( 0x04, 0x07, 0x00, 0x01 );
        if( power > 22 )
        {
            power = 22;
        }
        else if( power < -9 )
        {
            power = -9;
        }
    }
    buf[0] = power;
    buf[1] = ( uint8_t )rampTime;
    LLCC68WriteCommand( RADIO_SET_TXPARAMS, buf, 2 );
}

void LLCC68SetModulationParams( ModulationParams_t *modulationParams )
{
    uint8_t n;
    uint32_t tempVal = 0;
    uint8_t buf[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // Check if required configuration corresponds to the stored packet type
    // If not, silently update radio packet type
    if( PacketType != modulationParams->PacketType )
    {
        LLCC68SetPacketType( modulationParams->PacketType );
    }

    switch( modulationParams->PacketType )
    {
    case PACKET_TYPE_GFSK:
        n = 8;
        tempVal = ( uint32_t )( 32 * LLCC68_XTAL_FREQ / modulationParams->Params.Gfsk.BitRate );
        buf[0] = ( tempVal >> 16 ) & 0xFF;
        buf[1] = ( tempVal >> 8 ) & 0xFF;
        buf[2] = tempVal & 0xFF;
        buf[3] = modulationParams->Params.Gfsk.ModulationShaping;
        buf[4] = modulationParams->Params.Gfsk.Bandwidth;
        tempVal = LLCC68ConvertFreqInHzToPllStep( modulationParams->Params.Gfsk.Fdev );
        buf[5] = ( tempVal >> 16 ) & 0xFF;
        buf[6] = ( tempVal >> 8 ) & 0xFF;
        buf[7] = ( tempVal& 0xFF );
        LLCC68WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, n );
        break;
    case PACKET_TYPE_LORA:
        n = 4;
        buf[0] = modulationParams->Params.LoRa.SpreadingFactor;
        buf[1] = modulationParams->Params.LoRa.Bandwidth;
        buf[2] = modulationParams->Params.LoRa.CodingRate;
        buf[3] = modulationParams->Params.LoRa.LowDatarateOptimize;

        LLCC68WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, n );

        break;
    default:
    case PACKET_TYPE_NONE:
        return;
    }
}

void LLCC68SetPacketParams( PacketParams_t *packetParams )
{
    uint8_t n;
    uint8_t crcVal = 0;
    uint8_t buf[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    // Check if required configuration corresponds to the stored packet type
    // If not, silently update radio packet type
    if( PacketType != packetParams->PacketType )
    {
        LLCC68SetPacketType( packetParams->PacketType );
    }

    switch( packetParams->PacketType )
    {
    case PACKET_TYPE_GFSK:
        if( packetParams->Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES_IBM )
        {
            LLCC68SetCrcSeed( CRC_IBM_SEED );
            LLCC68SetCrcPolynomial( CRC_POLYNOMIAL_IBM );
            crcVal = RADIO_CRC_2_BYTES;
        }
        else if( packetParams->Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES_CCIT )
        {
            LLCC68SetCrcSeed( CRC_CCITT_SEED );
            LLCC68SetCrcPolynomial( CRC_POLYNOMIAL_CCITT );
            crcVal = RADIO_CRC_2_BYTES_INV;
        }
        else
        {
            crcVal = packetParams->Params.Gfsk.CrcLength;
        }
        n = 9;
        buf[0] = ( packetParams->Params.Gfsk.PreambleLength >> 8 ) & 0xFF;
        buf[1] = packetParams->Params.Gfsk.PreambleLength;
        buf[2] = packetParams->Params.Gfsk.PreambleMinDetect;
        buf[3] = ( packetParams->Params.Gfsk.SyncWordLength /*<< 3*/ ); // convert from byte to bit
        buf[4] = packetParams->Params.Gfsk.AddrComp;
        buf[5] = packetParams->Params.Gfsk.HeaderType;
        buf[6] = packetParams->Params.Gfsk.PayloadLength;
        buf[7] = crcVal;
        buf[8] = packetParams->Params.Gfsk.DcFree;
        break;
    case PACKET_TYPE_LORA:
        n = 6;
        buf[0] = ( packetParams->Params.LoRa.PreambleLength >> 8 ) & 0xFF;
        buf[1] = packetParams->Params.LoRa.PreambleLength;
        buf[2] = LoRaHeaderType = packetParams->Params.LoRa.HeaderType;
        buf[3] = packetParams->Params.LoRa.PayloadLength;
        buf[4] = packetParams->Params.LoRa.CrcMode;
        buf[5] = packetParams->Params.LoRa.InvertIQ;
        break;
    default:
    case PACKET_TYPE_NONE:
        return;
    }
    LLCC68WriteCommand( RADIO_SET_PACKETPARAMS, buf, n );
}

void LLCC68SetCadParams( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, RadioCadExitModes_t cadExitMode, uint32_t cadTimeout )
{
    uint8_t buf[7];

    buf[0] = ( uint8_t )cadSymbolNum;
    buf[1] = cadDetPeak;
    buf[2] = cadDetMin;
    buf[3] = ( uint8_t )cadExitMode;
    buf[4] = ( uint8_t )( ( cadTimeout >> 16 ) & 0xFF );
    buf[5] = ( uint8_t )( ( cadTimeout >> 8 ) & 0xFF );
    buf[6] = ( uint8_t )( cadTimeout & 0xFF );
    LLCC68WriteCommand( RADIO_SET_CADPARAMS, buf, 7 );
    LLCC68SetOperatingMode( MODE_CAD );
}

void LLCC68SetBufferBaseAddress( uint8_t txBaseAddress, uint8_t rxBaseAddress )
{
    uint8_t buf[2];

    buf[0] = txBaseAddress;
    buf[1] = rxBaseAddress;
    LLCC68WriteCommand( RADIO_SET_BUFFERBASEADDRESS, buf, 2 );
}

RadioStatus_t LLCC68GetStatus( void )
{
    uint8_t stat = 0;
    RadioStatus_t status = { 0 };
		status.Value = 0;

    stat = LLCC68ReadCommand( RADIO_GET_STATUS, NULL, 0 );
    status.Fields.CmdStatus = ( stat & ( 0x07 << 1 ) ) >> 1;
    status.Fields.ChipMode = ( stat & ( 0x07 << 4 ) ) >> 4;
    return status;
}

int8_t LLCC68GetRssiInst( void )
{
    uint8_t buf[1];
    int8_t rssi = 0;

    LLCC68ReadCommand( RADIO_GET_RSSIINST, buf, 1 );
    rssi = -buf[0] >> 1;
    return rssi;
}

void LLCC68GetRxBufferStatus( uint8_t *payloadLength, uint8_t *rxStartBufferPointer )
{
    uint8_t status[2];

    LLCC68ReadCommand( RADIO_GET_RXBUFFERSTATUS, status, 2 );

    // In case of LORA fixed header, the payloadLength is obtained by reading
    // the register REG_LR_PAYLOADLENGTH
    if( ( LLCC68GetPacketType( ) == PACKET_TYPE_LORA ) && ( LoRaHeaderType == LORA_PACKET_FIXED_LENGTH ) )
    {
        *payloadLength = LLCC68ReadRegister( REG_LR_PAYLOADLENGTH );
    }
    else
    {
        *payloadLength = status[0];
    }
    *rxStartBufferPointer = status[1];
}

void LLCC68GetPacketStatus( PacketStatus_t *pktStatus )
{
    uint8_t status[3];

    LLCC68ReadCommand( RADIO_GET_PACKETSTATUS, status, 3 );

    pktStatus->packetType = LLCC68GetPacketType( );
    switch( pktStatus->packetType )
    {
        case PACKET_TYPE_GFSK:
            pktStatus->Params.Gfsk.RxStatus = status[0];
            pktStatus->Params.Gfsk.RssiSync = -status[1] >> 1;
            pktStatus->Params.Gfsk.RssiAvg = -status[2] >> 1;
            pktStatus->Params.Gfsk.FreqError = 0;
            break;

        case PACKET_TYPE_LORA:
            pktStatus->Params.LoRa.RssiPkt = -status[0] >> 1;
            // Returns SNR value [dB] rounded to the nearest integer value
            pktStatus->Params.LoRa.SnrPkt = ( ( ( int8_t )status[1] ) + 2 ) >> 2;
            pktStatus->Params.LoRa.SignalRssiPkt = -status[2] >> 1;
            pktStatus->Params.LoRa.FreqError = FrequencyError;
            break;

        default:
        case PACKET_TYPE_NONE:
            // In that specific case, we set everything in the pktStatus to zeros
            // and reset the packet type accordingly
            memset( pktStatus, 0, sizeof( PacketStatus_t ) );
            pktStatus->packetType = PACKET_TYPE_NONE;
            break;
    }
}

RadioError_t LLCC68GetDeviceErrors( void )
{
    uint8_t err[] = { 0, 0 };
    RadioError_t error = { 0 };
		error.Value = 0;

    LLCC68ReadCommand( RADIO_GET_ERROR, ( uint8_t* )err, 2 );
    error.Fields.PaRamp     = ( err[0] & ( 1 << 0 ) ) >> 0;
    error.Fields.PllLock    = ( err[1] & ( 1 << 6 ) ) >> 6;
    error.Fields.XoscStart  = ( err[1] & ( 1 << 5 ) ) >> 5;
    error.Fields.ImgCalib   = ( err[1] & ( 1 << 4 ) ) >> 4;
    error.Fields.AdcCalib   = ( err[1] & ( 1 << 3 ) ) >> 3;
    error.Fields.PllCalib   = ( err[1] & ( 1 << 2 ) ) >> 2;
    error.Fields.Rc13mCalib = ( err[1] & ( 1 << 1 ) ) >> 1;
    error.Fields.Rc64kCalib = ( err[1] & ( 1 << 0 ) ) >> 0;
    return error;
}

void LLCC68ClearDeviceErrors( void )
{
    uint8_t buf[2] = { 0x00, 0x00 };
    LLCC68WriteCommand( RADIO_CLR_ERROR, buf, 2 );
}

void LLCC68ClearIrqStatus( uint16_t irq )
{
    uint8_t buf[2];

    buf[0] = ( uint8_t )( ( ( uint16_t )irq >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( ( uint16_t )irq & 0x00FF );
    LLCC68WriteCommand( RADIO_CLR_IRQSTATUS, buf, 2 );
}

static uint32_t LLCC68ConvertFreqInHzToPllStep( uint32_t freqInHz )
{
    uint32_t stepsInt;
    uint32_t stepsFrac;

    // pllSteps = freqInHz / (LLCC68_XTAL_FREQ / 2^19 )
    // Get integer and fractional parts of the frequency computed with a PLL step scaled value
    stepsInt = freqInHz / LLCC68_PLL_STEP_SCALED;
    stepsFrac = freqInHz - ( stepsInt * LLCC68_PLL_STEP_SCALED );
    
    // Apply the scaling factor to retrieve a frequency in Hz (+ ceiling)
    return ( stepsInt << LLCC68_PLL_STEP_SHIFT_AMOUNT ) + 
           ( ( ( stepsFrac << LLCC68_PLL_STEP_SHIFT_AMOUNT ) + ( LLCC68_PLL_STEP_SCALED >> 1 ) ) /
             LLCC68_PLL_STEP_SCALED );
}


/*!
 * \file      radio.c
 *
 * \brief     Radio driver API definition
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */

#define TIP_FLAG 0

/*!
 * Defines the time required for the TCXO to wakeup [ms].
 */
#define BOARD_TCXO_WAKEUP_TIME                      0

/*!
 * \brief Initializes the radio
 *
 * \param [IN] events Structure containing the driver callback functions
 */
void RadioInit( RadioEvents_t *events );

/*!
 * Return current radio status
 *
 * \param status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
RadioState_t RadioGetStatus( void );

/*!
 * \brief Configures the radio with the given modem
 *
 * \param [IN] modem Modem to be used [0: FSK, 1: LoRa]
 */
void RadioSetModem( RadioModems_t modem );

/*!
 * \brief Sets the channel frequency
 *
 * \param [IN] freq         Channel RF frequency
 */
void RadioSetChannel( uint32_t freq );

/*!
 * \brief Checks if the channel is free for the given time
 *
 * \remark The FSK modem is always used for this task as we can select the Rx bandwidth at will.
 *
 * \param [IN] freq                Channel RF frequency in Hertz
 * \param [IN] rxBandwidth         Rx bandwidth in Hertz
 * \param [IN] rssiThresh          RSSI threshold in dBm
 * \param [IN] maxCarrierSenseTime Max time in milliseconds while the RSSI is measured
 *
 * \retval isFree         [true: Channel is free, false: Channel is not free]
 */
bool RadioIsChannelFree( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime );

/*!
 * \brief Generates a 32 bits random value based on the RSSI readings
 *
 * \remark This function sets the radio in LoRa modem mode and disables
 *         all interrupts.
 *         After calling this function either Radio.SetRxConfig or
 *         Radio.SetTxConfig functions must be called.
 *
 * \retval randomValue    32 bits random value
 */
uint32_t RadioRandom( void );

/*!
 * \brief Sets the reception parameters
 *
 * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] bandwidthAfc Sets the AFC Bandwidth (FSK only)
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: N/A ( set to 0 )
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] symbTimeout  Sets the RxSingle timeout value
 *                          FSK : timeout in number of bytes
 *                          LoRa: timeout in symbols
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param [IN] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param [IN] rxContinuous Sets the reception in continuous mode
 *                          [false: single mode, true: continuous mode]
 */
void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                          uint32_t datarate, uint8_t coderate,
                          uint32_t bandwidthAfc, uint16_t preambleLen,
                          uint16_t symbTimeout, bool fixLen,
                          uint8_t payloadLen,
                          bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                          bool iqInverted, bool rxContinuous );

/*!
 * \brief Sets the transmission parameters
 *
 * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] power        Sets the output power [dBm]
 * \param [IN] fdev         Sets the frequency deviation (FSK only)
 *                          FSK : [Hz]
 *                          LoRa: 0
 * \param [IN] bandwidth    Sets the bandwidth (LoRa only)
 *                          FSK : 0
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] crcOn        Enables disables the CRC [0: OFF, 1: ON]
 * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param [IN] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param [IN] timeout      Transmission timeout [ms]
 */
void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                          uint32_t bandwidth, uint32_t datarate,
                          uint8_t coderate, uint16_t preambleLen,
                          bool fixLen, bool crcOn, bool FreqHopOn,
                          uint8_t HopPeriod, bool iqInverted, uint32_t timeout );

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param [IN] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
bool RadioCheckRfFrequency( uint32_t frequency );

/*!
 * \brief Computes the packet time on air in ms for the given payload
 *
 * \Remark Can only be called once SetRxConfig or SetTxConfig have been called
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 *
 * \retval airTime        Computed airTime (ms) for the given packet payload length
 */
uint32_t RadioTimeOnAir( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn );

/*!
 * \brief Sends the buffer of size. Prepares the packet to be sent and sets
 *        the radio in transmission
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
void RadioSend( uint8_t *buffer, uint8_t size );

/*!
 * \brief Sets the radio in sleep mode
 */
void RadioSleep( void );

/*!
 * \brief Sets the radio in standby mode
 */
void RadioStandby( void );

/*!
 * \brief Sets the radio in reception mode for the given time
 * \param [IN] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
void RadioRx( uint32_t timeout );

/*!
 * \brief Start a Channel Activity Detection
 */
void RadioStartCad( void );

/*!
 * \brief Sets the radio in continuous wave transmission mode
 *
 * \param [IN]: freq       Channel RF frequency
 * \param [IN]: power      Sets the output power [dBm]
 * \param [IN]: time       Transmission mode timeout [s]
 */
void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time );

/*!
 * \brief Reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
int16_t RadioRssi( RadioModems_t modem );

/*!
 * \brief Writes the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [IN]: data New register value
 */
void RadioWrite( uint32_t addr, uint8_t data );

/*!
 * \brief Reads the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \retval data Register value
 */
uint8_t RadioRead( uint32_t addr );

/*!
 * \brief Writes multiple radio registers starting at address
 *
 * \param [IN] addr   First Radio register address
 * \param [IN] buffer Buffer containing the new register's values
 * \param [IN] size   Number of registers to be written
 */
void RadioWriteBuffer( uint32_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads multiple radio registers starting at address
 *
 * \param [IN] addr First Radio register address
 * \param [OUT] buffer Buffer where to copy the registers data
 * \param [IN] size Number of registers to be read
 */
void RadioReadBuffer( uint32_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Sets the maximum payload length.
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] max        Maximum payload length in bytes
 */
void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max );

/*!
 * \brief Sets the network to public or private. Updates the sync byte.
 *
 * \remark Applies to LoRa modem only
 *
 * \param [IN] enable if true, it enables a public network
 */
void RadioSetPublicNetwork( bool enable );

/*!
 * \brief Gets the time required for the board plus radio to get out of sleep.[ms]
 *
 * \retval time Radio plus board wakeup time in ms.
 */
uint32_t RadioGetWakeupTime( void );

/*!
 * \brief Process radio irq
 */
void RadioIrqProcess( void );

/*!
 * \brief Sets the radio in reception mode with Max LNA gain for the given time
 * \param [IN] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
void RadioRxBoosted( uint32_t timeout );

/*!
 * \brief Sets the Rx duty cycle management parameters
 *
 * \param [in]  rxTime        Structure describing reception timeout value
 * \param [in]  sleepTime     Structure describing sleep timeout value
 */
void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime );

/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    RadioInit,
    RadioGetStatus,
    RadioSetModem,
    RadioSetChannel,
    RadioIsChannelFree,
    RadioRandom,
    RadioSetRxConfig,
    RadioSetTxConfig,
    RadioCheckRfFrequency,
    RadioTimeOnAir,
    RadioSend,
    RadioSleep,
    RadioStandby,
    RadioRx,
    RadioStartCad,
    RadioSetTxContinuousWave,
    RadioRssi,
    RadioWrite,
    RadioRead,
    RadioWriteBuffer,
    RadioReadBuffer,
    RadioSetMaxPayloadLength,
    RadioSetPublicNetwork,
    RadioGetWakeupTime,
    RadioIrqProcess,
    // Available on LLCC68 only
    RadioRxBoosted,
    RadioSetRxDutyCycle
};

/*
 * Local types definition
 */


 /*!
 * FSK bandwidth definition
 */
typedef struct
{
    uint32_t bandwidth;
    uint8_t  RegValue;
}FskBandwidth_t;

/*!
 * Precomputed FSK bandwidth registers values
 */
const FskBandwidth_t FskBandwidths[] =
{
    { 4800  , 0x1F },
    { 5800  , 0x17 },
    { 7300  , 0x0F },
    { 9700  , 0x1E },
    { 11700 , 0x16 },
    { 14600 , 0x0E },
    { 19500 , 0x1D },
    { 23400 , 0x15 },
    { 29300 , 0x0D },
    { 39000 , 0x1C },
    { 46900 , 0x14 },
    { 58600 , 0x0C },
    { 78200 , 0x1B },
    { 93800 , 0x13 },
    { 117300, 0x0B },
    { 156200, 0x1A },
    { 187200, 0x12 },
    { 234300, 0x0A },
    { 312000, 0x19 },
    { 373600, 0x11 },
    { 467000, 0x09 },
    { 500000, 0x00 }, // Invalid Bandwidth
};

const RadioLoRaBandwidths_t Bandwidths[] = { LORA_BW_125, LORA_BW_250, LORA_BW_500 };

uint8_t MaxPayloadLength = 0xFF;

uint32_t TxTimeout = 0;
uint32_t RxTimeout = 0;

bool RxContinuous = false;


PacketStatus_t RadioPktStatus;
uint8_t RadioRxPayload[255];

bool IrqFired = false;

/*
 * LLCC68 DIO IRQ callback functions prototype
 */

/*!
 * \brief DIO 0 IRQ callback
 */
void RadioOnDioIrq( void* context );

/*
 * Private global variables
 */


/*!
 * Holds the current network type for the radio
 */
typedef struct
{
    bool Previous;
    bool Current;
}RadioPublicNetwork_t;

static RadioPublicNetwork_t RadioPublicNetwork = { false };

/*!
 * Radio callbacks variable
 */
static RadioEvents_t* RadioEvents;

/*
 * Public global variables
 */

/*!
 * Radio hardware and global parameters
 */
LLCC68_t LLCC68;

/*!
 * \brief Holds the internal operating mode of the radio
 */
static RadioOperatingModes_t OperatingMode;

//------------------------ board code start �� board �ƶ������Ĵ��� -----------------
RadioOperatingModes_t LLCC68GetOperatingMode( void )
{
    return OperatingMode;
}

void LLCC68SetOperatingMode( RadioOperatingModes_t mode )
{
    OperatingMode = mode;
}

void LLCC68IoTcxoInit( void )
{
    // No TCXO component available on this board design.
}

uint32_t LLCC68GetBoardTcxoWakeupTime( void )
{
    return BOARD_TCXO_WAKEUP_TIME;
}


//����DIO2λRF���ƽ�
void LLCC68IoRfSwitchInit( void )
{
    LLCC68SetDio2AsRfSwitchCtrl( true );
}

//�����������ڲ��Զ����ƣ�����ר������
void LLCC68AntSwOn( void )
{
    //GpioInit( &AntPow, RADIO_ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
}
//�����������ڲ��Զ����ƣ�����ר������
void LLCC68AntSwOff( void )
{
    //GpioInit( &AntPow, RADIO_ANT_SWITCH_POWER, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

uint8_t LLCC68GetDeviceId( void )
{
    return SX1262;
}

void LLCC68SetRfTxPower( int8_t power )
{
    LLCC68SetTxParams( power, RADIO_RAMP_40_US );
}

void LLCC68WriteRegister( uint16_t address, uint8_t value )
{
    LLCC68WriteRegisters( address, &value, 1 );
}

uint8_t LLCC68ReadRegister( uint16_t address )
{
    uint8_t data;
    LLCC68ReadRegisters( address, &data, 1 );
    return data;
}

void LLCC68Wakeup( void )
{
	CRITICAL_SECTION_BEGIN( );

	LLCC68SetNss(0);

	LLCC68SpiInOut(RADIO_GET_STATUS );
	LLCC68SpiInOut(0x00 );

	LLCC68SetNss(1);

	// Wait for chip to be ready.
	LLCC68WaitOnBusy( );

	// Update operating mode context variable
	LLCC68SetOperatingMode( MODE_STDBY_RC );

	CRITICAL_SECTION_END( );
}

void LLCC68WriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
	int i = 0;
	
#if (1 == TIP_FLAG)
	printf("[WriteCommand] command: %04X\r\nbuffer:", command);
	for(i = 0; i < size; i++ ){
		printf("[%d] %04X\r\n", i, buffer[i]);
	}
#endif
	
	LLCC68CheckDeviceReady( );

	LLCC68SetNss(0);

	LLCC68SpiInOut(( uint8_t )command);

	for( i = 0; i < size; i++ ){
		LLCC68SpiInOut(buffer[i]);
	}

	LLCC68SetNss(1);

	if( command != RADIO_SET_SLEEP ){
		LLCC68WaitOnBusy( );
	}
}

uint8_t LLCC68ReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
	uint8_t status = 0;
	uint16_t i = 0;
	
	LLCC68CheckDeviceReady( );

	LLCC68SetNss(0);

	LLCC68SpiInOut(( uint8_t )command);
	status = LLCC68SpiInOut(0x00);
	for( i = 0; i < size; i++ ){
		buffer[i] = LLCC68SpiInOut(0);
	}

	LLCC68SetNss(1);

	LLCC68WaitOnBusy( );
	
	#if (1 == TIP_FLAG)
	printf("[ReadCommand] command: %04X\r\nbuffer:", command);
	for(i = 0; i < size; i++ ){
		printf("[%d] %04X\r\n", i, buffer[i]);
	}
	#endif
	
	return status;
}

void LLCC68WriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
	uint16_t i = 0;
	
	#if (1 == TIP_FLAG)
	printf("[Write] Registers ADDR: %04X\r\nbuffer:", address);
	for(i = 0; i < size; i++ ){
		printf("[%d] %04X\r\n", i, buffer[i]);
	}
	#endif
	
	LLCC68CheckDeviceReady( );

	LLCC68SetNss(0);
    
	LLCC68SpiInOut(RADIO_WRITE_REGISTER );
	LLCC68SpiInOut(( address & 0xFF00 ) >> 8 );
	LLCC68SpiInOut( address & 0x00FF );
    
	for( i = 0; i < size; i++ ){
		LLCC68SpiInOut(buffer[i] );
	}

	LLCC68SetNss(1);

	LLCC68WaitOnBusy( );
}

void LLCC68ReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
	uint16_t i = 0;
	
	LLCC68CheckDeviceReady( );
	LLCC68SetNss(0);
	
	LLCC68SpiInOut(RADIO_READ_REGISTER);
	LLCC68SpiInOut(( address & 0xFF00 ) >> 8);
	LLCC68SpiInOut(address & 0x00FF);
	LLCC68SpiInOut(0);
	
	for( i = 0; i < size; i++ ){
		buffer[i] = LLCC68SpiInOut(0);
	}
	LLCC68SetNss(1);

	LLCC68WaitOnBusy( );
	
	#if (1 == TIP_FLAG)
	printf("[Read] Registers ADDR: %04X\r\nbuffer:", address);
	for(i = 0; i < size; i++ ){
		printf("[%d] %04X\r\n", i, buffer[i]);
	}
	#endif
}

void LLCC68WriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
	uint16_t i = 0;
	#if (1 == TIP_FLAG)
	printf("[WriteBuffer] buffer offset: %04X\r\nbuffer:", offset);
	for(i = 0; i < size; i++ ){
		printf("[%d] %04X\r\n", i, buffer[i]);
	}
	#endif
	
	LLCC68CheckDeviceReady( );

	LLCC68SetNss(0);

	LLCC68SpiInOut(RADIO_WRITE_BUFFER );
	LLCC68SpiInOut(offset );
	for( i = 0; i < size; i++ ){
		LLCC68SpiInOut(buffer[i] );
	}
	LLCC68SetNss(1);

	LLCC68WaitOnBusy( );
}

void LLCC68ReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
	uint16_t i = 0;
	
	LLCC68CheckDeviceReady( );

	LLCC68SetNss(0);

	LLCC68SpiInOut(RADIO_READ_BUFFER );
	LLCC68SpiInOut(offset );
	LLCC68SpiInOut(0 );
	for( i = 0; i < size; i++ ){
		buffer[i] = LLCC68SpiInOut(0);
	}
	LLCC68SetNss(1);

	LLCC68WaitOnBusy( );
	
	#if (1 == TIP_FLAG)
	printf("[ReadBuffer] buffer offset: %04X\r\nbuffer:", offset);
	for(i = 0; i < size; i++ ){
		printf("[%d] %04X\r\n", i, buffer[i]);
	}
	#endif
}
//------------------------ board code end -----------------

/*!
 * Returns the known FSK bandwidth registers value
 *
 * \param [IN] bandwidth Bandwidth value in Hz
 * \retval regValue Bandwidth register value.
 */
static uint8_t RadioGetFskBandwidthRegValue( uint32_t bandwidth )
{
    uint8_t i;

    if( bandwidth == 0 )
    {
        return( 0x1F );
    }

    for( i = 0; i < ( sizeof( FskBandwidths ) / sizeof( FskBandwidth_t ) ) - 1; i++ )
    {
        if( ( bandwidth >= FskBandwidths[i].bandwidth ) && ( bandwidth < FskBandwidths[i + 1].bandwidth ) )
        {
            return FskBandwidths[i+1].RegValue;
        }
    }
    // ERROR: Value not found
    while( 1 );
}

void RadioInit( RadioEvents_t *events )
{
    RadioEvents = events;
    LLCC68IoInit();

    LLCC68Init( RadioOnDioIrq );
    LLCC68SetStandby( STDBY_RC );
    LLCC68SetRegulatorMode( USE_DCDC );

    LLCC68SetBufferBaseAddress( 0x00, 0x00 );
    LLCC68SetTxParams( 0, RADIO_RAMP_200_US );
    LLCC68SetDioIrqParams( IRQ_RADIO_ALL, IRQ_RADIO_ALL, IRQ_RADIO_NONE, IRQ_RADIO_NONE );

    LLCC68TimerInit();

    IrqFired = false;
}

RadioState_t RadioGetStatus( void )
{
    switch( LLCC68GetOperatingMode( ) )
    {
        case MODE_TX:
            return RF_TX_RUNNING;
        case MODE_RX:
            return RF_RX_RUNNING;
        case MODE_CAD:
            return RF_CAD;
        default:
            return RF_IDLE;
    }
}

void RadioSetModem( RadioModems_t modem )
{
    switch( modem )
    {
    default:
    case MODEM_FSK:
        LLCC68SetPacketType( PACKET_TYPE_GFSK );
        // When switching to GFSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        RadioPublicNetwork.Current = false;
        break;
    case MODEM_LORA:
        LLCC68SetPacketType( PACKET_TYPE_LORA );
        // Public/Private network register is reset when switching modems
        if( RadioPublicNetwork.Current != RadioPublicNetwork.Previous )
        {
            RadioPublicNetwork.Current = RadioPublicNetwork.Previous;
            RadioSetPublicNetwork( RadioPublicNetwork.Current );
        }
        break;
    }
}

void RadioSetChannel( uint32_t freq )
{
    LLCC68SetRfFrequency( freq );
}

bool RadioIsChannelFree( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    bool     status           = true;
    int16_t  rssi             = 0;
    uint32_t count = 0;

    RadioSetModem( MODEM_FSK );

    RadioSetChannel( freq );

    // Set Rx bandwidth. Other parameters are not used.
    RadioSetRxConfig( MODEM_FSK, rxBandwidth, 600, 0, rxBandwidth, 3, 0, false,
                      0, false, 0, 0, false, true );
    RadioRx( 0 );

    LLCC68DelayMs( 1 );

    // Perform carrier sense for maxCarrierSenseTime
		count = 0;
    while( count < maxCarrierSenseTime )
    {
        rssi = RadioRssi( MODEM_FSK );

        if( rssi > rssiThresh )
        {
            status = false;
            break;
        }
				LLCC68DelayMs( 1 );
				count++;
    }
    RadioSleep( );
    return status;
}

uint32_t RadioRandom( void )
{
    uint32_t rnd = 0;

    /*
     * Radio setup for random number generation
     */
    // Set LoRa modem ON
    RadioSetModem( MODEM_LORA );

    // Disable LoRa modem interrupts
    LLCC68SetDioIrqParams( IRQ_RADIO_NONE, IRQ_RADIO_NONE, IRQ_RADIO_NONE, IRQ_RADIO_NONE );

    rnd = LLCC68GetRandom( );

    return rnd;
}

void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                         uint32_t datarate, uint8_t coderate,
                         uint32_t bandwidthAfc, uint16_t preambleLen,
                         uint16_t symbTimeout, bool fixLen,
                         uint8_t payloadLen,
                         bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                         bool iqInverted, bool rxContinuous )
{
    uint8_t syncWordBuf[]={ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
		
    RxContinuous = rxContinuous;
    if( rxContinuous == true )
    {
        symbTimeout = 0;
    }
    if( fixLen == true )
    {
        MaxPayloadLength = payloadLen;
    }
    else
    {
        MaxPayloadLength = 0xFF;
    }

    switch( modem )
    {
        case MODEM_FSK:
            LLCC68SetStopRxTimerOnPreambleDetect( false );
            LLCC68.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            LLCC68.ModulationParams.Params.Gfsk.BitRate = datarate;
            LLCC68.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            LLCC68.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth << 1 ); // LLCC68 badwidth is double sided

            LLCC68.PacketParams.PacketType = PACKET_TYPE_GFSK;
            LLCC68.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            LLCC68.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            LLCC68.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3; // convert byte into bit
            LLCC68.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            LLCC68.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;
            LLCC68.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            if( crcOn == true )
            {
                LLCC68.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                LLCC68.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            LLCC68.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( LLCC68.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            LLCC68SetModulationParams( &LLCC68.ModulationParams );
            LLCC68SetPacketParams( &LLCC68.PacketParams );
            LLCC68SetSyncWord( syncWordBuf );
            LLCC68SetWhiteningSeed( 0x01FF );

            RxTimeout = ( uint32_t )symbTimeout * 8000UL / datarate;
            break;

        case MODEM_LORA:
            LLCC68SetStopRxTimerOnPreambleDetect( false );
            LLCC68.ModulationParams.PacketType = PACKET_TYPE_LORA;
            LLCC68.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t )datarate;
            LLCC68.ModulationParams.Params.LoRa.Bandwidth = Bandwidths[bandwidth];
            LLCC68.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                LLCC68.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                LLCC68.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            LLCC68.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( LLCC68.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( LLCC68.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    LLCC68.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    LLCC68.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                LLCC68.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            LLCC68.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;

            LLCC68.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            LLCC68.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            LLCC68.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( LLCC68.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            LLCC68SetModulationParams( &LLCC68.ModulationParams );
            LLCC68SetPacketParams( &LLCC68.PacketParams );
            LLCC68SetLoRaSymbNumTimeout( symbTimeout );

            // WORKAROUND - Optimizing the Inverted IQ Operation, see DS_SX1261-2_V1.2 datasheet chapter 15.4
            if( LLCC68.PacketParams.Params.LoRa.InvertIQ == LORA_IQ_INVERTED )
            {
                // RegIqPolaritySetup = @address 0x0736
                LLCC68WriteRegister( 0x0736, LLCC68ReadRegister( 0x0736 ) & ~( 1 << 2 ) );
            }
            else
            {
                // RegIqPolaritySetup @address 0x0736
                LLCC68WriteRegister( 0x0736, LLCC68ReadRegister( 0x0736 ) | ( 1 << 2 ) );
            }
            // WORKAROUND END

            // Timeout Max, Timeout handled directly in SetRx function
            RxTimeout = 0xFFFF;

            break;
    }
}

void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                        uint32_t bandwidth, uint32_t datarate,
                        uint8_t coderate, uint16_t preambleLen,
                        bool fixLen, bool crcOn, bool freqHopOn,
                        uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
    uint8_t syncWordBuf[] = { 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
		
    switch( modem )
    {
        case MODEM_FSK:
            LLCC68.ModulationParams.PacketType = PACKET_TYPE_GFSK;
            LLCC68.ModulationParams.Params.Gfsk.BitRate = datarate;

            LLCC68.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            LLCC68.ModulationParams.Params.Gfsk.Bandwidth = RadioGetFskBandwidthRegValue( bandwidth << 1 ); // LLCC68 badwidth is double sided
            LLCC68.ModulationParams.Params.Gfsk.Fdev = fdev;

            LLCC68.PacketParams.PacketType = PACKET_TYPE_GFSK;
            LLCC68.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            LLCC68.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            LLCC68.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3 ; // convert byte into bit
            LLCC68.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            LLCC68.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;

            if( crcOn == true )
            {
                LLCC68.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                LLCC68.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            LLCC68.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( LLCC68.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            LLCC68SetModulationParams( &LLCC68.ModulationParams );
            LLCC68SetPacketParams( &LLCC68.PacketParams );
            LLCC68SetSyncWord( syncWordBuf );
            LLCC68SetWhiteningSeed( 0x01FF );
            break;

        case MODEM_LORA:
            LLCC68.ModulationParams.PacketType = PACKET_TYPE_LORA;
            LLCC68.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) datarate;
            LLCC68.ModulationParams.Params.LoRa.Bandwidth =  Bandwidths[bandwidth];
            LLCC68.ModulationParams.Params.LoRa.CodingRate= ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
            ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                LLCC68.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                LLCC68.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            LLCC68.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( LLCC68.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( LLCC68.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    LLCC68.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    LLCC68.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                LLCC68.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            LLCC68.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;
            LLCC68.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            LLCC68.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            LLCC68.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( LLCC68.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            LLCC68SetModulationParams( &LLCC68.ModulationParams );
            LLCC68SetPacketParams( &LLCC68.PacketParams );
            break;
    }

    // WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth, see DS_SX1261-2_V1.2 datasheet chapter 15.1
    if( ( modem == MODEM_LORA ) && ( LLCC68.ModulationParams.Params.LoRa.Bandwidth == LORA_BW_500 ) )
    {
        // RegTxModulation = @address 0x0889
        LLCC68WriteRegister( 0x0889, LLCC68ReadRegister( 0x0889 ) & ~( 1 << 2 ) );
    }
    else
    {
        // RegTxModulation = @address 0x0889
        LLCC68WriteRegister( 0x0889, LLCC68ReadRegister( 0x0889 ) | ( 1 << 2 ) );
    }
    // WORKAROUND END

    LLCC68SetRfTxPower( power );
    TxTimeout = timeout;
}

bool RadioCheckRfFrequency( uint32_t frequency )
{
    return true;
}

static uint32_t RadioGetLoRaBandwidthInHz( RadioLoRaBandwidths_t bw )
{
    uint32_t bandwidthInHz = 0;

    switch( bw )
    {
    case LORA_BW_007:
        bandwidthInHz = 7812UL;
        break;
    case LORA_BW_010:
        bandwidthInHz = 10417UL;
        break;
    case LORA_BW_015:
        bandwidthInHz = 15625UL;
        break;
    case LORA_BW_020:
        bandwidthInHz = 20833UL;
        break;
    case LORA_BW_031:
        bandwidthInHz = 31250UL;
        break;
    case LORA_BW_041:
        bandwidthInHz = 41667UL;
        break;
    case LORA_BW_062:
        bandwidthInHz = 62500UL;
        break;
    case LORA_BW_125:
        bandwidthInHz = 125000UL;
        break;
    case LORA_BW_250:
        bandwidthInHz = 250000UL;
        break;
    case LORA_BW_500:
        bandwidthInHz = 500000UL;
        break;
    }
    return bandwidthInHz;
}

static uint32_t RadioGetGfskTimeOnAirNumerator( uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn )
{
    const RadioAddressComp_t addrComp = RADIO_ADDRESSCOMP_FILT_OFF;
    const uint8_t syncWordLength = 3;

    return ( preambleLen << 3 ) +
           ( ( fixLen == false ) ? 8 : 0 ) +
             ( syncWordLength << 3 ) +
             ( ( payloadLen +
               ( addrComp == RADIO_ADDRESSCOMP_FILT_OFF ? 0 : 1 ) +
               ( ( crcOn == true ) ? 2 : 0 ) 
               ) << 3 
             );
}

static uint32_t RadioGetLoRaTimeOnAirNumerator( uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn )
{
    int32_t crDenom           = coderate + 4;
    bool    lowDatareOptimize = false;
    int32_t ceilDenominator;
    int32_t ceilNumerator = 0;
    int32_t intermediate = 0;

    // Ensure that the preamble length is at least 12 symbols when using SF5 or
    // SF6
    if( ( datarate == 5 ) || ( datarate == 6 ) )
    {
        if( preambleLen < 12 )
        {
            preambleLen = 12;
        }
    }

    if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
        ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
    {
        lowDatareOptimize = true;
    }

    
    ceilNumerator = ( payloadLen << 3 ) +
                            ( crcOn ? 16 : 0 ) -
                            ( 4 * datarate ) +
                            ( fixLen ? 0 : 20 );

    if( datarate <= 6 )
    {
        ceilDenominator = 4 * datarate;
    }
    else
    {
        ceilNumerator += 8;

        if( lowDatareOptimize == true )
        {
            ceilDenominator = 4 * ( datarate - 2 );
        }
        else
        {
            ceilDenominator = 4 * datarate;
        }
    }

    if( ceilNumerator < 0 )
    {
        ceilNumerator = 0;
    }

    // Perform integral ceil()
    intermediate =
        ( ( ceilNumerator + ceilDenominator - 1 ) / ceilDenominator ) * crDenom + preambleLen + 12;

    if( datarate <= 6 )
    {
        intermediate += 2;
    }

    return ( uint32_t )( ( 4 * intermediate + 1 ) * ( 1 << ( datarate - 2 ) ) );
}

uint32_t RadioTimeOnAir( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn )
{
    uint32_t numerator = 0;
    uint32_t denominator = 1;

    switch( modem )
    {
    case MODEM_FSK:
        {
            numerator   = 1000U * RadioGetGfskTimeOnAirNumerator( datarate, coderate,
                                                                  preambleLen, fixLen,
                                                                  payloadLen, crcOn );
            denominator = datarate;
        }
        break;
    case MODEM_LORA:
        {
            numerator   = 1000U * RadioGetLoRaTimeOnAirNumerator( bandwidth, datarate,
                                                                  coderate, preambleLen,
                                                                  fixLen, payloadLen, crcOn );
            denominator = RadioGetLoRaBandwidthInHz( Bandwidths[bandwidth] );
        }
        break;
    }
    // Perform integral ceil()
    return ( numerator + denominator - 1 ) / denominator;
}

void RadioSend( uint8_t *buffer, uint8_t size )
{
    LLCC68SetDioIrqParams( IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( LLCC68GetPacketType( ) == PACKET_TYPE_LORA )
    {
        LLCC68.PacketParams.Params.LoRa.PayloadLength = size;
    }
    else
    {
        LLCC68.PacketParams.Params.Gfsk.PayloadLength = size;
    }
    LLCC68SetPacketParams( &LLCC68.PacketParams );

    LLCC68SendPayload( buffer, size, 0 );
    LLCC68SetTxTimerValue(TxTimeout);
    LLCC68TxTimerStart();
}

void RadioSleep( void )
{
    SleepParams_t params = { 0 };

    params.Fields.WarmStart = 1;
    LLCC68SetSleep( params );

    LLCC68DelayMs( 2 );
}

void RadioStandby( void )
{
    LLCC68SetStandby( STDBY_RC );
}

void RadioRx( uint32_t timeout )
{
    LLCC68SetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( timeout != 0 )
    {
        LLCC68SetRxTimerValue(timeout);
        LLCC68RxTimerStart();
    }

    if( RxContinuous == true )
    {
        LLCC68SetRx( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        LLCC68SetRx( RxTimeout << 6 );
    }
}

void RadioRxBoosted( uint32_t timeout )
{
    LLCC68SetDioIrqParams( IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_ALL, //IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT,
                           IRQ_RADIO_NONE,
                           IRQ_RADIO_NONE );

    if( timeout != 0 )
    {
        LLCC68SetRxTimerValue(timeout);
        LLCC68RxTimerStart();
    }

    if( RxContinuous == true )
    {
        LLCC68SetRxBoosted( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        LLCC68SetRxBoosted( RxTimeout << 6 );
    }
}

void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
    LLCC68SetRxDutyCycle( rxTime, sleepTime );
}

void RadioStartCad( void )
{
    LLCC68SetDioIrqParams( IRQ_CAD_DONE | IRQ_CAD_ACTIVITY_DETECTED, IRQ_CAD_DONE | IRQ_CAD_ACTIVITY_DETECTED, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
    LLCC68SetCad( );
}

void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{
    uint32_t timeout = ( uint32_t )time * 1000;

    LLCC68SetRfFrequency( freq );
    LLCC68SetRfTxPower( power );
    LLCC68SetTxContinuousWave( );

    LLCC68SetTxTimerValue(timeout);
    LLCC68TxTimerStart();
}

int16_t RadioRssi( RadioModems_t modem )
{
    return LLCC68GetRssiInst( );
}

void RadioWrite( uint32_t addr, uint8_t data )
{
    LLCC68WriteRegister( addr, data );
}

uint8_t RadioRead( uint32_t addr )
{
    return LLCC68ReadRegister( addr );
}

void RadioWriteBuffer( uint32_t addr, uint8_t *buffer, uint8_t size )
{
    LLCC68WriteRegisters( addr, buffer, size );
}

void RadioReadBuffer( uint32_t addr, uint8_t *buffer, uint8_t size )
{
    LLCC68ReadRegisters( addr, buffer, size );
}

void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    if( modem == MODEM_LORA )
    {
        LLCC68.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength = max;
        LLCC68SetPacketParams( &LLCC68.PacketParams );
    }
    else
    {
        if( LLCC68.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_VARIABLE_LENGTH )
        {
            LLCC68.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength = max;
            LLCC68SetPacketParams( &LLCC68.PacketParams );
        }
    }
}

void RadioSetPublicNetwork( bool enable )
{
    RadioPublicNetwork.Current = RadioPublicNetwork.Previous = enable;

    RadioSetModem( MODEM_LORA );
    if( enable == true )
    {
        // Change LoRa modem SyncWord
        LLCC68WriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PUBLIC_SYNCWORD >> 8 ) & 0xFF );
        LLCC68WriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PUBLIC_SYNCWORD & 0xFF );
    }
    else
    {
        // Change LoRa modem SyncWord
        LLCC68WriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PRIVATE_SYNCWORD >> 8 ) & 0xFF );
        LLCC68WriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PRIVATE_SYNCWORD & 0xFF );
    }
}

uint32_t RadioGetWakeupTime( void )
{
    return LLCC68GetBoardTcxoWakeupTime( ) + RADIO_WAKEUP_TIME;
}

void RadioOnTxTimeoutIrq( void* context )
{
    if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
    {
        RadioEvents->TxTimeout( );
    }
}

void RadioOnRxTimeoutIrq( void* context )
{
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
    {
        RadioEvents->RxTimeout( );
    }
}

void RadioOnDioIrq( void* context )
{
    IrqFired = true;
}

void RadioIrqProcess( void )
{
    uint16_t irqRegs = 0;
    if( IrqFired == true )
    {
        CRITICAL_SECTION_BEGIN( );
        // Clear IRQ flag
        IrqFired = false;
        CRITICAL_SECTION_END( );

        irqRegs = LLCC68GetIrqStatus( );
        LLCC68ClearIrqStatus( irqRegs );

        if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
        {
            LLCC68TxTimerStop();
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            LLCC68SetOperatingMode( MODE_STDBY_RC );
            if( ( RadioEvents != NULL ) && ( RadioEvents->TxDone != NULL ) )
            {
                RadioEvents->TxDone( );
            }
        }

        if( ( irqRegs & IRQ_RX_DONE ) == IRQ_RX_DONE )
        {
            if( ( irqRegs & IRQ_CRC_ERROR ) == IRQ_CRC_ERROR )
            {
                if( RxContinuous == false )
                {
                    //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                    LLCC68SetOperatingMode( MODE_STDBY_RC );
                }
                if( ( RadioEvents != NULL ) && ( RadioEvents->RxError ) )
                {
                    RadioEvents->RxError( );
                }
            }
            else
            {
                uint8_t size;

                LLCC68RxTimerStop();
                if( RxContinuous == false )
                {
                    //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                    LLCC68SetOperatingMode( MODE_STDBY_RC );

                    // WORKAROUND - Implicit Header Mode Timeout Behavior, see DS_SX1261-2_V1.2 datasheet chapter 15.3
                    // RegRtcControl = @address 0x0902
                    LLCC68WriteRegister( 0x0902, 0x00 );
                    // RegEventMask = @address 0x0944
                    LLCC68WriteRegister( 0x0944, LLCC68ReadRegister( 0x0944 ) | ( 1 << 1 ) );
                    // WORKAROUND END
                }
                LLCC68GetPayload( RadioRxPayload, &size , 255 );
                LLCC68GetPacketStatus( &RadioPktStatus );
                if( ( RadioEvents != NULL ) && ( RadioEvents->RxDone != NULL ) )
                {
                    RadioEvents->RxDone( RadioRxPayload, size, RadioPktStatus.Params.LoRa.RssiPkt, RadioPktStatus.Params.LoRa.SnrPkt );
                }
            }
        }

        if( ( irqRegs & IRQ_CAD_DONE ) == IRQ_CAD_DONE )
        {
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            LLCC68SetOperatingMode( MODE_STDBY_RC );
            if( ( RadioEvents != NULL ) && ( RadioEvents->CadDone != NULL ) )
            {
                RadioEvents->CadDone( ( ( irqRegs & IRQ_CAD_ACTIVITY_DETECTED ) == IRQ_CAD_ACTIVITY_DETECTED ) );
            }
        }

        if( ( irqRegs & IRQ_RX_TX_TIMEOUT ) == IRQ_RX_TX_TIMEOUT )
        {
            if( LLCC68GetOperatingMode( ) == MODE_TX )
            {
                LLCC68TxTimerStop();
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                LLCC68SetOperatingMode( MODE_STDBY_RC );
                if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
                {
                    RadioEvents->TxTimeout( );
                }
            }
            else if( LLCC68GetOperatingMode( ) == MODE_RX )
            {
                LLCC68RxTimerStop();
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                LLCC68SetOperatingMode( MODE_STDBY_RC );
                if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
                {
                    RadioEvents->RxTimeout( );
                }
            }
        }

        if( ( irqRegs & IRQ_PREAMBLE_DETECTED ) == IRQ_PREAMBLE_DETECTED )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_SYNCWORD_VALID ) == IRQ_SYNCWORD_VALID )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_HEADER_VALID ) == IRQ_HEADER_VALID )
        {
            //__NOP( );
        }

        if( ( irqRegs & IRQ_HEADER_ERROR ) == IRQ_HEADER_ERROR )
        {
            LLCC68RxTimerStop();
            if( RxContinuous == false )
            {
                //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
                LLCC68SetOperatingMode( MODE_STDBY_RC );
            }
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
            {
                RadioEvents->RxTimeout( );
            }
        }
    }
}

