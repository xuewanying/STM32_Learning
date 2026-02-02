//
// Created by 23286 on 2026/2/2.
//

#ifndef TIM_SERVO_PID2_H
#define TIM_SERVO_PID2_H
#include "main.h"
//长期存储结构体
typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float SP;
    uint64_t t_t0;
    float err_t0;
    float err_in_t0;

}PID_TypeDef;



void PID_Init(PID_TypeDef*PID,float Kp,float Ki,float Kd);
void PID_Change_SP(PID_TypeDef*PID,float SP);
float PID_Compute(PID_TypeDef*PID,float FB);
#endif //TIM_SERVO_PID2_H