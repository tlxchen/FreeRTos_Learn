#include "mytask.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static TaskHandle_t  Bled_TaskHandle_t, Rled_TaskHandle_t, Key1_TaskHandle_t;
TaskHandle_t Star_TaskHandle_t;


/*OLED ������Դ*/
static SemaphoreHandle_t xOled_Semaphore = NULL;
static char oled_buff[15];
/*key1 ��ֵ�ź���*/
extern SemaphoreHandle_t xKeySemaphore;
/*����Rled�Ƿ��������ź���*/
static SemaphoreHandle_t xRled_contl_Semaphore;

/*һ����ʱ��*/
static TimerHandle_t TIM1Handle_t = NULL;

typedef struct messg{
	uint8_t Mesg_id;
	int count;
	char *info;
}Qmesg;

 
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
		//��ȡ��ƿ����ź���
		xSemaphoreTake(xRled_contl_Semaphore,portMAX_DELAY);
		
		vTaskDelay(1000);

		xSemaphoreTake(xOled_Semaphore,portMAX_DELAY);
		sprintf(oled_buff,"Rled_task:%d ",cnt++);
		OLED_ShowString(0,2,(uint8_t*)oled_buff,16);
		xSemaphoreGive(xOled_Semaphore);
		
		//����з�����Ϣ	
		
		printf("send(xRled\r\n");
		
		
		//�����ƻ�ȡ�ź���
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
		
		//���ƺ��������ź�������һ�������������ٰ�һ�½������
		if(cnt==1)xSemaphoreTake(xRled_contl_Semaphore,portMAX_DELAY);
		else xSemaphoreGive(xRled_contl_Semaphore);
		
		printf("Key1_task\r\n");
	}
}

void showCallback( TimerHandle_t xTimer )
{
	char tmpbuff[15];
	static uint8_t c = 0;
	int timId = (int)pvTimerGetTimerID(xTimer);
	
	sprintf(tmpbuff,"tID:%d c:%d",timId,c++);
	xSemaphoreTake(xOled_Semaphore,100);
	OLED_ShowString(0,6,(uint8_t*)tmpbuff,16);
	xSemaphoreGive(xOled_Semaphore);
}

void start_task(void* arg)
{
	xOled_Semaphore = xSemaphoreCreateMutex();
	xKeySemaphore = xSemaphoreCreateBinary();
	xRled_contl_Semaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xRled_contl_Semaphore);
	
	 TIM1Handle_t =  xTimerCreate("show_tim",1500,pdTRUE,0,showCallback);


	
	// ��������led1������
	if(xTaskCreate(Bled_task, "Bled_task", 128, NULL, 3, &Bled_TaskHandle_t) == 1)
		printf("creat Bled_task sussces\r\n");
	else
		printf("creat Bled_task false\r\n");
	
	// ��������led2������
	if(xTaskCreate(Rled_task, "Rled_task", 128, NULL, 5, &Rled_TaskHandle_t) == 1)
		printf("creat Rled_task sussces\r\n");
	else
		printf("creat Rled_task false\r\n");

	// ��������key1����
	if(xTaskCreate(Key1_task, "Key1_task", 128, NULL, 8, &Key1_TaskHandle_t) == 1)
		printf("creat Key1_task sussces\r\n");
	else
		printf("creat Key1_task false\r\n");
	
	xTimerStart( TIM1Handle_t ,200);
	vTaskDelete(NULL);
	
}
// xTaskCreate��������˵����
// 1��������ں���
// 2����������
// 3����ջ��С����λ
// 4�����ݸ�����Ĳ���
// 5���������ȼ�
// 6��������
		
//		// ��������led1������
//    xTaskCreate(Bled_task, "led1_task", 64, NULL, 3, &Bled_TaskHandle_t);
//		// ��������led2������
//		xTaskCreate(Gled_task, "led2_task", 64, NULL, 4, &Gled_TaskHandle_t);
//		// �������ڴ�ӡ����
//    xTaskCreate(print_task, "print_task", 128, NULL, 5, &Print_TaskHandle_t);
//    // �����������
//    vTaskStartScheduler();
