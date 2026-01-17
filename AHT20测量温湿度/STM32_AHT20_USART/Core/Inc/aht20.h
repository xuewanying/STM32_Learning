//
// Created by 23286 on 2026/1/16.
//

#ifndef IIC_AHT20_H
#define IIC_AHT20_H

#include "i2c.h"
extern uint8_t aht20_readBuffer[6];

void AHT20_Init();
void AHT20_Read(float *Humidity,float *Temperature);

void AHT20_Measure();
void AHT20_Get();
void AHT20_Analys(float *Humidity,float *Temperature);

#endif //IIC_AHT20_H
