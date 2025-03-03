#include "mytask.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static TaskHandle_t  Bled_TaskHandle_t, Rled_TaskHandle_t, Key1_TaskHandle_t;
TaskHandle_t Star_TaskHandle_t;


/*OLED 互斥资源*/
static SemaphoreHandle_t xOled_Semaphore = NULL;
static char oled_buff[15];
/*key1 二值信号量*/
extern SemaphoreHandle_t xKeySemaphore;

void Bled_task(void* arg)
{
	uint16_t cnt = 0;
	while(1)
	{
		HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		vTaskDelay(1000);

		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		sprintf(oled_buff,"Bled_task:%d ",cnt++);
		OLED_ShowString(0,0,(uint8_t*)oled_buff,16);
		xSemaphoreGive(xOled_Semaphore);
		
		printf("Bled_task\r\n");
	}
}

void Rled_task(void* arg)
{
	uint16_t cnt = 0;
	while(1)
	{
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		vTaskDelay(1000);

		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		sprintf(oled_buff,"Rled_task:%d ",cnt++);
		OLED_ShowString(0,0,(uint8_t*)oled_buff,16);
		xSemaphoreGive(xOled_Semaphore);
		
		printf("Rled_task\r\n");
	}
}

void Key1_task(void* arg)
{
//	xSemaphoreTake(xKeySemaphore,portMAX_DELAY);
	int cnt = 0;
	while(1)
	{
		xSemaphoreTake(xKeySemaphore,portMAX_DELAY);
		
		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		sprintf(oled_buff,"press key:%d ",++cnt);
		OLED_ShowString(0,4,(uint8_t*)oled_buff,16);
		xSemaphoreGive(xOled_Semaphore);
		
		printf("Key1_task\r\n");
	}
}


void start_task(void* arg)
{
	xOled_Semaphore = xSemaphoreCreateMutex();
	xKeySemaphore = xSemaphoreCreateBinary();


	// 创建任务led1灯任务
	if(xTaskCreate(Bled_task, "Bled_task", 128, NULL, 3, &Bled_TaskHandle_t) == 1)
		printf("creat Bled_task sussces\r\n");
	else
		printf("creat Bled_task false\r\n");
	
	// 创建任务led2灯任务
	if(xTaskCreate(Rled_task, "Rled_task", 128, NULL, 5, &Rled_TaskHandle_t) == 1)
		printf("creat Rled_task sussces\r\n");
	else
		printf("creat Rled_task false\r\n");

	// 创建任务key1灯任务
	if(xTaskCreate(Key1_task, "Key1_task", 128, NULL, 8, &Key1_TaskHandle_t) == 1)
		printf("creat Key1_task sussces\r\n");
	else
		printf("creat Key1_task false\r\n");

	
	vTaskDelete(NULL);
	
}
// xTaskCreate函数参数说明：
// 1、任务入口函数
// 2、任务名称
// 3、堆栈大小，单位
// 4、传递给任务的参数
// 5、任务优先级
// 6、任务句柄
		
//		// 创建任务led1灯任务
//    xTaskCreate(Bled_task, "led1_task", 64, NULL, 3, &Bled_TaskHandle_t);
//		// 创建任务led2灯任务
//		xTaskCreate(Gled_task, "led2_task", 64, NULL, 4, &Gled_TaskHandle_t);
//		// 创建串口打印任务
//    xTaskCreate(print_task, "print_task", 128, NULL, 5, &Print_TaskHandle_t);
//    // 启动任务调度
//    vTaskStartScheduler();
