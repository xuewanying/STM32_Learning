//
// Created by 23286 on 2026/2/4.
//

#include "moter.h"
#include "pid2.h"



void Moter_Init() {
    PID_Init(&pid_moter_L,0.0f,0.0f,0.0f);
    PID_Init(&pid_moter_R,0.0f,0.0f,0.0f);
}

void Moter_Proc() {
    //这里需要补充
    //执行间隔
    //通过编码器获得左右电机的角速度
    float omega1 = 10;
    float omega2 = 10;
    PID_Compute(&pid_moter_L,omega1);
    PID_Compute(&pid_moter_R,omega2);
    //计算PID控制器的输出
    float ua_1 = PID_Compute(&pid_moter_L,omega1);
    float ua_2 = PID_Compute(&pid_moter_R,omega2);

    //待补充：将电压设置到电机两端
    //获取电池电压
    //计算左右电机PWM的占空比

}

void Moter_SetOmega_L(float omega) {
    PID_Change_SP(&pid_moter_L,omega);
}


void Moter_SetOmega_R(float omega) {
    PID_Change_SP(&pid_moter_R,omega);
}


