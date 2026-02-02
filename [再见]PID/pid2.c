#include <stdint.h>
//
// Created by 23286 on 2026/2/2.
//
#include "pid2.h"


void PID_Init(PID_TypeDef*PID,float Kp,float Ki,float Kd) {
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;

    PID->SP = 0.0f;
    PID->t_t0 = 0;
    PID->err_t0 = 0;
    PID->err_in_t0 = 0;

}

void PID_Change_SP(PID_TypeDef*PID,float SP) {
    PID->SP = SP;
}

float PID_Compute(PID_TypeDef*PID,float FB) {
    float err = (PID->SP - FB);
    float t_tk = 0;//这个地方要传入定时器的值
    float deltaT = (t_tk - PID -> t_t0)*1.0e-6f;
    float err_dev = (err - PID -> err_t0)/deltaT;
    float err_int = PID -> err_in_t0 + (err - PID -> err_t0)*deltaT*0.5;

    float COp = PID->Kp * err ;
    float COi = PID->Ki * err_int ;
    float COd = PID->Kd * err_dev ;
    float CO = COp + COi + COd;

    PID->t_t0 = t_tk;
    PID->err_t0 = err;
    PID->err_in_t0 = err_int;


    return CO;
}
//学习了不依赖参数整定直接计算kp，ki，kd的科学方法！明天理解一下添加详细注释