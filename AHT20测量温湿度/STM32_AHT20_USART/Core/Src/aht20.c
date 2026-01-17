//
// Created by 23286 on 2026/1/16.
//
#include "aht20.h"

#include <sys/types.h>

#define AHT20_ADDRESS 0x70
uint8_t readBuffer[6] = {0};


void AHT20_Init() {
    uint8_t readBuffer;
    HAL_Delay(40);
    HAL_I2C_Master_Receive(&hi2c1,AHT20_ADDRESS,&readBuffer,1,HAL_MAX_DELAY);
    if ((readBuffer & 0x08) == 0x00) {
        uint8_t sendBuffer[3] = {0xBE,0x08,0x00};
        HAL_I2C_Master_Transmit(&hi2c1,AHT20_ADDRESS,sendBuffer,3,HAL_MAX_DELAY);
    }

}


void AHT20_Read(float *Humidity,float *Temperature) {
    uint8_t sendBuffer[3] = {0xAC,0x33,0x00};
    uint8_t readBuffer[6] = {0} ;
    HAL_I2C_Master_Transmit_DMA(&hi2c1,AHT20_ADDRESS,sendBuffer,3);
    HAL_Delay(75);
    HAL_I2C_Master_Receive_DMA(&hi2c1,AHT20_ADDRESS,readBuffer,6);



    if ((readBuffer[0] & 0x80) == 0x00) {
        uint32_t data = 0;
        data = ((uint32_t)readBuffer[3]>>4)+((uint32_t)readBuffer[2]<<4)+((uint32_t)readBuffer[1]<<12);
        *Humidity = data*100.0f / (1<<20);

        data = (((uint32_t)readBuffer[3] & 0x0F)<<16) + ((uint32_t)readBuffer[4] << 8) + (uint32_t)readBuffer[5];
        *Temperature = data*200.0f / (1<<20) - 50;
    }
}


//状态机改造
//发送测试指令
void AHT20_Measure() {
    static uint8_t sendBuffer[3] = {0xAC,0x33,0x00};
    HAL_I2C_Master_Transmit_DMA(&hi2c1,AHT20_ADDRESS,sendBuffer,3);
}

//读取测量数据（全局变量向上找）
void AHT20_Get() {
    HAL_I2C_Master_Receive_DMA(&hi2c1,AHT20_ADDRESS,readBuffer,6);
}

//计算温湿度
void AHT20_Analys(float *Humidity,float *Temperature) {
    if ((readBuffer[0] & 0x80) == 0x00) {
        uint32_t data = 0;
        data = ((uint32_t)readBuffer[3]>>4)+((uint32_t)readBuffer[2]<<4)+((uint32_t)readBuffer[1]<<12);
        *Humidity = data*100.0f / (1<<20);

        data = (((uint32_t)readBuffer[3] & 0x0F)<<16) + ((uint32_t)readBuffer[4] << 8) + (uint32_t)readBuffer[5];
        *Temperature = data*200.0f / (1<<20) - 50;
    }
}

