//
// Created by 23286 on 2026/1/26.
//

#ifndef MPU6050_MPU6050_H
#define MPU6050_MPU6050_H
#include "main.h"
void MPU6050_Init(void);
void MPU6050_Updata(void);

float MPU6050_GetAx(void);
float MPU6050_GetAy(void);
float MPU6050_GetAz(void);

float MPU6050_Temperature(void);


float MPU6050_GetGx(void);
float MPU6050_GetGy(void);
float MPU6050_GetGz(void);


// 陀螺仪校准相关函数
void MPU6050_CalibrateGyro(uint16_t samples);  // 陀螺仪校准
float MPU6050_GetGx_Calibrated(void);          // 获取校准后的Gx
float MPU6050_GetGy_Calibrated(void);          // 获取校准后的Gy
float MPU6050_GetGz_Calibrated(void);          // 获取校准后的Gz
void MPU6050_ResetGyroCalibration(void);       // 重置陀螺仪校准
void MPU6050_GetGyroOffsets(float *offset_x, float *offset_y, float *offset_z); // 获取当前零偏值

#endif //MPU6050_MPU6050_H