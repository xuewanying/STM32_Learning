//
// Created by 23286 on 2026/2/7.
//
#include "usart.h"
#include "../Types/commandtask.h"
void StartCommandTask(void *argument) {
    UART1_Receive_Start();

}