#ifndef __MAIN_TASK_CONFIG_H
#define __MAIN_TASK_CONFIG_H
#include "../FreeRTOS/include/FreeRTOS.h"
#include "../FreeRTOS/include/portable.h"
#include "../FreeRTOS/include/task.h"
#include "../FreeRTOS/include/queue.h"
#include "../FreeRTOS/include/semphr.h"
#include "../FreeRTOS/include/event_groups.h"
#include "../FreeRTOS/portable/RVDS/ARM_CM3/portmacro.h"
#include "string.h"
#include "limits.h" /* 引用ULONG_MAX必须导入该头文件, 否则自行输入0xffffffffffffffffUL */

//////////////////////////////////////////////////////////////
//						-- <开始任务> --					//
//////////////////////////////////////////////////////////////
#define START_TASK_PRIORITY			1		//开始任务优先级(与NVIC相反，数字越大优先级越高)
#define START_TASK_STACK_SIZE		128		//开始任务堆栈大小
TaskHandle_t StartTask_Handler;				//开始任务句柄
void start_task(void* pvParameters);		//开始任务函数

//////////////////////////////////////////////////////////////
//					   -- <NRF模式任务> --					//
//////////////////////////////////////////////////////////////
#define	NRF_MODE_TASK_PRIORITY		2		//NRF模式任务优先级
#define NRF_MODE_TASK_STACK_SIZE	128		//NRF模式任务堆栈大小
TaskHandle_t NRFModeTask_Handler;			//NRF模式任务句柄
void nrf_mode_task(void* pvParameters);		//NRF模式任务函数
#define SHOW_NRF_UI                 (1 << 0)//UI显示事件位

//////////////////////////////////////////////////////////////
//					   -- <ESP模式任务> --					//
//////////////////////////////////////////////////////////////
#define	ESP_MODE_TASK_PRIORITY		2		//ESP模式任务优先级
#define ESP_MODE_TASK_STACK_SIZE	128		//ESP模式任务堆栈大小
TaskHandle_t ESPModeTask_Handler;			//ESP模式任务句柄
void esp_mode_task(void* pvParameters);		//ESP模式任务函数
#define SHOW_ESP_UI                 (1 << 0)//UI显示事件位

//////////////////////////////////////////////////////////////
//					   -- <自动模式任务> --					//
//////////////////////////////////////////////////////////////
#define	AUTO_MODE_TASK_PRIORITY		2		//自动模式任务优先级
#define AUTO_MODE_TASK_STACK_SIZE	128		//自动模式任务堆栈大小
TaskHandle_t AutoModeTask_Handler;			//自动模式任务句柄
void auto_mode_task(void* pvParameters);	//自动模式任务函数
#define SHOW_AUTO_UI                (1 << 0)//UI显示事件位

//////////////////////////////////////////////////////////////
//					   -- <按键处理任务> --					//
//////////////////////////////////////////////////////////////
#define	KEY_PROCESS_TASK_PRIORITY	 3		//摇杆按键任务优先级
#define KEY_PROCESS_TASK_STACK_SIZE	 128	//摇杆按键任务堆栈大小
TaskHandle_t KeyProcessTask_Handler;		//摇杆按键任务句柄
void key_process_task(void* pvParameters);	//摇杆按键任务函数
#define SCAN_KEY                    (1 << 0)//扫描按键事件
#define RIGHT_KEY_LOCK              (1 << 1)//右键上锁事件

//////////////////////////////////////////////////////////////
//				   -- <NRF24L01发送任务> --					//
//////////////////////////////////////////////////////////////
#define SEND_TASK_PRIORITY			 4		//发送任务优先级
#define SEND_TASK_STACK_SIZE		 128	//发送任务堆栈大小
TaskHandle_t SendTask_Handler;				//发送任务句柄
void send_task(void* pvParameters);			//发送任务函数

//////////////////////////////////////////////////////////////
//					   -- <主处理任务> --					//
//////////////////////////////////////////////////////////////
#define MAIN_PROCESS_TASK_PRIORITY	 5		//主处理任务优先级
#define MAIN_PROCESS_TASK_STACK_SIZE 128	//主处理任务堆栈大小
TaskHandle_t MainProcessTask_Handler;		//主处理任务句柄
void main_process_task(void* pvParameters);	//主处理任务函数
#define SHOW_MAIN_UI                (1 << 0)//主UI显示事件位

//////////////////////////////////////////////////////////////
//					  -- <LED翻转任务> --					//
//////////////////////////////////////////////////////////////
#define LED_TOGGLE_TASK_PRIORITY	 6		//LED翻转任务优先级
#define LED_TOGGLE_TASK_STACK_SIZE	 128	//LED翻转任务堆栈大小
TaskHandle_t LEDToggleTask_Handler;			//LED翻转任务句柄
void led_toggle_task(void* pvParameters);	//LED翻转任务函数

#endif // !__MAIN_TASK_CONFIG_H
