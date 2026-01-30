#include "nrf24l01.h"
#include <stdio.h>  // 用于printf

// 默认地址（根据你的需要修改）
static uint8_t tx_address[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
static uint8_t rx_address[5] = {0xD7, 0xD7, 0xD7, 0xD7, 0xD7};

// 引脚控制函数 - 需要根据你的硬件平台实现
void NRF24_CSN_LOW(void) {
    // 实现CSN引脚置低
    // 例如：GPIO_WritePin(NRF24_CSN_Port, NRF24_CSN_Pin, GPIO_PIN_RESET);
}

void NRF24_CSN_HIGH(void) {
    // 实现CSN引脚置高
}

void NRF24_CE_LOW(void) {
    // 实现CE引脚置低
}

void NRF24_CE_HIGH(void) {
    // 实现CE引脚置高
}

// SPI发送接收函数 - 需要根据你的硬件实现
uint8_t SPI_TransmitReceive(uint8_t data) {
    // 实现SPI数据传输
    // 例如：return HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, 1000);
    return 0xFF; // 临时返回值
}

// 读取寄存器
uint8_t NRF24_ReadRegister(uint8_t reg) {
    uint8_t value = 0;
    
    NRF24_CSN_LOW();
    SPI_TransmitReceive(R_REGISTER | reg);
    value = SPI_TransmitReceive(NOP);
    NRF24_CSN_HIGH();
    
    return value;
}

// 写寄存器
void NRF24_WriteRegister(uint8_t reg, uint8_t value) {
    NRF24_CSN_LOW();
    SPI_TransmitReceive(W_REGISTER | reg);
    SPI_TransmitReceive(value);
    NRF24_CSN_HIGH();
}

// 读取多个寄存器
void NRF24_ReadRegisters(uint8_t reg, uint8_t *data, uint8_t length) {
    NRF24_CSN_LOW();
    SPI_TransmitReceive(R_REGISTER | reg);
    
    for(uint8_t i = 0; i < length; i++) {
        data[i] = SPI_TransmitReceive(NOP);
    }
    
    NRF24_CSN_HIGH();
}

// 写多个寄存器
void NRF24_WriteRegisters(uint8_t reg, uint8_t *data, uint8_t length) {
    NRF24_CSN_LOW();
    SPI_TransmitReceive(W_REGISTER | reg);
    
    for(uint8_t i = 0; i < length; i++) {
        SPI_TransmitReceive(data[i]);
    }
    
    NRF24_CSN_HIGH();
}

// 清空RX FIFO
void NRF24_FlushRx(void) {
    NRF24_CSN_LOW();
    SPI_TransmitReceive(FLUSH_RX);
    NRF24_CSN_HIGH();
}

// 清空TX FIFO
void NRF24_FlushTx(void) {
    NRF24_CSN_LOW();
    SPI_TransmitReceive(FLUSH_TX);
    NRF24_CSN_HIGH();
}

// 获取状态
uint8_t NRF24_GetStatus(void) {
    NRF24_CSN_LOW();
    uint8_t status = SPI_TransmitReceive(NOP);
    NRF24_CSN_HIGH();
    return status;
}

// 设置配置
void NRF24_SetConfig(nrf24_config_t *config) {
    // 设置地址宽度
    NRF24_WriteRegister(SETUP_AW, config->addr_width - 2);
    
    // 设置射频
    uint8_t rf_setup = 0;
    rf_setup = (config->tx_power << 1) | (config->data_rate << 3);
    NRF24_WriteRegister(RF_SETUP, rf_setup);
    
    // 设置频道
    NRF24_WriteRegister(RF_CH, config->channel);
    
    // 设置自动重发
    NRF24_WriteRegister(SETUP_RETR, (config->retry_delay << 4) | config->retry_count);
    
    // 启用CRC，设置CRC长度
    uint8_t config_reg = NRF24_ReadRegister(CONFIG);
    config_reg |= (1 << 3); // 启用CRC
    if(config->crc_length == 2) {
        config_reg |= (1 << 2); // 2字节CRC
    } else {
        config_reg &= ~(1 << 2); // 1字节CRC
    }
    NRF24_WriteRegister(CONFIG, config_reg);
}

// 设置工作模式
void NRF24_SetMode(uint8_t mode) {
    uint8_t config_reg = NRF24_ReadRegister(CONFIG);
    
    switch(mode) {
        case MODE_RX:
            config_reg |= (1 << 0);  // 设置为接收模式
            NRF24_WriteRegister(CONFIG, config_reg);
            NRF24_CE_HIGH();
            break;
            
        case MODE_TX:
            config_reg &= ~(1 << 0); // 设置为发送模式
            NRF24_WriteRegister(CONFIG, config_reg);
            NRF24_CE_LOW();
            break;
            
        case MODE_POWER_DOWN:
            NRF24_CE_LOW();
            config_reg &= ~(1 << 1); // 掉电模式
            NRF24_WriteRegister(CONFIG, config_reg);
            break;
            
        case MODE_STANDBY:
            NRF24_CE_LOW();
            config_reg |= (1 << 1); // 待机模式
            NRF24_WriteRegister(CONFIG, config_reg);
            break;
    }
}

// 设置发送地址
void NRF24_SetTxAddress(uint8_t *addr) {
    NRF24_WriteRegisters(TX_ADDR, addr, 5);
}

// 设置接收地址
void NRF24_SetRxAddress(uint8_t *addr, uint8_t pipe) {
    if(pipe == 0) {
        NRF24_WriteRegisters(RX_ADDR_P0, addr, 5);
        NRF24_WriteRegister(RX_PW_P0, 32); // 设置数据宽度
    } else if(pipe == 1) {
        NRF24_WriteRegisters(RX_ADDR_P1, addr, 5);
        NRF24_WriteRegister(RX_PW_P1, 32);
    }
    
    // 启用接收通道
    uint8_t en_rxaddr = NRF24_ReadRegister(EN_RXADDR);
    en_rxaddr |= (1 << pipe);
    NRF24_WriteRegister(EN_RXADDR, en_rxaddr);
}

// 初始化NRF24L01
void NRF24_Init(void) {
    // 初始状态
    NRF24_CE_LOW();
    NRF24_CSN_HIGH();
    
    // 配置默认参数
    nrf24_config_t default_config = {
        .channel = 76,
        .data_rate = RF_DR_2MBPS,
        .tx_power = RF_PWR_0DBM,
        .crc_length = 1,
        .retry_delay = 5,
        .retry_count = 15,
        .addr_width = 5
    };
    
    NRF24_SetConfig(&default_config);
    NRF24_SetTxAddress(tx_address);
    NRF24_SetRxAddress(rx_address, 0);
    
    // 清除状态寄存器
    NRF24_WriteRegister(STATUS, 0x70);
    
    // 清空FIFO
    NRF24_FlushTx();
    NRF24_FlushRx();
}

// 发送数据
uint8_t NRF24_Transmit(uint8_t *data, uint8_t length) {
    if(length > 32) length = 32;
    
    NRF24_FlushTx();
    NRF24_SetMode(MODE_TX);
    
    NRF24_CSN_LOW();
    SPI_TransmitReceive(W_TX_PAYLOAD);
    
    for(uint8_t i = 0; i < length; i++) {
        SPI_TransmitReceive(data[i]);
    }
    
    NRF24_CSN_HIGH();
    
    // 发送数据
    NRF24_CE_HIGH();
    // 保持CE高电平至少10us
    for(uint32_t i = 0; i < 1000; i++); // 简单延时
    NRF24_CE_LOW();
    
    // 等待发送完成
    uint32_t timeout = 100000;
    uint8_t status;
    
    while(timeout--) {
        status = NRF24_GetStatus();
        
        if(status & (1 << MAX_RT)) {
            NRF24_WriteRegister(STATUS, (1 << MAX_RT));
            NRF24_FlushTx();
            NRF24_CE_LOW();  // 确保CE拉低
            return 0; // 发送失败
        }
        
        if(status & (1 << TX_DS)) {
            NRF24_WriteRegister(STATUS, (1 << TX_DS));
            return 1; // 发送成功
        }
    }
    
    NRF24_CE_LOW();  // 超时情况下也要拉低CE
    return 0; // 超时
}
// 接收数据
uint8_t NRF24_Receive(uint8_t *data, uint8_t max_length) {
    uint8_t status = NRF24_GetStatus();
    uint8_t length = 0;
    
    if(status & (1 << RX_DR)) {
        // 读取数据长度
        NRF24_CSN_LOW();
        SPI_TransmitReceive(R_RX_PL_WID);
        length = SPI_TransmitReceive(NOP);
        NRF24_CSN_HIGH();
        
        if(length > 32) {
            NRF24_FlushRx();
            length = 0;
        } else if(length > 0) {
            // 读取数据
            NRF24_CSN_LOW();
            SPI_TransmitReceive(R_RX_PAYLOAD);
            
            for(uint8_t i = 0; i < length && i < max_length; i++) {
                data[i] = SPI_TransmitReceive(NOP);
            }
            
            NRF24_CSN_HIGH();
        }
        
        // 清除接收完成标志
        NRF24_WriteRegister(STATUS, (1 << RX_DR));
    }
    
    return length;
}

// 测试连接
uint8_t NRF24_TestConnection(void) {
    uint8_t test_addr[5] = {0x12, 0x34, 0x56, 0x78, 0x9A};
    uint8_t read_addr[5];
    
    // 写入测试地址
    NRF24_WriteRegisters(TX_ADDR, test_addr, 5);
    
    // 读取地址
    NRF24_ReadRegisters(TX_ADDR, read_addr, 5);
    
    // 比较
    for(uint8_t i = 0; i < 5; i++) {
        if(test_addr[i] != read_addr[i]) {
            printf("第%d字节不匹配！\r\n", i);
            return 0;
        }
    }
    
    return 1;
}