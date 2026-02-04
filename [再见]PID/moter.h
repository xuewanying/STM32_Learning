//
// Created by 23286 on 2026/2/4.
//

#ifndef TIM_SERVO_MOTER_H
#define TIM_SERVO_MOTER_H
#include "pid2.h"
#include "main.h"

static PID_TypeDef pid_moter_L;
static PID_TypeDef pid_moter_R;

void Moter_Init();
void Moter_Proc();
void Moter_SetOmega_L(float omega);
void Moter_SetOmega_R(float omega);

#endif //TIM_SERVO_MOTER_H