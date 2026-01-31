#include "nrf24l01.h"
#include <stdio.h>

// SPI句柄
extern SPI_HandleTypeDef hspi2;

// 地址
uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0xE1, 0xE2, 0xE3, 0xE4, 0xE5};
uint8_t RX_ADDRESS[RX_ADR_WIDTH] = {0xE1, 0xE2, 0xE3, 0xE4, 0xE5};

// SPI传输函数
uint8_t NRF_SPI_Transfer(uint8_t data) {
    uint8_t rx_data = 0;
	    if(HAL_SPI_GetState(&hspi2) != HAL_SPI_STATE_READY) {
        return 0;}
    HAL_SPI_TransmitReceive(&hspi2, &data, &rx_data, 1, 1000);
    return rx_data;
}

// 读寄存器
uint8_t NRF_ReadReg(uint8_t reg) {
    uint8_t value;
    NRF_CSN_LOW();
    NRF_SPI_Transfer(R_REGISTER | reg);
    value = NRF_SPI_Transfer(NOP);
    NRF_CSN_HIGH();
    return value;
}

// 写寄存器
uint8_t NRF_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t status;
    NRF_CSN_LOW();
    status = NRF_SPI_Transfer(W_REGISTER | reg);
    NRF_SPI_Transfer(value);
    NRF_CSN_HIGH();
    return status;
}

// 批量写寄存器
void NRF_WriteMultiReg(uint8_t reg, uint8_t *data, uint8_t len) {
    NRF_CSN_LOW();
    NRF_SPI_Transfer(W_REGISTER | reg);
    for(uint8_t i = 0; i < len; i++) {
        NRF_SPI_Transfer(data[i]);
    }
    NRF_CSN_HIGH();
}

// 批量读寄存器
void NRF_ReadMultiReg(uint8_t reg, uint8_t *data, uint8_t len) {
    NRF_CSN_LOW();
    NRF_SPI_Transfer(R_REGISTER | reg);
    for(uint8_t i = 0; i < len; i++) {
        data[i] = NRF_SPI_Transfer(NOP);
    }
    NRF_CSN_HIGH();
}

// 清空TX FIFO
void NRF_FlushTX(void) {
    NRF_CSN_LOW();
    NRF_SPI_Transfer(FLUSH_TX);
    NRF_CSN_HIGH();
}

// 清空RX FIFO
void NRF_FlushRX(void) {
    NRF_CSN_LOW();
    NRF_SPI_Transfer(FLUSH_RX);
    NRF_CSN_HIGH();
}

// 读取状态寄存器
uint8_t NRF_ReadStatus(void) {
    uint8_t status;
    NRF_CSN_LOW();
    status = NRF_SPI_Transfer(NOP);
    NRF_CSN_HIGH();
    return status;
}

// 检测模块
uint8_t NRF_Check(void) {
    uint8_t buf[5] = {0xA5, 0xA5, 0xA5, 0xA5, 0xA5};
    uint8_t buf_read[5];

    NRF_WriteMultiReg(TX_ADDR, buf, 5);
    NRF_ReadMultiReg(TX_ADDR, buf_read, 5);

    for(uint8_t i = 0; i < 5; i++) {
        if(buf_read[i] != buf[i]) {
            return 0;
        }
    }
    return 1;
}

// 初始化
uint8_t NRF_Init(void) {
	    HAL_Delay(100);  // 等待电源稳定
    NRF_CE_LOW();
    NRF_CSN_HIGH();
    HAL_Delay(100);
	    if(!NRF_Check()) {
        // 可以尝试多次检查
        for(int i = 0; i < 3; i++) {
            HAL_Delay(10);
            if(NRF_Check()) {
                return 1;
            }
        }
        return 0;
    }
	}



// 发送模式配置
void NRF_TX_Mode(void) {
    NRF_CE_LOW();

    // 写TX地址
    NRF_WriteMultiReg(TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);
    // 写RX地址（通道0，用于接收ACK）
    NRF_WriteMultiReg(RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);

    NRF_WriteReg(EN_AA, 0x01);      // 使能通道0自动ACK
    NRF_WriteReg(EN_RXADDR, 0x01);  // 使能接收地址通道0
    NRF_WriteReg(SETUP_RETR, 0x1A); // 500us + 86us重发延时，最大重发10次
    NRF_WriteReg(RF_CH, 2);         // 2.402GHz + 2 = 2.404GHz
    NRF_WriteReg(RF_SETUP, 0x07);   // 0dB增益，1Mbps
    NRF_WriteReg(RX_PW_P0, TX_PLOAD_WIDTH); // 接收通道0数据宽度

    // 配置为发送模式
    NRF_WriteReg(CONFIG, 0x0E);     // 上电，使能CRC（2字节），发送模式

    NRF_FlushTX();
    NRF_FlushRX();
}

// 接收模式配置
void NRF_RX_Mode(void) {
    NRF_CE_LOW();

    // 写RX地址
    NRF_WriteMultiReg(RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH);

    NRF_WriteReg(EN_AA, 0x01);      // 使能通道0自动ACK
    NRF_WriteReg(EN_RXADDR, 0x01);  // 使能接收地址通道0
    NRF_WriteReg(RF_CH, 2);         // 2.402GHz + 2 = 2.404GHz
    NRF_WriteReg(RF_SETUP, 0x07);   // 0dB增益，1Mbps
    NRF_WriteReg(RX_PW_P0, RX_PLOAD_WIDTH); // 接收通道0数据宽度

    // 配置为接收模式
    NRF_WriteReg(CONFIG, 0x0F);     // 上电，使能CRC（2字节），接收模式

    NRF_FlushRX();
    NRF_CE_HIGH();
    HAL_Delay(2);
}

// 发送数据（按照参考代码的流程）
uint8_t NRF_SendData(uint8_t *data, uint8_t len) {
    uint8_t status;

    if(len > TX_PLOAD_WIDTH) {
        return 0;
    }

    NRF_CE_LOW();  // 进入待机模式

    // 写接收地址（用于ACK）
    NRF_WriteMultiReg(RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);

    // 写发送数据
    NRF_CSN_LOW();
    NRF_SPI_Transfer(W_TX_PAYLOAD);
    for(uint8_t i = 0; i < len; i++) {
        NRF_SPI_Transfer(data[i]);
    }
    NRF_CSN_HIGH();

    // 配置为发送模式
    NRF_WriteReg(CONFIG, 0x0E);  // IRQ收发完成中断响应，16位CRC，主发送

    // 启动发送（保持至少10us）
    NRF_CE_HIGH();
    HAL_Delay(1);
    NRF_CE_LOW();

    // 等待发送完成或超时
    uint32_t timeout = 100000;
    while(timeout--) {
        status = NRF_ReadStatus();

        if(status & TX_DS) {  // 发送成功
            NRF_WriteReg(STATUS, TX_DS);  // 清除TX_DS中断标志
            return 1;
        }

        if(status & MAX_RT) {  // 达到最大重发次数
            NRF_FlushTX();  // 清空TX FIFO
            NRF_WriteReg(STATUS, MAX_RT);  // 清除MAX_RT中断标志
            return 2;
        }

        HAL_Delay(1);
    }

    return 0;  // 超时
}

// 接收数据
uint8_t NRF_ReceiveData(uint8_t *data) {
    uint8_t status;

    status = NRF_ReadStatus();

    if(status & RX_DR) {  // 接收到数据
        NRF_CE_LOW();

        // 读取RX FIFO数据
        NRF_CSN_LOW();
        NRF_SPI_Transfer(R_RX_PAYLOAD);
        for(uint8_t i = 0; i < RX_PLOAD_WIDTH; i++) {
            data[i] = NRF_SPI_Transfer(NOP);
        }
        NRF_CSN_HIGH();

        // 清除中断标志
        NRF_WriteReg(STATUS, RX_DR);

        // 清空RX FIFO
        NRF_FlushRX();

        // 重新进入接收模式
        NRF_RX_Mode();

        return 1;
    }

    return 0;
	}

/*使用示例：
// 发送示例
void Send_Test(void) {
    uint8_t tx_data[32] = "Hello, nRF24L01!";

    NRF_TX_Mode();

    if(NRF_SendData(tx_data, 32) == 1) {
        printf("Send success!\r\n");
    } else {
        printf("Send failed!\r\n");
    }
}

// 接收示例
void Receive_Test(void) {
    uint8_t rx_data[32];

    NRF_RX_Mode();

    if(NRF_ReceiveData(rx_data)) {
        printf("Received: %s\r\n", rx_data);
    }
}

// 主函数
int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI2_Init();

    // 初始化nRF24L01
    if(NRF_Init()) {
        printf("nRF24L01 init success!\r\n");
    } else {
        printf("nRF24L01 init failed!\r\n");
        while(1);
    }

    // 测试发送
    Send_Test();

    // 或者测试接收
    // Receive_Test();

    while(1) {
        HAL_Delay(1000);
    }
}
 */