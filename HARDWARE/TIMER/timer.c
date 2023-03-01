#include "timer.h"
#include "../../FreeRTOS/include/FreeRTOS.h"
#include "../../FreeRTOS/include/portable.h"
#include "../../FreeRTOS/include/queue.h"
#include "../../FreeRTOS/include/task.h"

//extern u16 USART3_RX_STA;
//FreeRTOS时间统计所用的节拍计数器
//volatile unsigned long long FreeRTOSRunTimeTicks;

/**
 * @brief	初始化TIM3使其为FreeRTOS的时间统计提供时基
 *//*
void ConfigureTimerForRunTimeStats(void) {
	//定时器3初始化，定时器时钟为72M，分频系数为72-1，所以定时器3的频率
	//为72M/72=1M，自动重装载为50-1，那么定时器周期就是50us
	FreeRTOSRunTimeTicks = 0;
	TIM3_Int_Init(50 - 1, 72 - 1);	//初始化TIM3
}*/

/**
 * @brief	定时器2初始化
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM2_Int_Init(u16 arr, u16 psc) {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能

	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 	//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //使能指定的TIM2中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9; //先占优先级4级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure); //初始化NVIC寄存器

	TIM_Cmd(TIM2, ENABLE); //使能TIM2
}

/**
 * @brief	定时器3初始化
 * @note	用于FreeRTOS的时间统计
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM3_Int_Init(u16 arr, u16 psc) {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级4级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx
}

/**
 * @brief	定时器3中断服务函数
 *//*
void TIM3_IRQHandler(void) {
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET) //溢出中断
		FreeRTOSRunTimeTicks++;
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除中断标志位
}*/

/**
 * @brief	定时器4初始化
 * @note	用于USART3接收数据定时
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM4_Int_Init(u16 arr, u16 psc) {
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//TIM4时钟使能    

	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //使能指定的TIM4中断,允许更新中断

	TIM_Cmd(TIM4, ENABLE);//开启定时器4

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
}

/**
 * @brief	定时器4中断服务程序
 */
// void TIM4_IRQHandler(void) {
// 	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) { //TIM4触发中断
// 		USART3_RX_STA |= 0x8000; //超时，强制标记串口3数据接收完成
// 		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //清除TIM4更新中断标志
// 		TIM_Cmd(TIM4, DISABLE);  //关闭TIM4
// 	}
// }
