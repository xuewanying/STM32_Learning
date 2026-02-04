//
// Created by 23286 on 2026/2/2.
#ifndef TIM_SERVO_PID2_H
#define TIM_SERVO_PID2_H
#include "main.h"

// 长期存储结构体 - PID控制器状态结构体
typedef struct {
    float Kp;          // 比例增益
    float Ki;          // 积分增益
    float Kd;          // 微分增益
    float SP;          // 设定值（Set Point）
    uint64_t t_t0;     // 上次采样时间戳
    float err_t0;      // 上次的误差值
    float err_in_t0;   // 上次的误差积分值
} PID_TypeDef;

// 函数声明
void PID_Init(PID_TypeDef* PID, float Kp, float Ki, float Kd);  // PID初始化
void PID_Change_SP(PID_TypeDef* PID, float SP);                  // 改变设定值
float PID_Compute(PID_TypeDef* PID, float FB);                   // PID计算

#endif // TIM_SERVO_PID2_H