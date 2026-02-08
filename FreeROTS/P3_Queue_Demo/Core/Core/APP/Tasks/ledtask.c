//
// Created by 23286 on 2026/2/7.
//

#include "../Types/ledtask.h"
#include "Types/keytask.h"
#include "FreeRTOS.h"

void StartLEDTask(void *argument) {
    for (;;) {
        LEDMessage *message;
        osMessageQueueGet(LEDQueueHandle,&message,0,osWaitForever);
        switch (message->color) {
            case LEDColor_Red:
                HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,message->state?GPIO_PIN_SET:GPIO_PIN_RESET);
                break;
            case LEDColor_Green:
                HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,message->state?GPIO_PIN_SET:GPIO_PIN_RESET);
                break;
            case LEDColor_Blue:
                HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,message->state?GPIO_PIN_SET:GPIO_PIN_RESET);
                break;
        }
        vPortFree(message);

    }
}
