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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "stdio.h"
#include "string.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 卡尔曼滤波器结构体
typedef struct {
    float x;        // 状态估计值
    float P;        // 估计误差协方差
    float Q;        // 过程噪声协方差
    float R;        // 测量噪声协方差
    float K;        // 卡尔曼增益
    float F;        // 状态转移矩阵（简化为一维）
    float H;        // 观测矩阵（简化为一维）
} KalmanFilter;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USE_JUSTFLOAT 1  // 1:使用JustFloat协议，0:使用文本格式
#define CHANNEL_COUNT 2  // 发送2个通道的数据（原始值和滤波值）
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int upEdge = 0;
int downEdge = 0;
float raw_distance = 0;        // 原始测量距离
float filtered_distance = 0;    // 滤波后距离

// VOFA+ 数据缓冲区
uint8_t vofa_buffer[sizeof(float) * CHANNEL_COUNT + 4];  // 数据 + 帧尾

// 卡尔曼滤波器实例
KalmanFilter kf;

// 帧尾定义（JustFloat协议要求）
const uint8_t justfloat_tail[4] = {0x00, 0x00, 0x80, 0x7F};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
// 卡尔曼滤波函数声明
void Kalman_Init(KalmanFilter *kf, float init_value, float Q, float R);
float Kalman_Update(KalmanFilter *kf, float measurement);
void Send_RawData_To_VOFA(float raw);
void Send_FilteredData_To_VOFA(float filtered);
void Send_BothData_To_VOFA(float raw, float filtered);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
        upEdge = HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_3);
        downEdge = HAL_TIM_ReadCapturedValue(&htim1, TIM_CHANNEL_4);

        // 计算原始距离
        if (downEdge > upEdge) {
            raw_distance = ((downEdge - upEdge) * 0.034f) / 2.0f;

            // 距离限制（超声波有效范围通常为2cm-400cm）
            if (raw_distance < 2.0f) raw_distance = 2.0f;
            if (raw_distance > 400.0f) raw_distance = 400.0f;
        }
    }
}

// 卡尔曼滤波器初始化
void Kalman_Init(KalmanFilter *kf, float init_value, float Q, float R) {
    kf->x = init_value;        // 初始状态估计
    kf->P = 1.0f;              // 初始估计误差协方差
    kf->Q = Q;                 // 过程噪声协方差
    kf->R = R;                 // 测量噪声协方差
    kf->F = 1.0f;              // 状态转移矩阵（一维）
    kf->H = 1.0f;              // 观测矩阵（一维）
}

// 卡尔曼滤波器更新
float Kalman_Update(KalmanFilter *kf, float measurement) {
    // 预测步骤
    kf->x = kf->F * kf->x;           // 状态预测
    kf->P = kf->F * kf->P * kf->F + kf->Q;  // 协方差预测

    // 更新步骤
    kf->K = kf->P * kf->H / (kf->H * kf->P * kf->H + kf->R);  // 计算卡尔曼增益
    kf->x = kf->x + kf->K * (measurement - kf->H * kf->x);    // 状态更新
    kf->P = (1 - kf->K * kf->H) * kf->P;                      // 协方差更新

    return kf->x;
}

// 发送原始数据到VOFA+（文本格式）
void Send_RawData_To_VOFA(float raw) {
    char buffer[20];
    int len = sprintf(buffer, "Raw:%.2f\n", raw);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
}

// 发送滤波数据到VOFA+（文本格式）
void Send_FilteredData_To_VOFA(float filtered) {
    char buffer[20];
    int len = sprintf(buffer, "Filt:%.2f\n", filtered);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
}

// 发送双通道数据到VOFA+（JustFloat协议）
void Send_BothData_To_VOFA(float raw, float filtered) {
#if USE_JUSTFLOAT
    // 准备浮点数组
    float data[CHANNEL_COUNT];
    data[0] = raw;
    data[1] = filtered;

    // 复制数据到缓冲区（小端字节序）
    // STM32通常是小端，直接复制即可
    memcpy(vofa_buffer, data, sizeof(float) * CHANNEL_COUNT);

    // 添加帧尾
    memcpy(vofa_buffer + sizeof(float) * CHANNEL_COUNT, justfloat_tail, 4);

    // 发送完整数据包
    HAL_UART_Transmit(&huart1, vofa_buffer, sizeof(float) * CHANNEL_COUNT + 4, 100);
#else
    // 文本格式：发送"原始值,滤波值\n"
    char buffer[30];
    int len = sprintf(buffer, "%.2f,%.2f\n", raw, filtered);
    HAL_UART_Transmit(&huart1, (uint8_t*)buffer, len, 100);
#endif
}

// 调试函数：发送测试数据到VOFA+
void Send_TestData_To_VOFA(void) {
    static float t = 0;
    t += 0.1f;

    float test_data[4];
    test_data[0] = sinf(t);
    test_data[1] = sinf(2*t);
    test_data[2] = sinf(3*t);
    test_data[3] = sinf(4*t);

    // 发送数据部分
    HAL_UART_Transmit(&huart1, (uint8_t*)test_data, sizeof(float) * 4, 100);

    // 发送帧尾
    HAL_UART_Transmit(&huart1, justfloat_tail, 4, 100);
}
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
    MX_TIM1_Init();
    MX_USART1_UART_Init();
    /* USER CODE BEGIN 2 */
    HAL_Delay(500);
    OLED_Init();

    // 初始化卡尔曼滤波器
    // 参数调整建议：
    // Q: 过程噪声协方差，值越小越相信模型（建议0.001-0.1）
    // R: 测量噪声协方差，值越小越相信测量（建议0.1-1.0）
    Kalman_Init(&kf, 10.0f, 0.1f, 1.0f);

    // 启动定时器和输入捕获
    HAL_TIM_Base_Start_IT(&htim1);
    HAL_TIM_IC_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4);

    char message[20] = "";

    // 发送测试数据（可选，用于检查VOFA+连接）
    // Send_TestData_To_VOFA();
    // HAL_Delay(1000);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        // 发送触发脉冲
        HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_SET);
        HAL_Delay(1);  // 10us脉冲，根据HC-SR04要求
        HAL_GPIO_WritePin(Trig_GPIO_Port, Trig_Pin, GPIO_PIN_RESET);
        __HAL_TIM_SET_COUNTER(&htim1, 0);

        // 等待测量完成
        HAL_Delay(60);  // HC-SR04测量周期至少60ms，这里用20ms

        // 卡尔曼滤波
        filtered_distance = Kalman_Update(&kf, raw_distance);

        // OLED显示
        OLED_NewFrame();
        sprintf(message, "Raw:%.1fcm", raw_distance);
        OLED_PrintString(0, 0, message, &font16x16, OLED_COLOR_NORMAL);

        sprintf(message, "Filt:%.1fcm", filtered_distance);
        OLED_PrintString(0, 16, message, &font16x16, OLED_COLOR_NORMAL);

        OLED_ShowFrame();

        // 发送数据到VOFA+
        Send_BothData_To_VOFA(raw_distance, filtered_distance);

        // 控制发送频率
        // 注意：不要在主循环中添加额外的延迟，因为测量已经需要20ms延迟
        // 现在的总周期约为20ms（50Hz），适合VOFA+显示

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