/**
 * @file axk_llcc68.h
 * @brief LLCC68 LoRa 射频收发模块驱动 — 纯 ACLL 跨平台实现(两文件合并版)。
 * @author xemowo (xemowo@qq.com)
 * @version 0.1.1
 * @date 2026-08-05
 *
 * @copyright Copyright (c) 2026
 */
#ifndef __AXK_LLCC68_H__
#define __AXK_LLCC68_H__

#include <stdint.h>
#include <stdbool.h>

#include "scbb_config.h"

#ifdef SCBB_LLCC68_ENABLED

#ifdef SCBB_LLCC68_SPI_HEADER
#include SCBB_LLCC68_SPI_HEADER
#else
#include "stm32f10x_bsp_spi.h"
#endif
#ifdef SCBB_LLCC68_GPIO_HEADER
#include SCBB_LLCC68_GPIO_HEADER
#else
#include "stm32f10x_bsp_gpio.h"
#endif
#ifdef SCBB_LLCC68_DELAY_HEADER
#include SCBB_LLCC68_DELAY_HEADER
#else
#include "stm32f10x_delay.h"
#endif

/* ACLL 宏:优先使用 scbb_config.h(menuconfig 生成)的定义,否则使用默认 */
#ifndef AXK_LLCC68_SPI_ACLL
#define AXK_LLCC68_SPI_ACLL(_func, ...)   bsp_spi_##_func(__VA_ARGS__)
#endif
#ifndef AXK_LLCC68_GPIO_ACLL
#define AXK_LLCC68_GPIO_ACLL(_func, ...)  bsp_gpio_##_func(__VA_ARGS__)
#endif
/* 延时宏固定按本模块 BSP 适配(menuconfig 生成的 delay_ms 与延时 BSP 的 HAL_Delay 不一致) */
#undef AXK_LLCC68_DELAY_MS
#undef AXK_LLCC68_DELAY_US
#if defined(SCBB_LLCC68_DELAY_PREFIX) && SCBB_LLCC68_DELAY_PREFIX == bsp_delay
#define AXK_LLCC68_DELAY_MS(x)            bsp_delay_ms((x))
#define AXK_LLCC68_DELAY_US(x)            bsp_delay_us((x))
#else
#define AXK_LLCC68_DELAY_MS(x)            HAL_Delay((x))
#define AXK_LLCC68_DELAY_US(x)            HAL_Delay_us((x))
#endif

/*!
 * @brief 引脚配置(按需修改)。
 * GPIO 引脚编码:低 4 位为引脚号(0~15),高 4 位为端口号
 * (0=GPIOA, 1=GPIOB, 2=GPIOC, 3=GPIOD)。
 * 默认值对应安信可 STM32F103 参考接线:NSS=PA4, RESET=PB14,
 * BUSY=PA0, DIO1=PB1。
 */
#define AXK_LLCC68_SPI_NAME         "spi1"  /**< SPI 总线名称 */
#define AXK_LLCC68_NSS_PIN          4       /**< NSS 片选引脚(PA4) */
#define AXK_LLCC68_NRESET_PIN       0x0E    /**< 复位引脚(PB14) */
#define AXK_LLCC68_BUSY_PIN         0       /**< BUSY 忙检测引脚(PA0) */
#define AXK_LLCC68_DIO1_PIN         0x01    /**< DIO1 中断引脚(PB1) */

/*!
 * @brief 默认射频参数(按需修改)。
 */
#define AXK_LLCC68_FREQUENCY_HZ     470500000UL /**< 默认收发频率(Hz) */
#define AXK_LLCC68_TX_POWER_DBM     22          /**< 发送功率(dBm, 0~22) */
#define AXK_LLCC68_BANDWIDTH        0           /**< 带宽 [0:125k, 1:250k, 2:500k] */
#define AXK_LLCC68_SPREADING_FACTOR 9           /**< 扩频因子 (7~12) */
#define AXK_LLCC68_CODING_RATE      1           /**< 编码率 [1:4/5, 2:4/6, 3:4/7, 4:4/8] */
#define AXK_LLCC68_PREAMBLE_LENGTH  8           /**< 前导码长度 */
#define AXK_LLCC68_SYMB_TIMEOUT     0           /**< 符号超时(0 关闭) */
#define AXK_LLCC68_FIX_LEN          0           /**< 固定长度载荷: 1 开, 0 关 */
#define AXK_LLCC68_CRC_ON           1           /**< 载荷 CRC: 1 开, 0 关 */
#define AXK_LLCC68_IQ_INVERTED      0           /**< IQ 反转: 1 开, 0 关 */

/*! 临界区(SCBB 单线程模式置空) */
#define CRITICAL_SECTION_BEGIN( )
#define CRITICAL_SECTION_END( )

/* ================== Semtech 官方驱动类型与声明(原样) ================== */





#define SX1261                                      1
#define SX1262                                      2


#define RADIO_WAKEUP_TIME                           3 // [ms]


#define AUTO_RX_TX_OFFSET                           2


#define CRC_IBM_SEED                                0xFFFF


#define CRC_CCITT_SEED                              0x1D0F


#define CRC_POLYNOMIAL_IBM                          0x8005


#define CRC_POLYNOMIAL_CCITT                        0x1021


#define REG_LR_CRCSEEDBASEADDR                      0x06BC


#define REG_LR_CRCPOLYBASEADDR                      0x06BE


#define REG_LR_WHITSEEDBASEADDR_MSB                 0x06B8
#define REG_LR_WHITSEEDBASEADDR_LSB                 0x06B9


#define REG_LR_PACKETPARAMS                         0x0704


#define REG_LR_PAYLOADLENGTH                        0x0702


#define REG_LR_SYNCH_TIMEOUT                        0x0706


#define REG_LR_SYNCWORDBASEADDRESS                  0x06C0


#define REG_LR_SYNCWORD                             0x0740


#define LORA_MAC_PRIVATE_SYNCWORD                   0x1424


#define LORA_MAC_PUBLIC_SYNCWORD                    0x3444



#define RANDOM_NUMBER_GENERATORBASEADDR             0x0819


#define REG_ANA_LNA                                 0x08E2


#define REG_ANA_MIXER                               0x08E5


#define REG_RX_GAIN                                 0x08AC


#define REG_XTA_TRIM                                0x0911


#define REG_OCP                                     0x08E7


typedef union RadioStatus_u
{
    uint8_t Value;
    struct
    {   //bit order is lsb -> msb
        uint8_t           : 1;  //!< Reserved
        uint8_t CmdStatus : 3;  //!< Command status
        uint8_t ChipMode  : 3;  //!< Chip mode
        uint8_t           : 1;  //!< Reserved
    }Fields;
}RadioStatus_t;


typedef enum
{
    IRQ_HEADER_ERROR_CODE                   = 0x01,
    IRQ_SYNCWORD_ERROR_CODE                 = 0x02,
    IRQ_CRC_ERROR_CODE                      = 0x04,
}IrqErrorCode_t;

enum IrqPblSyncHeaderCode_t
{
    IRQ_PBL_DETECT_CODE                     = 0x01,
    IRQ_SYNCWORD_VALID_CODE                 = 0x02,
    IRQ_HEADER_VALID_CODE                   = 0x04,
};


typedef enum
{
    MODE_SLEEP                              = 0x00,         //! The radio is in sleep mode
    MODE_STDBY_RC,                                          //! The radio is in standby mode with RC oscillator
    MODE_STDBY_XOSC,                                        //! The radio is in standby mode with XOSC oscillator
    MODE_FS,                                                //! The radio is in frequency synthesis mode
    MODE_TX,                                                //! The radio is in transmit mode
    MODE_RX,                                                //! The radio is in receive mode
    MODE_RX_DC,                                             //! The radio is in receive duty cycle mode
    MODE_CAD                                                //! The radio is in channel activity detection mode
}RadioOperatingModes_t;


typedef enum
{
    STDBY_RC                                = 0x00,
    STDBY_XOSC                              = 0x01,
}RadioStandbyModes_t;


typedef enum
{
    USE_LDO                                 = 0x00, // default
    USE_DCDC                                = 0x01,
}RadioRegulatorMode_t;


typedef enum
{
    PACKET_TYPE_GFSK                        = 0x00,
    PACKET_TYPE_LORA                        = 0x01,
    PACKET_TYPE_NONE                        = 0x0F,
}RadioPacketTypes_t;


typedef enum
{
    RADIO_RAMP_10_US                        = 0x00,
    RADIO_RAMP_20_US                        = 0x01,
    RADIO_RAMP_40_US                        = 0x02,
    RADIO_RAMP_80_US                        = 0x03,
    RADIO_RAMP_200_US                       = 0x04,
    RADIO_RAMP_800_US                       = 0x05,
    RADIO_RAMP_1700_US                      = 0x06,
    RADIO_RAMP_3400_US                      = 0x07,
}RadioRampTimes_t;


typedef enum
{
    LORA_CAD_01_SYMBOL                      = 0x00,
    LORA_CAD_02_SYMBOL                      = 0x01,
    LORA_CAD_04_SYMBOL                      = 0x02,
    LORA_CAD_08_SYMBOL                      = 0x03,
    LORA_CAD_16_SYMBOL                      = 0x04,
}RadioLoRaCadSymbols_t;


typedef enum
{
    LORA_CAD_ONLY                           = 0x00,
    LORA_CAD_RX                             = 0x01,
    LORA_CAD_LBT                            = 0x10,
}RadioCadExitModes_t;


typedef enum
{
    MOD_SHAPING_OFF                         = 0x00,
    MOD_SHAPING_G_BT_03                     = 0x08,
    MOD_SHAPING_G_BT_05                     = 0x09,
    MOD_SHAPING_G_BT_07                     = 0x0A,
    MOD_SHAPING_G_BT_1                      = 0x0B,
}RadioModShapings_t;


typedef enum
{
    RX_BW_4800                              = 0x1F,
    RX_BW_5800                              = 0x17,
    RX_BW_7300                              = 0x0F,
    RX_BW_9700                              = 0x1E,
    RX_BW_11700                             = 0x16,
    RX_BW_14600                             = 0x0E,
    RX_BW_19500                             = 0x1D,
    RX_BW_23400                             = 0x15,
    RX_BW_29300                             = 0x0D,
    RX_BW_39000                             = 0x1C,
    RX_BW_46900                             = 0x14,
    RX_BW_58600                             = 0x0C,
    RX_BW_78200                             = 0x1B,
    RX_BW_93800                             = 0x13,
    RX_BW_117300                            = 0x0B,
    RX_BW_156200                            = 0x1A,
    RX_BW_187200                            = 0x12,
    RX_BW_234300                            = 0x0A,
    RX_BW_312000                            = 0x19,
    RX_BW_373600                            = 0x11,
    RX_BW_467000                            = 0x09,
}RadioRxBandwidth_t;


typedef enum
{
    LORA_SF5                                = 0x05,
    LORA_SF6                                = 0x06,
    LORA_SF7                                = 0x07,
    LORA_SF8                                = 0x08,
    LORA_SF9                                = 0x09,
    LORA_SF10                               = 0x0A,
    LORA_SF11                               = 0x0B,
    LORA_SF12                               = 0x0C,
}RadioLoRaSpreadingFactors_t;


typedef enum
{
    LORA_BW_500                             = 6,
    LORA_BW_250                             = 5,
    LORA_BW_125                             = 4,
    LORA_BW_062                             = 3,
    LORA_BW_041                             = 10,
    LORA_BW_031                             = 2,
    LORA_BW_020                             = 9,
    LORA_BW_015                             = 1,
    LORA_BW_010                             = 8,
    LORA_BW_007                             = 0,
}RadioLoRaBandwidths_t;


typedef enum
{
    LORA_CR_4_5                             = 0x01,
    LORA_CR_4_6                             = 0x02,
    LORA_CR_4_7                             = 0x03,
    LORA_CR_4_8                             = 0x04,
}RadioLoRaCodingRates_t;


typedef enum
{
    RADIO_PREAMBLE_DETECTOR_OFF             = 0x00,         //!< Preamble detection length off
    RADIO_PREAMBLE_DETECTOR_08_BITS         = 0x04,         //!< Preamble detection length 8 bits
    RADIO_PREAMBLE_DETECTOR_16_BITS         = 0x05,         //!< Preamble detection length 16 bits
    RADIO_PREAMBLE_DETECTOR_24_BITS         = 0x06,         //!< Preamble detection length 24 bits
    RADIO_PREAMBLE_DETECTOR_32_BITS         = 0x07,         //!< Preamble detection length 32 bit
}RadioPreambleDetection_t;


typedef enum
{
    RADIO_ADDRESSCOMP_FILT_OFF              = 0x00,         //!< No correlator turned on, i.e. do not search for SyncWord
    RADIO_ADDRESSCOMP_FILT_NODE             = 0x01,
    RADIO_ADDRESSCOMP_FILT_NODE_BROAD       = 0x02,
}RadioAddressComp_t;


typedef enum
{
    RADIO_PACKET_FIXED_LENGTH               = 0x00,         //!< The packet is known on both sides, no header included in the packet
    RADIO_PACKET_VARIABLE_LENGTH            = 0x01,         //!< The packet is on variable size, header included
}RadioPacketLengthModes_t;


typedef enum
{
    RADIO_CRC_OFF                           = 0x01,         //!< No CRC in use
    RADIO_CRC_1_BYTES                       = 0x00,
    RADIO_CRC_2_BYTES                       = 0x02,
    RADIO_CRC_1_BYTES_INV                   = 0x04,
    RADIO_CRC_2_BYTES_INV                   = 0x06,
    RADIO_CRC_2_BYTES_IBM                   = 0xF1,
    RADIO_CRC_2_BYTES_CCIT                  = 0xF2,
}RadioCrcTypes_t;


typedef enum
{
    RADIO_DC_FREE_OFF                       = 0x00,
    RADIO_DC_FREEWHITENING                  = 0x01,
}RadioDcFree_t;


typedef enum
{
    LORA_PACKET_VARIABLE_LENGTH             = 0x00,         //!< The packet is on variable size, header included
    LORA_PACKET_FIXED_LENGTH                = 0x01,         //!< The packet is known on both sides, no header included in the packet
    LORA_PACKET_EXPLICIT                    = LORA_PACKET_VARIABLE_LENGTH,
    LORA_PACKET_IMPLICIT                    = LORA_PACKET_FIXED_LENGTH,
}RadioLoRaPacketLengthsMode_t;


typedef enum
{
    LORA_CRC_ON                             = 0x01,         //!< CRC activated
    LORA_CRC_OFF                            = 0x00,         //!< CRC not used
}RadioLoRaCrcModes_t;


typedef enum
{
    LORA_IQ_NORMAL                          = 0x00,
    LORA_IQ_INVERTED                        = 0x01,
}RadioLoRaIQModes_t;


typedef enum
{
    TCXO_CTRL_1_6V                          = 0x00,
    TCXO_CTRL_1_7V                          = 0x01,
    TCXO_CTRL_1_8V                          = 0x02,
    TCXO_CTRL_2_2V                          = 0x03,
    TCXO_CTRL_2_4V                          = 0x04,
    TCXO_CTRL_2_7V                          = 0x05,
    TCXO_CTRL_3_0V                          = 0x06,
    TCXO_CTRL_3_3V                          = 0x07,
}RadioTcxoCtrlVoltage_t;


typedef enum
{
    IRQ_RADIO_NONE                          = 0x0000,
    IRQ_TX_DONE                             = 0x0001,
    IRQ_RX_DONE                             = 0x0002,
    IRQ_PREAMBLE_DETECTED                   = 0x0004,
    IRQ_SYNCWORD_VALID                      = 0x0008,
    IRQ_HEADER_VALID                        = 0x0010,
    IRQ_HEADER_ERROR                        = 0x0020,
    IRQ_CRC_ERROR                           = 0x0040,
    IRQ_CAD_DONE                            = 0x0080,
    IRQ_CAD_ACTIVITY_DETECTED               = 0x0100,
    IRQ_RX_TX_TIMEOUT                       = 0x0200,
    IRQ_RADIO_ALL                           = 0xFFFF,
}RadioIrqMasks_t;


typedef enum RadioCommands_e
{
    RADIO_GET_STATUS                        = 0xC0,
    RADIO_WRITE_REGISTER                    = 0x0D,
    RADIO_READ_REGISTER                     = 0x1D,
    RADIO_WRITE_BUFFER                      = 0x0E,
    RADIO_READ_BUFFER                       = 0x1E,
    RADIO_SET_SLEEP                         = 0x84,
    RADIO_SET_STANDBY                       = 0x80,
    RADIO_SET_FS                            = 0xC1,
    RADIO_SET_TX                            = 0x83,
    RADIO_SET_RX                            = 0x82,
    RADIO_SET_RXDUTYCYCLE                   = 0x94,
    RADIO_SET_CAD                           = 0xC5,
    RADIO_SET_TXCONTINUOUSWAVE              = 0xD1,
    RADIO_SET_TXCONTINUOUSPREAMBLE          = 0xD2,
    RADIO_SET_PACKETTYPE                    = 0x8A,
    RADIO_GET_PACKETTYPE                    = 0x11,
    RADIO_SET_RFFREQUENCY                   = 0x86,
    RADIO_SET_TXPARAMS                      = 0x8E,
    RADIO_SET_PACONFIG                      = 0x95,
    RADIO_SET_CADPARAMS                     = 0x88,
    RADIO_SET_BUFFERBASEADDRESS             = 0x8F,
    RADIO_SET_MODULATIONPARAMS              = 0x8B,
    RADIO_SET_PACKETPARAMS                  = 0x8C,
    RADIO_GET_RXBUFFERSTATUS                = 0x13,
    RADIO_GET_PACKETSTATUS                  = 0x14,
    RADIO_GET_RSSIINST                      = 0x15,
    RADIO_GET_STATS                         = 0x10,
    RADIO_RESET_STATS                       = 0x00,
    RADIO_CFG_DIOIRQ                        = 0x08,
    RADIO_GET_IRQSTATUS                     = 0x12,
    RADIO_CLR_IRQSTATUS                     = 0x02,
    RADIO_CALIBRATE                         = 0x89,
    RADIO_CALIBRATEIMAGE                    = 0x98,
    RADIO_SET_REGULATORMODE                 = 0x96,
    RADIO_GET_ERROR                         = 0x17,
    RADIO_CLR_ERROR                         = 0x07,
    RADIO_SET_TCXOMODE                      = 0x97,
    RADIO_SET_TXFALLBACKMODE                = 0x93,
    RADIO_SET_RFSWITCHMODE                  = 0x9D,
    RADIO_SET_STOPRXTIMERONPREAMBLE         = 0x9F,
    RADIO_SET_LORASYMBTIMEOUT               = 0xA0,
}RadioCommands_t;


typedef struct
{
    RadioPacketTypes_t                   PacketType;        //!< Packet to which the modulation parameters are referring to.
    struct
    {
        struct
        {
            uint32_t                     BitRate;
            uint32_t                     Fdev;
            RadioModShapings_t           ModulationShaping;
            uint8_t                      Bandwidth;
        }Gfsk;
        struct
        {
            RadioLoRaSpreadingFactors_t  SpreadingFactor;   //!< Spreading Factor for the LoRa modulation
            RadioLoRaBandwidths_t        Bandwidth;         //!< Bandwidth for the LoRa modulation
            RadioLoRaCodingRates_t       CodingRate;        //!< Coding rate for the LoRa modulation
            uint8_t                      LowDatarateOptimize; //!< Indicates if the modem uses the low datarate optimization
        }LoRa;
    }Params;                                                //!< Holds the modulation parameters structure
}ModulationParams_t;


typedef struct
{
    RadioPacketTypes_t                    PacketType;        //!< Packet to which the packet parameters are referring to.
    struct
    {
        
        struct
        {
            uint16_t                     PreambleLength;    //!< The preamble Tx length for GFSK packet type in bit
            RadioPreambleDetection_t     PreambleMinDetect; //!< The preamble Rx length minimal for GFSK packet type
            uint8_t                      SyncWordLength;    //!< The synchronization word length for GFSK packet type
            RadioAddressComp_t           AddrComp;          //!< Activated SyncWord correlators
            RadioPacketLengthModes_t     HeaderType;        //!< If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted
            uint8_t                      PayloadLength;     //!< Size of the payload in the GFSK packet
            RadioCrcTypes_t              CrcLength;         //!< Size of the CRC block in the GFSK packet
            RadioDcFree_t                DcFree;
        }Gfsk;
        
        struct
        {
            uint16_t                     PreambleLength;    //!< The preamble length is the number of LoRa symbols in the preamble
            RadioLoRaPacketLengthsMode_t HeaderType;        //!< If the header is explicit, it will be transmitted in the LoRa packet. If the header is implicit, it will not be transmitted
            uint8_t                      PayloadLength;     //!< Size of the payload in the LoRa packet
            RadioLoRaCrcModes_t          CrcMode;           //!< Size of CRC block in LoRa packet
            RadioLoRaIQModes_t           InvertIQ;          //!< Allows to swap IQ for LoRa packet
        }LoRa;
    }Params;                                                //!< Holds the packet parameters structure
}PacketParams_t;


typedef struct
{
    RadioPacketTypes_t                    packetType;      //!< Packet to which the packet status are referring to.
    struct
    {
        struct
        {
            uint8_t RxStatus;
            int8_t RssiAvg;                                //!< The averaged RSSI
            int8_t RssiSync;                               //!< The RSSI measured on last packet
            uint32_t FreqError;
        }Gfsk;
        struct
        {
            int8_t RssiPkt;                                //!< The RSSI of the last packet
            int8_t SnrPkt;                                 //!< The SNR of the last packet
            int8_t SignalRssiPkt;
            uint32_t FreqError;
        }LoRa;
    }Params;
}PacketStatus_t;


typedef struct
{
    RadioPacketTypes_t                    packetType;       //!< Packet to which the packet status are referring to.
    uint16_t PacketReceived;
    uint16_t CrcOk;
    uint16_t LengthError;
}RxCounter_t;


typedef union
{
    struct
    {
        uint8_t RC64KEnable    : 1;                             //!< Calibrate RC64K clock
        uint8_t RC13MEnable    : 1;                             //!< Calibrate RC13M clock
        uint8_t PLLEnable      : 1;                             //!< Calibrate PLL
        uint8_t ADCPulseEnable : 1;                             //!< Calibrate ADC Pulse
        uint8_t ADCBulkNEnable : 1;                             //!< Calibrate ADC bulkN
        uint8_t ADCBulkPEnable : 1;                             //!< Calibrate ADC bulkP
        uint8_t ImgEnable      : 1;
        uint8_t                : 1;
    }Fields;
    uint8_t Value;
}CalibrationParams_t;


typedef union
{
    struct
    {
        uint8_t WakeUpRTC               : 1;                    //!< Get out of sleep mode if wakeup signal received from RTC
        uint8_t Reset                   : 1;
        uint8_t WarmStart               : 1;
        uint8_t Reserved                : 5;
    }Fields;
    uint8_t Value;
}SleepParams_t;


typedef union
{
    struct
    {
        uint8_t Rc64kCalib              : 1;                    //!< RC 64kHz oscillator calibration failed
        uint8_t Rc13mCalib              : 1;                    //!< RC 13MHz oscillator calibration failed
        uint8_t PllCalib                : 1;                    //!< PLL calibration failed
        uint8_t AdcCalib                : 1;                    //!< ADC calibration failed
        uint8_t ImgCalib                : 1;                    //!< Image calibration failed
        uint8_t XoscStart               : 1;                    //!< XOSC oscillator failed to start
        uint8_t PllLock                 : 1;                    //!< PLL lock failed
        uint8_t                         : 1;                    //!< Buck converter failed to start
        uint8_t PaRamp                  : 1;                    //!< PA ramp failed
        uint8_t                         : 7;                    //!< Reserved
    }Fields;
    uint16_t Value;
}RadioError_t;


typedef struct LLCC68_s
{
    PacketParams_t PacketParams;
    PacketStatus_t PacketStatus;
    ModulationParams_t ModulationParams;
}LLCC68_t;


typedef void ( DioIrqHandler )( void* context );




typedef struct
{
    void ( *txDone )( void );                       //!< Pointer to a function run on successful transmission
    void ( *rxDone )( void );                       //!< Pointer to a function run on successful reception
    void ( *rxPreambleDetect )( void );             //!< Pointer to a function run on successful Preamble detection
    void ( *rxSyncWordDone )( void );               //!< Pointer to a function run on successful SyncWord reception
    void ( *rxHeaderDone )( bool isOk );            //!< Pointer to a function run on successful Header reception
    void ( *txTimeout )( void );                    //!< Pointer to a function run on transmission timeout
    void ( *rxTimeout )( void );                    //!< Pointer to a function run on reception timeout
    void ( *rxError )( IrqErrorCode_t errCode );    //!< Pointer to a function run on reception error
    void ( *cadDone )( bool cadFlag );              //!< Pointer to a function run on channel activity detected
}LLCC68Callbacks_t;


 

void LLCC68Init( DioIrqHandler dioIrq );


void LLCC68CheckDeviceReady( void );


void LLCC68SetPayload( uint8_t *payload, uint8_t size );


uint8_t LLCC68GetPayload( uint8_t *payload, uint8_t *size, uint8_t maxSize );


void LLCC68SendPayload( uint8_t *payload, uint8_t size, uint32_t timeout );


uint8_t LLCC68SetSyncWord( uint8_t *syncWord );


void LLCC68SetCrcSeed( uint16_t seed );


void LLCC68SetCrcPolynomial( uint16_t polynomial );


void LLCC68SetWhiteningSeed( uint16_t seed );


uint32_t LLCC68GetRandom( void );


void LLCC68SetSleep( SleepParams_t sleepConfig );


void LLCC68SetStandby( RadioStandbyModes_t mode );


void LLCC68SetFs( void );


void LLCC68SetTx( uint32_t timeout );


void LLCC68SetRx( uint32_t timeout );


void LLCC68SetRxBoosted( uint32_t timeout );


void LLCC68SetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime );


void LLCC68SetCad( void );


void LLCC68SetTxContinuousWave( void );


void LLCC68SetTxInfinitePreamble( void );


void LLCC68SetStopRxTimerOnPreambleDetect( bool enable );


void LLCC68SetLoRaSymbNumTimeout( uint8_t SymbNum );


void LLCC68SetRegulatorMode( RadioRegulatorMode_t mode );


void LLCC68Calibrate( CalibrationParams_t calibParam );


void LLCC68CalibrateImage( uint32_t freq );


void LLCC68SetLongPreamble( uint8_t enable );


void LLCC68SetPaConfig( uint8_t paDutyCycle, uint8_t hpMax, uint8_t deviceSel, uint8_t paLut );


void LLCC68SetRxTxFallbackMode( uint8_t fallbackMode );


void LLCC68WriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size );


void LLCC68ReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size );


void LLCC68WriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size );


void LLCC68ReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size );


void LLCC68SetDioIrqParams( uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask );


uint16_t LLCC68GetIrqStatus( void );


void LLCC68SetDio2AsRfSwitchCtrl( uint8_t enable );


void LLCC68SetDio3AsTcxoCtrl( RadioTcxoCtrlVoltage_t tcxoVoltage, uint32_t timeout );


void LLCC68SetRfFrequency( uint32_t frequency );


void LLCC68SetPacketType( RadioPacketTypes_t packetType );


RadioPacketTypes_t LLCC68GetPacketType( void );


void LLCC68SetTxParams( int8_t power, RadioRampTimes_t rampTime );


void LLCC68SetModulationParams( ModulationParams_t *modParams );


void LLCC68SetPacketParams( PacketParams_t *packetParams );


void LLCC68SetCadParams( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, RadioCadExitModes_t cadExitMode, uint32_t cadTimeout );


void LLCC68SetBufferBaseAddress( uint8_t txBaseAddress, uint8_t rxBaseAddress );


RadioStatus_t LLCC68GetStatus( void );


int8_t LLCC68GetRssiInst( void );


void LLCC68GetRxBufferStatus( uint8_t *payloadLength, uint8_t *rxStartBuffer );


void LLCC68GetPacketStatus( PacketStatus_t *pktStatus );


RadioError_t LLCC68GetDeviceErrors( void );


void LLCC68ClearDeviceErrors( void );


void LLCC68ClearIrqStatus( uint16_t irq );









typedef enum
{
    MODEM_FSK = 0,
    MODEM_LORA,
}RadioModems_t;


typedef enum
{
    RF_IDLE = 0,   //!< The radio is idle
    RF_RX_RUNNING, //!< The radio is in reception state
    RF_TX_RUNNING, //!< The radio is in transmission state
    RF_CAD,        //!< The radio is doing channel activity detection
}RadioState_t;


typedef struct
{
    
    void    ( *TxDone )( void );
    
    void    ( *TxTimeout )( void );
    
    void    ( *RxDone )( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );
    
    void    ( *RxTimeout )( void );
    
    void    ( *RxError )( void );
    
    void ( *FhssChangeChannel )( uint8_t currentChannel );

    
    void ( *CadDone ) ( bool channelActivityDetected );
    
    
    void    ( *GnssDone )( void );
    
    
    void    ( *WifiDone )( void );
}RadioEvents_t;


struct Radio_s
{
    
    void    ( *Init )( RadioEvents_t *events );
    
    RadioState_t ( *GetStatus )( void );
    
    void    ( *SetModem )( RadioModems_t modem );
    
    void    ( *SetChannel )( uint32_t freq );
    
    bool    ( *IsChannelFree )( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime );
    
    uint32_t ( *Random )( void );
    
    void    ( *SetRxConfig )( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint32_t bandwidthAfc, uint16_t preambleLen,
                              uint16_t symbTimeout, bool fixLen,
                              uint8_t payloadLen,
                              bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                              bool iqInverted, bool rxContinuous );
    
    void    ( *SetTxConfig )( RadioModems_t modem, int8_t power, uint32_t fdev,
                              uint32_t bandwidth, uint32_t datarate,
                              uint8_t coderate, uint16_t preambleLen,
                              bool fixLen, bool crcOn, bool freqHopOn,
                              uint8_t hopPeriod, bool iqInverted, uint32_t timeout );
    
    bool    ( *CheckRfFrequency )( uint32_t frequency );
    
    uint32_t  ( *TimeOnAir )( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn );
    
    void    ( *Send )( uint8_t *buffer, uint8_t size );
    
    void    ( *Sleep )( void );
    
    void    ( *Standby )( void );
    
    void    ( *Rx )( uint32_t timeout );
    
    void    ( *StartCad )( void );
    
    void    ( *SetTxContinuousWave )( uint32_t freq, int8_t power, uint16_t time );
    
    int16_t ( *Rssi )( RadioModems_t modem );
    
    void    ( *Write )( uint32_t addr, uint8_t data );
    
    uint8_t ( *Read )( uint32_t addr );
    
    void    ( *WriteBuffer )( uint32_t addr, uint8_t *buffer, uint8_t size );
    
    void    ( *ReadBuffer )( uint32_t addr, uint8_t *buffer, uint8_t size );
    
    void    ( *SetMaxPayloadLength )( RadioModems_t modem, uint8_t max );
    
    void    ( *SetPublicNetwork )( bool enable );
    
    uint32_t  ( *GetWakeupTime )( void );
    
    void ( *IrqProcess )( void );
    
    
    void    ( *RxBoosted )( uint32_t timeout );
    
    void ( *SetRxDutyCycle ) ( uint32_t rxTime, uint32_t sleepTime );
};


RadioOperatingModes_t LLCC68GetOperatingMode( void );


void LLCC68SetOperatingMode( RadioOperatingModes_t mode );


void LLCC68IoTcxoInit( void );


uint32_t LLCC68GetBoardTcxoWakeupTime( void );


void LLCC68IoRfSwitchInit( void );


void LLCC68AntSwOn( void );


void LLCC68AntSwOff( void );


uint8_t LLCC68GetDeviceId( void );


void LLCC68SetRfTxPower( int8_t power );


void LLCC68WriteRegister( uint16_t address, uint8_t value );


uint8_t LLCC68ReadRegister( uint16_t address );


void LLCC68Wakeup( void );


void LLCC68WriteCommand( RadioCommands_t opcode, uint8_t *buffer, uint16_t size );


uint8_t LLCC68ReadCommand( RadioCommands_t opcode, uint8_t *buffer, uint16_t size );


void RadioOnTxTimeoutIrq( void* context );


void RadioOnRxTimeoutIrq( void* context );






/*! 事件回调结构(与 Semtech RadioEvents_t 一致) */
typedef RadioEvents_t axk_llcc68_events_t;

/*! Radio 驱动实例(定义在 axk_llcc68.c) */
extern const struct Radio_s Radio;

/* ================== 板级原语声明(由 axk_llcc68.c 实现) ================== */

void LLCC68IoInit(void);
void LLCC68IoIrqInit(DioIrqHandler dioIrq);
void LLCC68ResetInit(void);
void LLCC68WaitOnBusy(void);
void LLCC68SetNss(uint8_t lev);
uint8_t LLCC68SpiInOut(uint8_t data);
void LLCC68DelayMs(uint32_t ms);
bool LLCC68CheckRfFrequency(uint32_t frequency);
void LLCC68TimerInit(void);
void LLCC68SetTxTimerValue(uint32_t value);
void LLCC68TxTimerStart(void);
void LLCC68TxTimerStop(void);
void LLCC68SetRxTimerValue(uint32_t value);
void LLCC68RxTimerStart(void);
void LLCC68RxTimerStop(void);
void RadioIrqProcess(void);

/* ================== SCBB 公共 API ================== */

/**
 * @brief 初始化 LLCC68(引脚/SPI/复位/默认收发参数)
 *
 * @param[in]  events  事件回调结构指针,可为 NULL
 * @return     int     0: 成功
 */
int axk_llcc68_init(const axk_llcc68_events_t *events);

/**
 * @brief 设置射频收发频率
 *
 * @param[in]  freq_hz  频率(Hz)
 * @return     int      0: 成功
 */
int axk_llcc68_set_frequency(uint32_t freq_hz);

/**
 * @brief 非阻塞发送数据(结果通过 TxDone/TxTimeout 回调返回)
 *
 * @param[in]  payload  待发送数据缓冲区
 * @param[in]  size     数据长度(字节)
 * @return     int      0: 成功, -1: 参数无效
 */
int axk_llcc68_send(const uint8_t *payload, uint8_t size);

/**
 * @brief 进入接收模式(结果通过 RxDone/RxTimeout/RxError 回调返回)
 *
 * @param[in]  timeout_ms  接收超时时间(ms),0 表示连续接收
 * @return     int         0: 成功
 */
int axk_llcc68_start_rx(uint32_t timeout_ms);

/**
 * @brief 读取瞬时 RSSI
 *
 * @return     int   RSSI(dBm)
 */
int axk_llcc68_get_rssi(void);

/**
 * @brief DIO1 中断回调处理,在 MCU 的 DIO1 外部中断服务函数中调用
 */
void axk_llcc68_irq_cb(void);

#endif /* SCBB_LLCC68_ENABLED */
#endif /* __AXK_LLCC68_H__ */
