//
// Created by 23286 on 2026/2/8.
//

#ifndef RINGBUFFER_COMMAND_H
#define RINGBUFFER_COMMAND_H


#include "main.h"
#include <string.h>

uint8_t Command_Write(uint8_t *data, uint8_t length);
uint8_t Command_GetCommand(uint8_t *command);

#endif //RINGBUFFER_COMMAND_H