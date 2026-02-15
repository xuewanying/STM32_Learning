//
// Created by 23286 on 2026/2/11.
//

#ifndef REMOTE_TRAN_H
#define REMOTE_TRAN_H
#include <stdint.h>

typedef struct
{
    uint8_t header;      // 帧头，固定 0xAA
    uint16_t throttle;   // 油门值（已映射至 0~1000）
    uint8_t checksum;    // 校验和
} Packet;

/* 遥控功能接口 */
uint8_t Tran_Init(void);          // 初始化遥控发送模块（配置NRF为发送模式）
uint8_t Tran_SendThrottle(void);  // 读取摇杆 → 打包 → 无线发送
uint16_t Tran_ReadADC(void);   // 读取PA1的ADC原始值（若需单独使用）

#endif //REMOTE_TRAN_H