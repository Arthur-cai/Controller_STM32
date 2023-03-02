#include "delay.h"

//如果需要使用OS,则包括下面的头文件即可
#if FREE_RTOS_SUPPORT
#include "FreeRTOS.h" //FreeRTOS使用
#include "task.h"
#endif // FREE_RTOS_SUPPORT

static u8  fac_us = 0;	//us延时倍乘数
static u16 fac_ms = 0;	//ms延时倍乘数,在os下,代表每个节拍的ms数

extern void xPortSysTickHandler(void);

#if FREE_RTOS_SUPPORT
//systick中断服务函数,使用os时用到
void SysTick_Handler(void) {
	//系统已经运行
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) xPortSysTickHandler();
}
#endif // FREE_RTOS_SUPPORT

/**
 * @brief	初始化延时函数
 * @note	SYSTICK的时钟固定为HCLK时钟的1/8
			系统时钟当使用OS的时候，此函数会初始化OS的时钟节拍
*/
void delay_init() {
#if FREE_RTOS_SUPPORT  //如果需要支持OS
	u32 reload;
	fac_us = SystemCoreClock / 1000000;
#else
	fac_us = SystemCoreClock / 8000000;	//为系统时钟的1/8  
#endif // FREE_RTOS_SUPPORT
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); //选择外部时钟 HCLK/8
	
#if FREE_RTOS_SUPPORT  //如果需要支持OS
	reload = SystemCoreClock / 1000000;		//每秒钟的计数次数 72000000/1000000 = 72Hz
	reload *= 1000000 / configTICK_RATE_HZ;	/* 根据configTICK_RATE_HZ设定溢出时间, reload为24位寄存器, 最大值:16777216
												在72M下,约合0.233s左右	*/
	fac_ms = 1000 / configTICK_RATE_HZ;	//代表OS可以延时的最少单位	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;  //开启SYSTICK中断
	SysTick->LOAD = reload; 					//每1/configTICK_RATE_HZ秒中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   //开启SYSTICK   
#else
	fac_ms = (u16)fac_us * 1000; //非OS下,代表每个ms需要的systick时钟数   
#endif // FREE_RTOS_SUPPORT
}

/**
 * @brief	微秒延时
 * @param	nus，微秒数
*/
void delay_us(u32 nus) {
#if FREE_RTOS_SUPPORT
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD; //LOAD的值	    	 
	ticks = nus * fac_us;		//需要的节拍数 
	told = SysTick->VAL;		//刚进入时的计数器值
	while (1) {
		tnow = SysTick->VAL;
		if (tnow != told) {
			if (tnow < told) tcnt += told - tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks) break;			//时间超过/等于要延迟的时间,则退出.
		}
	}
#else
	u32 temp;
	SysTick->LOAD = nus * fac_us; 				//时间加载	  		 
	SysTick->VAL = 0x00;        				//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//开始倒数	  
	do {
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16)));//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0X00;      					//清空计数器	
#endif
}

/**
 * @brief	毫秒延时
 * @param	nms，毫秒数。对72M条件下，nms<=1864
 * @note	SysTick->LOAD为24位寄存器，最大延时为：nms<=0xffffff*8*1000/SYSCLK
*/
void delay_ms(u16 nms) {
#if FREE_RTOS_SUPPORT //如果支持os
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) { //系统已经运行
		if (nms >= fac_ms)	//延时的时间大于OS的最少时间周期 
			vTaskDelay(nms / fac_ms); //FreeRTOS延时
		nms %= fac_ms; //OS已经无法提供这么小的延时了,采用普通方式延时    
	}
	delay_us((u32)(nms * 1000)); //普通方式延时
#else
	u32 temp;
	SysTick->LOAD = (u32)nms * fac_ms;			//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;						//清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	//开始倒数  
	do {
		temp = SysTick->CTRL;
	} while ((temp & 0x01) && !(temp & (1 << 16)));//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL = 0X00;       					//清空计数器
#endif
}

/**
 * @brief	毫秒延时，直接调用delay_us，不会引起任务调度
*/
void delay_xms(u32 nms) {
	u32 i;
	for (i = 0; i < nms; i++) delay_us(1000);
}
