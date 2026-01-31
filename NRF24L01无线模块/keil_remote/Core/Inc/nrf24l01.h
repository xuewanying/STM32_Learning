#ifndef __NRF24L01_H
#define __NRF24L01_H

#include "main.h"
#include "spi.h"
#include "gpio.h"

// 寄存器地址定义
#define CONFIG      0x00
#define EN_AA       0x01
#define EN_RXADDR   0x02
#define SETUP_AW    0x03
#define SETUP_RETR  0x04
#define RF_CH       0x05
#define RF_SETUP    0x06
#define STATUS      0x07
#define OBSERVE_TX  0x08
#define CD          0x09
#define RX_ADDR_P0  0x0A
#define RX_ADDR_P1  0x0B
#define TX_ADDR     0x10
#define RX_PW_P0    0x11
#define FIFO_STATUS 0x17

// SPI命令
#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define R_RX_PAYLOAD  0x61
#define W_TX_PAYLOAD  0xA0
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define R_RX_PL_WID   0x60
#define NOP           0xFF

// 状态位
#define RX_DR   0x40
#define TX_DS   0x20
#define MAX_RT  0x10

// 地址宽度
#define TX_ADR_WIDTH  5
#define RX_ADR_WIDTH  5
#define TX_PLOAD_WIDTH 32
#define RX_PLOAD_WIDTH 32

// 引脚定义
#define NRF_CE_PIN     GPIO_PIN_11
#define NRF_CE_PORT    GPIOA
#define NRF_CSN_PIN    GPIO_PIN_12
#define NRF_CSN_PORT   GPIOB
#define NRF_IRQ_PIN    GPIO_PIN_8
#define NRF_IRQ_PORT   GPIOA

// 操作宏
#define NRF_CE_LOW()   HAL_GPIO_WritePin(NRF_CE_PORT, NRF_CE_PIN, GPIO_PIN_RESET)
#define NRF_CE_HIGH()  HAL_GPIO_WritePin(NRF_CE_PORT, NRF_CE_PIN, GPIO_PIN_SET)
#define NRF_CSN_LOW()  HAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_RESET)
#define NRF_CSN_HIGH() HAL_GPIO_WritePin(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_PIN_SET)
#define NRF_IRQ_CHECK() (HAL_GPIO_ReadPin(NRF_IRQ_PORT, NRF_IRQ_PIN) == GPIO_PIN_RESET)

// 函数声明
uint8_t NRF_SPI_Transfer(uint8_t data);
uint8_t NRF_Init(void);
uint8_t NRF_Check(void);
void NRF_TX_Mode(void);
void NRF_RX_Mode(void);
uint8_t NRF_SendData(uint8_t *data, uint8_t len);
uint8_t NRF_ReceiveData(uint8_t *data);
void NRF_FlushTX(void);
void NRF_FlushRX(void);
uint8_t NRF_ReadStatus(void);
uint8_t NRF_ReadReg(uint8_t reg);
uint8_t NRF_WriteReg(uint8_t reg, uint8_t value);

#endif