//
// Created by 23286 on 2026/2/7.
//

#include "../Types/keytask.h"

#include <stdlib.h>

#include "cmsis_os2.h"
// #include "portable.h"
#include "FreeRTOS.h"
#include "stm32g4xx_hal_gpio.h"



uint8_t isKeyPressed() {
    static uint8_t pressed = 0;

    if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)== GPIO_PIN_RESET) {
        osDelay(10);
        if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)== GPIO_PIN_RESET) {
            pressed = 1;
            return 1;
        }
    }
    if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)== GPIO_PIN_SET) {
        pressed = 0;
        return 0;
    }
}




void StartKEYTask(void *argument) {
    LEDState state = LEDstate_OFF;
    for (;;) {
        if (isKeyPressed() == 1) {
            state = !state;
            // LEDMessage* message = malloc(sizeof(LEDMessage));
            LEDMessage* message = pvPortMalloc(sizeof(LEDMessage));
            message->color = LEDColor_Green;
            message->state = state;

            osMessageQueuePut(LEDQueueHandle,(&message),0,osWaitForever);

        }
        osDelay(10);
    }
}
