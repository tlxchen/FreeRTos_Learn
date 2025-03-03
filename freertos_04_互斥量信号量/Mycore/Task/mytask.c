#include "mytask.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static TaskHandle_t  Bled_TaskHandle_t, Rled_TaskHandle_t, Key1_TaskHandle_t,Show_TaskHandle_t;
TaskHandle_t Star_TaskHandle_t;


/*OLED 互斥资源*/
static SemaphoreHandle_t xOled_Semaphore = NULL;
static char oled_buff[15];
/*key1 二值信号量*/
extern SemaphoreHandle_t xKeySemaphore;
/*控制Rled是否阻塞的信号量*/
static SemaphoreHandle_t xRled_contl_Semaphore;
/*Rled与Bled共享队列*/
static QueueHandle_t xLedQueue1 = NULL;

typedef struct messg{
	uint8_t Mesg_id;
	int count;
	char *info;
}Qmesg;
void Bled_task(void* arg)
{
	uint16_t cnt = 0;
	Qmesg Bled_mesg;
	Bled_mesg.Mesg_id = 0;
	Bled_mesg.info = "Bled";
	while(1)
	{
		HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
		//向队列发送信息
		printf("send(Bled_task\r\n");
		Bled_mesg.count = cnt;
		xQueueSend(xLedQueue1,&Bled_mesg,portMAX_DELAY);
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
	Qmesg Rled_mesg;
	Rled_mesg.Mesg_id = 1;
	Rled_mesg.info = "Rled";
	while(1)
	{
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		//获取红灯控制信号量
		xSemaphoreTake(xRled_contl_Semaphore,portMAX_DELAY);
		
		vTaskDelay(1000);

		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		sprintf(oled_buff,"Rled_task:%d ",cnt++);
		OLED_ShowString(0,2,(uint8_t*)oled_buff,16);
		xSemaphoreGive(xOled_Semaphore);
		
		//向队列发送信息	
		Rled_mesg.count = cnt;
		printf("send(xRled\r\n");
		xQueueSend(xLedQueue1,&Rled_mesg,portMAX_DELAY);
		
		//给予红灯获取信号量
		xSemaphoreGive(xRled_contl_Semaphore);
		
		printf("Rled_task\r\n");
	}
}

void Key1_task(void* arg)
{
	int cnt = 0;
	while(1)
	{
		xSemaphoreTake(xKeySemaphore,portMAX_DELAY);

		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		cnt = (cnt+1)%2;
		sprintf(oled_buff,"press key:%d ",cnt);
		OLED_ShowString(0,4,(uint8_t*)oled_buff,16);

		xSemaphoreGive(xOled_Semaphore);
		
		//控制红灯任务的信号量，按一下阻塞该任务，再按一下解除阻塞
		if(cnt==1)xSemaphoreTake(xRled_contl_Semaphore,portMAX_DELAY);
		else xSemaphoreGive(xRled_contl_Semaphore);
		
		printf("Key1_task\r\n");
	}
}

void show_task(void* arg)
{
	Qmesg re;
	uint8_t line;		//控制显示的行
	while(1)
	{
		xQueueReceive(xLedQueue1,&re,portMAX_DELAY);
		
		if(re.Mesg_id == 0)
			line = 4;
		else
			line = 6;
		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		
		sprintf(oled_buff,"recieve:%d ",re.count);
		OLED_ShowString(0,line,(uint8_t*)oled_buff,16);
		
		OLED_ShowString(0,line==6?4:6,(uint8_t*)"              ",16);
		xSemaphoreGive(xOled_Semaphore);
	}

}

void start_task(void* arg)
{
	xOled_Semaphore = xSemaphoreCreateMutex();
	xKeySemaphore = xSemaphoreCreateBinary();
	xRled_contl_Semaphore = xSemaphoreCreateBinary();
	xLedQueue1 =  xQueueCreate(10, sizeof(Qmesg));
	
	xSemaphoreGive(xRled_contl_Semaphore);
	

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

	// 创建任务key1任务
	if(xTaskCreate(Key1_task, "Key1_task", 128, NULL, 8, &Key1_TaskHandle_t) == 1)
		printf("creat Key1_task sussces\r\n");
	else
		printf("creat Key1_task false\r\n");
	// 创建任务showinfo任务
	if(xTaskCreate(show_task, "show_task", 128, NULL, 7, &Show_TaskHandle_t) == 1)
		printf("creat show_task sussces\r\n");
	else
		printf("creat show_task false\r\n");
	
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
