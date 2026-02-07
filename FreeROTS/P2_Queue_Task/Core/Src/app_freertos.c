/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
// uint8_t btnPressed = 0;
// uint8_t btnCount = 0;

/* USER CODE END Variables */
/* Definitions for LTask */
osThreadId_t LTaskHandle;
const osThreadAttr_t LTask_attributes = {
  .name = "LTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for STask */
osThreadId_t STaskHandle;
const osThreadAttr_t STask_attributes = {
  .name = "STask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 128 * 4
};
/* Definitions for BtnQueue */
osMessageQueueId_t BtnQueueHandle;
const osMessageQueueAttr_t BtnQueue_attributes = {
  .name = "BtnQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLTask(void *argument);
void StartSTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of BtnQueue */
  BtnQueueHandle = osMessageQueueNew (16, sizeof(uint32_t), &BtnQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LTask */
  LTaskHandle = osThreadNew(StartLTask, NULL, &LTask_attributes);

  /* creation of STask */
  STaskHandle = osThreadNew(StartSTask, NULL, &STask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartLTask */
/**
  * @brief  Function implementing the LTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLTask */
void StartLTask(void *argument)
{
  /* USER CODE BEGIN StartLTask */

  static uint32_t btnCount = 0;

  for(;;) {
    osDelay(10);
    if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11) == RESET) {
      osDelay(10);
      if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11) == RESET) {

        btnCount++;
        osMessageQueuePut(BtnQueueHandle, &btnCount, 0, 0);
      }
      while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11) == RESET) {
        osDelay(10);
      }
    }



  }

  /* USER CODE END StartLTask */
}
  //   if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11) == GPIO_PIN_RESET) {
  //     osDelay(10);
  //     if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11) == GPIO_PIN_RESET) {
  //       btnPressed = 1;
  //       btnCount++;
  //     }
  //     // while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11) == GPIO_PIN_RESET) {
  //     //   btnPressed = 1;
  //     //   osDelay(10);
  //     // }
  //   }else{
  //     btnPressed = 0;
  //     osDelay(10);
  //   }
  //
  // }
  /* USER CODE END StartLTask */


/* USER CODE BEGIN Header_StartSTask */
/**
* @brief Function implementing the STask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSTask */
void StartSTask(void *argument) {
  /* USER CODE BEGIN StartSTask */
  static uint32_t btnCount= 0;
  char msg[50];

  // uint32_t dataCount = 0;
  // char msg[50];
  uint8_t dataCount = 0;

  for(;;) {
    osMessageQueueGet(BtnQueueHandle, &btnCount, 0, osWaitForever);
    dataCount++;
    osDelay(1000);
    sprintf(msg,"btnCount: %d  dataCount：%d ",btnCount,dataCount);
    HAL_UART_Transmit(&huart1,(uint8_t *)msg,strlen(msg),10000);
  }

  // for(;;)
  // {
  //   osDelay(10);
  //   if (btnPressed == 1) {
  //     dataCount++;
  //     osDelay(1000);
  //     sprintf(msg,"Button Pressed: %d",dataCount);
  //     HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), 1000);
  //   }
  // }
  /* USER CODE END StartSTask */
}


  /* USER CODE END StartSTask */

  /* Private application code --------------------------------------------------*/
  /* USER CODE BEGIN Application */

  /* USER CODE END Application */
