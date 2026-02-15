//
// Created by 23286 on 2026/2/11.
//

#include "tran.h"

#include "adc.h"
#include "nrf24l01.h"

/**
  * @brief 读取PA1(ADC1_IN1)原始值（12位，0~4095）
  */
uint16_t Tran_ReadADC(void)
{
    uint16_t adc_value = 0;
    HAL_ADC_Start(&hadc1);   // 启动单次转换
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        adc_value = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return adc_value;
}

/**
  * @brief 初始化遥控发送模块（必须在NRF_Init()成功之后调用）
  */
uint8_t Tran_Init(void)
{
    NRF_TX_Mode();          // 配置为发送模式，地址、频道等已在驱动中设定
    
}

/**
  * @brief 读取摇杆 → 映射油门 → 打包 → 无线发送
  * @note  发送频率由主循环中的HAL_Delay控制，推荐20ms（50Hz）
  */
uint8_t Tran_SendThrottle(void)
{
    Packet tx_packet;
    uint8_t tx_buffer[4];

    // 1. 读取ADC摇杆原始值
    uint16_t adc_raw = Tran_ReadADC();

    // 2. 映射至油门范围（0~1000，可根据飞控协议调整）
    uint16_t throttle = (uint16_t)((uint32_t)adc_raw * 1000 / 4095);

    // 3. 填充数据包
    tx_packet.header   = 0xAA;
    tx_packet.throttle = throttle;
    // 简单累加校验和（帧头 + 油门高8位 + 油门低8位）
    tx_packet.checksum = (uint8_t)(tx_packet.header
                         + (tx_packet.throttle >> 8)
                         + (tx_packet.throttle & 0xFF));

    // 4. 转换为字节数组（避免结构体对齐问题）
    tx_buffer[0] = tx_packet.header;
    tx_buffer[1] = (tx_packet.throttle >> 8) & 0xFF;
    tx_buffer[2] = tx_packet.throttle & 0xFF;
    tx_buffer[3] = tx_packet.checksum;

    // 5. 通过NRF24L01发送（len=4）
    return NRF_SendData(tx_buffer, sizeof(tx_buffer));

    // 返回值：1成功，2达到最大重发，0超时，可根据需要添加状态指示
	
	

}