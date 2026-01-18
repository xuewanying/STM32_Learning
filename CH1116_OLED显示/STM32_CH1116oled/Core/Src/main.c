/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "font.h"
#include "string.h"
#include "stdio.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  HAL_Delay(20);  // 给OLED足够的上电时间


  // 1. 初始化OLED
  OLED_Init();


  // 2. 绘制全屏白色矩形（亮屏测试）
  OLED_NewFrame();  // 开始新的一帧
  OLED_DrawFilledRectangle(0, 0, 128, 64, OLED_COLOR_NORMAL);  // 全屏白色填充
  OLED_ShowFrame();  // 显示到屏幕


  HAL_Delay(3000);  // 保持3秒

  // 3. 清屏（黑色）
  OLED_NewFrame();  // 开始新的一帧
  OLED_DrawFilledRectangle(0, 0, 128, 64, OLED_COLOR_REVERSED);  // 全屏黑色填充
  OLED_ShowFrame();  // 显示到屏幕
  HAL_Delay(1000);  // 保持1秒

  // 4. 绘制白色边框矩形
  OLED_NewFrame();  // 开始新的一帧
  OLED_DrawRectangle(10, 10, 108, 44, OLED_COLOR_NORMAL);  // 白色边框矩形
  OLED_ShowFrame();  // 显示到屏幕

  HAL_Delay(1000);  // 保持1秒

  // 5. 填充白色矩形（中间区域）
  OLED_NewFrame();  // 开始新的一帧
  OLED_DrawFilledRectangle(20, 20, 88, 24, OLED_COLOR_NORMAL);  // 白色填充矩形
  OLED_ShowFrame();  // 显示到屏幕

  HAL_Delay(1000);  // 保持1秒

  // 6. 绘制对角线（交叉线）
  OLED_NewFrame();  // 开始新的一帧
  OLED_DrawLine(0, 0, 127, 63, OLED_COLOR_NORMAL);  // 左上到右下
  OLED_DrawLine(127, 0, 0, 63, OLED_COLOR_NORMAL);  // 右上到左下
  OLED_ShowFrame();  // 显示到屏幕

  HAL_Delay(1000);  // 保持1秒

  // 7. 绘制多个矩形（棋盘效果）
  OLED_NewFrame();  // 开始新的一帧
  for(int x = 0; x < 128; x += 32) {
    for(int y = 0; y < 64; y += 16) {
      if((x/32 + y/16) % 2 == 0) {
        OLED_DrawFilledRectangle(x, y, 32, 16, OLED_COLOR_NORMAL);
      }
    }
  }
  OLED_ShowFrame();  // 显示到屏幕
  HAL_Delay(2000);  // 保持2秒

  // 8. 返回正常显示模式
  OLED_NewFrame();  // 开始新的一帧
  // 绘制一个边框
  OLED_DrawRectangle(0, 0, 127, 63, OLED_COLOR_NORMAL);
  // 显示文字
  OLED_PrintASCIIString(10, 10, "OLED Test", &afont12x6, OLED_COLOR_NORMAL);
  OLED_PrintASCIIString(10, 30, "Screen OK!", &afont12x6, OLED_COLOR_NORMAL);
  OLED_PrintString(10, 50, "波特律动", &font16x16, OLED_COLOR_NORMAL);
  OLED_ShowFrame();  // 显示到屏幕
  printf("最终显示界面完成\r\n");

  printf("OLED测试程序完成！\r\n");
  /* USER CODE END 2 */

/* USER CODE END 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
