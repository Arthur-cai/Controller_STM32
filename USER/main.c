//////////////////////////// <头文件> ////////////////////////////
/** 
 * 注意：在FreeRTOSConfig.h文件内，定义了时间频率configTICK_RATE_HZ为100，即时间周期10ms
 *		以及宏定义C8T6_OR_ZET6置0，选择C8T6作为MCU，初始化相应的堆大小
*/
#include "../BASIC/sys/sys.h"
#include "../BASIC/usart/usart.h"
#include "../BASIC/delay/delay.h"
#include "../HARDWARE/LED/led.h"
#include "../HARDWARE/OLED096/oled096.h"
#include "../HARDWARE/NRF24L01/24l01.h"
#include "../HARDWARE/JOY_STICK/joy_stick.h"
#include "main_task_config.h"
//////////////////////////// <头文件> ////////////////////////////

//////////////////////////// <变量> ////////////////////////////
static u8 CtrlMode = 1;						//当前控制模式
u8 com[3] = { NONE_CONTROL_SIGNAL, 0, 0 };	//命令数组
//////////////////////////// <变量> ////////////////////////////

/**
 * @brief	NRF模式UI界面
*/
static void NRF_UI(void) {
	OLED_Clear();
	OLED_ShowStr(0, 0, "NRF24L01 Mode");
	OLED_ShowStr(2, 0, "Lx:    , Ry:");
}

/**
 * @brief	ESP模式UI界面
*/
static void ESP_UI(void) {
	OLED_Clear();
	OLED_ShowStr(0, 0, "ESP8266 Mode");
}

/**
 * @brief	自动模式UI界面
*/
static void Auto_UI(void) {
	OLED_Clear();
	OLED_ShowStr(0, 0, "Auto Mode");
}

/**
 * @brief	主UI界面
*/
static void MainUI(void) {
	OLED_Clear();
	OLED_ShowStr(0, 0, "Select CtrlMode");
	OLED_ShowArrow((CtrlMode * 2), 3 * 8, 1);
	OLED_ShowStr(2, 5 * 8, "NRF24L01");
	OLED_ShowStr(4, 5 * 8, "ESP8266");
	OLED_ShowStr(6, 5 * 8, "Auto");
}

/**
 * @brief	主函数
 */
int main(void) {
	NVIC_Configuration();					/* NVIC优先级分组配置为 NVIC_PriorityGroup_4 */
	delay_init();							/* 延时初始化 */
	delay_xms(100);							/* 延时100ms */
	USART1_Init(115200);					/* 初始化串口1 */
	LED_Init();								/* LED初始化 */
	NRF24L01_Init();						/* NRF24L01初始化 */
	JOY_STICK_Init();						/* 摇杆初始化 */
	I2C_Configuration();					/* IIC初始化 */
	OLED_Init();							/* OLED初始化 */

	xTaskCreate(							/* 创建开始任务 */
		(TaskFunction_t)start_task,			/* 任务函数 */
		(const char*)"start_task",			/* 任务名称 */
		(uint16_t)START_TASK_STACK_SIZE,	/* 任务堆栈大小 */
		(void*)NULL,						/* 传递的参数 */
		(UBaseType_t)START_TASK_PRIORITY,	/* 任务优先级 */
		(TaskHandle_t*)&StartTask_Handler	/* 任务句柄 */
	);
	vTaskStartScheduler();					/* 开启任务调度 */
}

/**
 * @brief	开始任务
*/
void start_task(void* pvParameters) {
	taskENTER_CRITICAL();						/* 进入临界区 */
	xTaskCreate(								/* 创建NRF模式任务 */
		(TaskFunction_t)nrf_mode_task,			/* 任务函数 */
		(const char*)"nrf_mode_task",			/* 任务名称 */
		(uint16_t)NRF_MODE_TASK_STACK_SIZE,		/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)NRF_MODE_TASK_PRIORITY,	/* 任务优先级 */
		(TaskHandle_t*)&NRFModeTask_Handler		/* 任务句柄 */
	);
	xTaskCreate(								/* 创建ESP模式任务 */
		(TaskFunction_t)esp_mode_task,			/* 任务函数 */
		(const char*)"esp_mode_task",			/* 任务名称 */
		(uint16_t)ESP_MODE_TASK_STACK_SIZE,		/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)ESP_MODE_TASK_PRIORITY,	/* 任务优先级 */
		(TaskHandle_t*)&ESPModeTask_Handler		/* 任务句柄 */
	);
	xTaskCreate(								/* 创建自动模式任务 */
		(TaskFunction_t)auto_mode_task,			/* 任务函数 */
		(const char*)"auto_mode_task",			/* 任务名称 */
		(uint16_t)AUTO_MODE_TASK_STACK_SIZE,	/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)AUTO_MODE_TASK_PRIORITY,	/* 任务优先级 */
		(TaskHandle_t*)&AutoModeTask_Handler	/* 任务句柄 */
	);
	xTaskCreate(								/* 创建主处理任务 */
		(TaskFunction_t)main_process_task,		/* 任务函数 */
		(const char*)"main_process_task",		/* 任务名称 */
		(uint16_t)MAIN_PROCESS_TASK_STACK_SIZE,	/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)MAIN_PROCESS_TASK_PRIORITY,/* 任务优先级 */
		(TaskHandle_t*)&MainProcessTask_Handler	/* 任务句柄 */
	);
	xTaskCreate(								/* 创建按键处理任务 */
		(TaskFunction_t)key_process_task,		/* 任务函数 */
		(const char*)"key_process_task",		/* 任务名称 */
		(uint16_t)KEY_PROCESS_TASK_STACK_SIZE,	/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)KEY_PROCESS_TASK_PRIORITY,	/* 任务优先级 */
		(TaskHandle_t*)&KeyProcessTask_Handler	/* 任务句柄 */
	);
	xTaskCreate(								/* 创建发送任务 */
		(TaskFunction_t)send_task,				/* 任务函数 */
		(const char*)"send_task",				/* 任务名称 */
		(uint16_t)SEND_TASK_STACK_SIZE,			/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)SEND_TASK_PRIORITY,		/* 任务优先级 */
		(TaskHandle_t*)&SendTask_Handler		/* 任务句柄 */
	);
	xTaskCreate(								/* 创建LED翻转任务 */
		(TaskFunction_t)led_toggle_task,		/* 任务函数 */
		(const char*)"led_toggle_task",			/* 任务名称 */
		(uint16_t)LED_TOGGLE_TASK_STACK_SIZE,	/* 任务堆栈大小 */
		(void*)NULL,							/* 传递的参数 */
		(UBaseType_t)LED_TOGGLE_TASK_PRIORITY,	/* 任务优先级 */
		(TaskHandle_t*)&LEDToggleTask_Handler	/* 任务句柄 */
	);
	vTaskSuspend(NRFModeTask_Handler);			/* 挂起NRF模式任务 */
	vTaskSuspend(ESPModeTask_Handler);			/* 挂起ESP模式任务 */
	vTaskSuspend(AutoModeTask_Handler);			/* 挂起自动模式任务 */
	xTaskNotify(								/* 发送显示主UI事件 */
		MainProcessTask_Handler, SHOW_MAIN_UI, eSetBits);
	vTaskDelete(StartTask_Handler);				/* 删除开始任务 */
	taskEXIT_CRITICAL();						/* 退出临界区 */
}

/**
 * @brief	NRF模式任务
*/
void nrf_mode_task(void* pvParameters) {
	u32 notify = 0;			//通知值
	BaseType_t state = 0;	//通知获取状态
	while (TRUE) {
		state = xTaskNotifyWait(				/* 接收NRF模式UI显示事件 */
			0, ULONG_MAX, &notify, 1);
		if (state == pdPASS && notify & SHOW_NRF_UI) {
			NRF_UI();							/* 显示NRF模式UI */
		}
		Motion_Judge(com);						/* 获取摇杆状态 */
		OLED_ShowNum(2, 3 * 8, com[1], 3);		/* 显示左摇杆状态值 */
		OLED_ShowNum(2, 12 * 8, com[2], 3);		/* 显示右摇杆状态值 */
		com[0] = CONTROL_BY_NRF24L01;			/* com数组赋为NRF控制信号 */
		xTaskNotifyGive(SendTask_Handler);		/* 通知发送任务 */
		OLED_ShowNum(6, 8 * 8, com[0], 3);		/* 显示当前控制信号 */
		xTaskNotify(							/* 发送扫描按键事件 */
			KeyProcessTask_Handler, SCAN_KEY, eSetBits);
		vTaskDelay(10);							/* 延时10个时钟单位(100ms) */
	}
}

/**
 * @brief	ESP模式任务
*/
void esp_mode_task(void* pvParameters) {
	u32 notify = 0;			//通知值
	BaseType_t state = 0;	//通知获取状态
	while (TRUE) {
		state = xTaskNotifyWait(				/* 接收ESP模式UI显示事件 */
			0, ULONG_MAX, &notify, 1);
		if (state == pdPASS && notify & SHOW_ESP_UI) {
			ESP_UI();							/* 显示ESP模式UI */
		}
		com[0] = CONTROL_BY_ESP8266;			/* com数组赋为ESP控制信号 */
		xTaskNotifyGive(SendTask_Handler);		/* 通知发送任务 */
		OLED_ShowNum(6, 8 * 8, com[0], 3);		/* 显示当前控制信号 */
		xTaskNotify(							/* 发送扫描按键事件 */
			KeyProcessTask_Handler, SCAN_KEY, eSetBits);
		vTaskDelay(10);							/* 延时10个时钟单位(100ms) */
	}
}

/**
 * @brief	自动模式任务
*/
void auto_mode_task(void* pvParameters) {
	u32 notify = 0;			//通知值
	BaseType_t state = 0;	//通知获取状态
	while (TRUE) {
		state = xTaskNotifyWait(				/* 接收自动模式UI显示事件 */
			0, ULONG_MAX, &notify, 1);
		if (state == pdPASS && notify & SHOW_AUTO_UI) {
			Auto_UI();							/* 显示自动模式UI */
		}
		com[0] = CONTROL_BY_SELF;				/* com数组赋为自动控制信号 */
		xTaskNotifyGive(SendTask_Handler);		/* 通知发送任务 */
		OLED_ShowNum(6, 8 * 8, com[0], 3);		/* 显示当前控制信号 */
		xTaskNotify(							/* 发送扫描按键事件 */
			KeyProcessTask_Handler, SCAN_KEY, eSetBits);
		vTaskDelay(10);							/* 延时10个时钟单位(100ms) */
	}
}

/**
 * @brief	主处理任务
*/
void main_process_task(void* pvParameters) {
	u32 notify = 0;			//通知值
	BaseType_t state = 0;	//通知获取状态
	while (TRUE) {
		state = xTaskNotifyWait(				/* 接收主UI显示事件 */
			0, ULONG_MAX, &notify, 1);
		if (state == pdPASS && notify & SHOW_MAIN_UI) {
			MainUI();							/* 显示主UI */
		}
		com[0] = NONE_CONTROL_SIGNAL;			/* com数组赋为非控制信号 */
		xTaskNotifyGive(SendTask_Handler);		/* 通知发送任务 */
		Motion_Judge(com);						/* 获取摇杆状态 */
		switch (com[1]) {
		case FORWARD_COM: {						/* 如果摇杆为前推，则箭头向上滑动*/
			OLED_ShowArrow((CtrlMode * 2), 3 * 8, 0);
			CtrlMode--;
			if (CtrlMode < 1) CtrlMode = 3;
			OLED_ShowArrow((CtrlMode * 2), 3 * 8, 1);
			break;
		}
		case BACKWARD_COM: {					/* 如果摇杆为后推，则箭头向下滑动*/
			OLED_ShowArrow((CtrlMode * 2), 3 * 8, 0);
			CtrlMode++;
			if (CtrlMode > 3) CtrlMode = 1;
			OLED_ShowArrow((CtrlMode * 2), 3 * 8, 1);
			break;
		}
		default: break;
		}
		xTaskNotify(KeyProcessTask_Handler, 	/* 发送扫描按键和右键上锁事件 */
			SCAN_KEY | RIGHT_KEY_LOCK, eSetBits);
		vTaskDelay(10);							/* 延时10个时钟单位(100ms) */
	}
}

/**
 * @brief	按键处理任务
*/
void key_process_task(void* pvParameters) {
	u32 notify = 0;				//通知值
	BaseType_t state = 0;		//通知获取状态
	u8 key = 0;					//按键值
	u8 RIGHT_KEY_LOCK_FLAG = 0; //右键上锁标记
	while (TRUE) {
		state = xTaskNotifyWait(0, ULONG_MAX, &notify, portMAX_DELAY);	/* 获取任务事件 */
		if (state == pdPASS && notify & SCAN_KEY) {						/* 接收到扫描按键事件 */

			if (notify & RIGHT_KEY_LOCK) RIGHT_KEY_LOCK_FLAG = 1;		/* 如果接收到右键上锁事件 */
			else RIGHT_KEY_LOCK_FLAG = 0;								/* 否则右键解锁 */

			key = STICK_KEY_Status(NOT_CONTINUE);						/* 扫描按键状态 */
			if (RIGHT_KEY_LOCK_FLAG && key == LEFT_STICK_KEY_PRESS) {	/* 如果按下左键 */
				vTaskSuspend(MainProcessTask_Handler);					/* 挂起主处理任务 */
				if (CtrlMode == 1) {									/* 当前为NRF模式 */
					xTaskNotify(
						NRFModeTask_Handler, SHOW_NRF_UI, eSetBits);	/* 显示NRF模式UI事件 */
					vTaskResume(NRFModeTask_Handler);					/* 恢复NRF模式任务 */
				}
				if (CtrlMode == 2) {									/* 当前为ESP模式 */
					xTaskNotify(
						ESPModeTask_Handler, SHOW_ESP_UI, eSetBits);	/* 显示ESP模式UI事件 */
					vTaskResume(ESPModeTask_Handler);					/* 恢复ESP模式任务 */
				}
				if (CtrlMode == 3) {									/* 当前为自动模式 */
					xTaskNotify(
						AutoModeTask_Handler, SHOW_AUTO_UI, eSetBits);	/* 显示自动模式UI事件 */
					vTaskResume(AutoModeTask_Handler);					/* 恢复自动模式任务 */
				}
			}
			if (!RIGHT_KEY_LOCK_FLAG && key == RIGHT_STICK_KEY_PRESS) {	/* 如果按下右键 */
				if (CtrlMode == 1) {									/* 当前为NRF模式 */
					vTaskSuspend(NRFModeTask_Handler);					/* 挂起NRF模式任务 */
				}
				if (CtrlMode == 2) {									/* 当前为ESP模式 */
					vTaskSuspend(ESPModeTask_Handler);					/* 挂起ESP模式任务 */
				}
				if (CtrlMode == 3) {									/* 当前为自动模式 */
					vTaskSuspend(AutoModeTask_Handler);					/* 挂起自动模式任务 */
				}
				xTaskNotify(
					MainProcessTask_Handler, SHOW_MAIN_UI, eSetBits);	/* 显示主UI事件 */
				vTaskResume(MainProcessTask_Handler);					/* 恢复主处理任务 */
			}
		}
	}
}

/**
 * @brief	发送任务
*/
void send_task(void* pvParameters) {
	u32 notify = 0;
	u8 state;
	while (TRUE) {
		notify = ulTaskNotifyTake(pdFALSE, portMAX_DELAY);	/* 获取任务通知 */
		if (notify) {
			NRF24L01_TX_Mode();								/* 设置为发送模式 */
			state = NRF24L01_TxPacket(com);					/* 发送com数组 */
			if (state == TX_OK) OLED_ShowCircle(6, 0, 1);	/* 发送成功就显示菱形 */
			else OLED_ShowCircle(6, 0, 0);					/* 发送失败不显示 */
		}
	}
}

/**
 * @brief	LED翻转任务
*/
void led_toggle_task(void* pvParameters) {
	while (TRUE) {
		LED = !LED;
		vTaskDelay(50);
	}
}

