//
// Created by 23286 on 2026/2/7.
//
#include "main.h"

#ifndef QUEUE_DEMO_KEYTASK_H
#define QUEUE_DEMO_KEYTASK_H
typedef enum {
    LEDColor_Red = 0,
    LEDColor_Green = 1,
    LEDColor_Blue = 2,
}LEDColor;

typedef enum {
    LEDstate_OFF = 0,
    LEDstate_ON = 1,
}LEDState;

typedef struct {
    LEDColor color;
    LEDState state;

} LEDMessage;
#endif //QUEUE_DEMO_KEYTASK_H