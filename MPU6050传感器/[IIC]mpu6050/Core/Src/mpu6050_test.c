//
// Created by 23286 on 2026/1/26.
//
#include "mpu6050.h"         // MPU6050传感器驱动头文件
#include "usart.h"           // 串口通信头文件
#include "stdio.h"           // 标准输入输出，用于sprintf函数
#include "string.h"          // 字符串操作，用于strlen函数

/**
 * @brief MPU6050传感器测试函数
 * @note 此函数初始化MPU6050并循环读取传感器数据，通过串口发送到上位机
 * 数据格式：加速度(ax, ay, az)，角速度(gx, gy, gz)，温度(temperature)
 * 发送频率：1Hz（每1000ms发送一次）
 */
void mpu6050_test(void) {

    char buffer[128];        // 数据缓冲区，用于格式化输出数据

    MPU6050_Init();          // 初始化MPU6050传感器（I2C通信、寄存器配置等）

    while (1) {
        // 更新传感器数据（读取原始寄存器值并转换为物理量）
        MPU6050_Updata();

        // 获取加速度数据（单位：g或m/s²，取决于驱动实现）
        float ax = MPU6050_GetAx();  // X轴加速度
        float ay = MPU6050_GetAy();  // Y轴加速度
        float az = MPU6050_GetAz();  // Z轴加速度

        // 获取角速度数据（单位：°/s 或 rad/s，取决于驱动实现）
        float gx = MPU6050_GetGx();  // X轴角速度
        float gy = MPU6050_GetGy();  // Y轴角速度
        float gz = MPU6050_GetGz();  // Z轴角速度

        // 获取温度数据（单位：℃）
        float temperature = MPU6050_Temperature();

        // 将数据格式化为字符串，保留3位小数，每个数据用逗号分隔
        // 格式示例：1.234,0.567,-9.812,0.123,0.456,0.789,25.500\r\n
        sprintf(buffer, "%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",
                ax, ay, az, gx, gy, gz, temperature);

        // 通过串口1发送数据到上位机
        // &huart1：串口1句柄
        // (uint8_t*)buffer：数据缓冲区（转换为无符号8位指针）
        // strlen(buffer)：要发送的数据长度
        // 50：发送超时时间（单位：ms）
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), 50);

        // 延时1000ms，控制数据发送频率为1Hz
        HAL_Delay(1000);
    }
}