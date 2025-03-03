/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "oled.h"

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

int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xffff);
	return ch;
}


TaskHandle_t Star_TaskHandle_t, Wled_TaskHandle_t, Gled_TaskHandle_t, Key1_TaskHandle_t, Print_TaskHandle_t;
void Wled_task(void* arg)
{
	while(1)
	{
		HAL_GPIO_TogglePin(LED_W_GPIO_Port, LED_W_Pin);
		vTaskDelay(500);
		printf("Wled_task\r\n");
	}
}

void Gled_task(void* arg)
{
	while(1)
	{
		HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
		vTaskDelay(300);
		printf("Gled_task\r\n");
	}
}

void print_task(void* arg)
{
    while(1)
    {
        HAL_UART_Transmit(&huart2, (uint8_t *)"HelloWorld\r\n", 12, 100);
        vTaskDelay(1000);
    }
}

void key1_task(void* arg)
{
	uint8_t end_flag = 0;
	while(1)
	{
		if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
		{
			vTaskDelay(10);
			if(HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin) == GPIO_PIN_RESET)
				end_flag = 1;
		}
		
		if(end_flag == 1)
		{
			vTaskDelete(Wled_TaskHandle_t);
			printf("key1_task end\r\n");
			break;
		}

		printf("key1_task\r\n");
		vTaskDelay(100);	
	}
	vTaskDelete(NULL);
}

void start_task(void* arg)
{
	// 创建任务led1灯任务
	if(xTaskCreate(Wled_task, "Wled_task", 32, NULL, 3, &Wled_TaskHandle_t) == 1)
		printf("creat Wled_task sussces\r\n");
	else
		printf("creat Wled_task false\r\n");
	
	// 创建任务led2灯任务
	if(xTaskCreate(Gled_task, "Gled_task", 32, NULL, 4, &Gled_TaskHandle_t) == 1)
		printf("creat Gled_task sussces\r\n");
	else
		printf("creat Gled_task false\r\n");
	
	// 创建按键检测任务
	if(xTaskCreate(key1_task, "key1_task", 32, NULL, 5, &Key1_TaskHandle_t) == 1)
		printf("creat key1_task sussces\r\n");
	else
		printf("creat key1_task false\r\n");
	
	printf("here\r\n");
	vTaskDelete(NULL);
	
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

		// xTaskCreate函数参数说明：
		// 1、任务入口函数
		// 2、任务名称
		// 3、堆栈大小，单位
		// 4、传递给任务的参数
		// 5、任务优先级
		// 6、任务句柄
		
//		// 创建任务led1灯任务
//    xTaskCreate(Wled_task, "led1_task", 64, NULL, 3, &Wled_TaskHandle_t);
//		// 创建任务led2灯任务
//		xTaskCreate(Gled_task, "led2_task", 64, NULL, 4, &Gled_TaskHandle_t);
//		// 创建串口打印任务
//    xTaskCreate(print_task, "print_task", 128, NULL, 5, &Print_TaskHandle_t);
//    // 启动任务调度
//    vTaskStartScheduler();


	if( xTaskCreate(start_task,"start_task",64,NULL,2, &Star_TaskHandle_t) == 1)
		printf("creat start_task sussces\r\n");
	else
		printf("creat start_task false\r\n");
	
	// 启动任务调度
	vTaskStartScheduler();

//  // 刚上电时STM32比OLED启动快，因此需要等待一段时间再初始化OLED
//  HAL_Delay(20);
//  // 初始化OLED
//  OLED_Init();
//  // 设置OLED显示模式：正常/反色
//  OLED_SetColorMode(OLED_COLOR_NORMAL);
//  // 设置OLED显示方向：0°/180°
//  OLED_SetOrientation(OLED_Orientation_0);
//  // 清空显示缓冲区
//  OLED_ClearBuffer();
//  // 将缓存内容更新到屏幕显示
//  OLED_Refresh();
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		printf("Z测试\n");
//		HAL_Delay(1000);

//    oled_string(10,10,"ni ssss kai de");


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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

#ifdef  USE_FULL_ASSERT
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
