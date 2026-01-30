#ifndef __NRF24L01_H
#define __NRF24L01_H

#include <stdint.h>
#include "main.h"

// NRF24L01寄存器地址
#define CONFIG      0x00  // 配置寄存器
#define EN_AA       0x01  // 自动应答功能设置
#define EN_RXADDR   0x02  // 接收地址允许
#define SETUP_AW    0x03  // 地址宽度设置
#define SETUP_RETR  0x04  // 自动重发设置
#define RF_CH       0x05  // 射频频率设置
#define RF_SETUP    0x06  // 射频寄存器
#define STATUS      0x07  // 状态寄存器
#define OBSERVE_TX  0x08  // 发送检测
#define CD          0x09  // 载波检测
#define RX_ADDR_P0  0x0A  // 数据通道0接收地址
#define RX_ADDR_P1  0x0B  // 数据通道1接收地址
#define RX_ADDR_P2  0x0C  // 数据通道2接收地址
#define RX_ADDR_P3  0x0D  // 数据通道3接收地址
#define RX_ADDR_P4  0x0E  // 数据通道4接收地址
#define RX_ADDR_P5  0x0F  // 数据通道5接收地址
#define TX_ADDR     0x10  // 发送地址
#define RX_PW_P0    0x11  // 接收数据通道0有效数据宽度
#define RX_PW_P1    0x12  // 接收数据通道1有效数据宽度
#define RX_PW_P2    0x13  // 接收数据通道2有效数据宽度
#define RX_PW_P3    0x14  // 接收数据通道3有效数据宽度
#define RX_PW_P4    0x15  // 接收数据通道4有效数据宽度
#define RX_PW_P5    0x16  // 接收数据通道5有效数据宽度
#define FIFO_STATUS 0x17  // FIFO状态

// SPI命令
#define R_REGISTER    0x00  // 读寄存器
#define W_REGISTER    0x20  // 写寄存器
#define R_RX_PAYLOAD  0x61  // 读RX有效数据
#define W_TX_PAYLOAD  0xA0  // 写TX有效数据
#define FLUSH_TX      0xE1  // 清除TX FIFO
#define FLUSH_RX      0xE2  // 清除RX FIFO
#define REUSE_TX_PL   0xE3  // 重用TX有效数据
#define R_RX_PL_WID   0x60  // 读RX有效数据宽度
#define W_ACK_PAYLOAD 0xA8  // 写ACK有效数据
#define W_TX_PAYLOAD_NOACK 0xB0  // 写TX有效数据，无ACK
#define NOP           0xFF  // 空操作

// 状态寄存器位
#define RX_DR        6     // 接收数据准备好
#define TX_DS        5     // 数据发送完成
#define MAX_RT       4     // 达到最大重发次数

// 射频速率
#define RF_DR_250KBPS 0
#define RF_DR_1MBPS   1
#define RF_DR_2MBPS   2

// 发射功率
#define RF_PWR_18DBM  0  // -18dBm
#define RF_PWR_12DBM  1  // -12dBm
#define RF_PWR_6DBM   2  // -6dBm
#define RF_PWR_0DBM   3  // 0dBm

// 工作模式
#define MODE_RX       0
#define MODE_TX       1
#define MODE_POWER_DOWN 2
#define MODE_STANDBY  3

// 配置结构体
typedef struct {
    uint8_t channel;       // 频道(0-125)
    uint8_t data_rate;     // 数据速率(RF_DR_250KBPS, RF_DR_1MBPS, RF_DR_2MBPS)
    uint8_t tx_power;      // 发射功率(RF_PWR_18DBM, RF_PWR_12DBM, RF_PWR_6DBM, RF_PWR_0DBM)
    uint8_t crc_length;    // CRC长度(1或2字节)
    uint8_t retry_delay;   // 重发延迟(0-15, 250us * delay + 250us)
    uint8_t retry_count;   // 重发次数(0-15)
    uint8_t addr_width;    // 地址宽度(3-5)
} nrf24_config_t;

// 引脚控制函数 - 需要根据你的硬件实现
void NRF24_CSN_LOW(void);
void NRF24_CSN_HIGH(void);
void NRF24_CE_LOW(void);
void NRF24_CE_HIGH(void);

// SPI函数 - 需要实现
uint8_t SPI_TransmitReceive(uint8_t data);

// NRF24L01驱动函数
void NRF24_Init(void);
uint8_t NRF24_ReadRegister(uint8_t reg);
void NRF24_WriteRegister(uint8_t reg, uint8_t value);
void NRF24_ReadRegisters(uint8_t reg, uint8_t *data, uint8_t length);
void NRF24_WriteRegisters(uint8_t reg, uint8_t *data, uint8_t length);
void NRF24_FlushRx(void);
void NRF24_FlushTx(void);
uint8_t NRF24_GetStatus(void);
void NRF24_SetConfig(nrf24_config_t *config);
void NRF24_SetMode(uint8_t mode);
void NRF24_SetTxAddress(uint8_t *addr);
void NRF24_SetRxAddress(uint8_t *addr, uint8_t pipe);
uint8_t NRF24_Transmit(uint8_t *data, uint8_t length);
uint8_t NRF24_Receive(uint8_t *data, uint8_t max_length);
uint8_t NRF24_TestConnection(void);

#endif /* __NRF24L01_H */