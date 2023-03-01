#ifndef __DELAY_H
#define __DELAY_H 			   
#include "../sys/sys.h"
	 
/**
 * @brief	初始化延时函数
 * @note	SYSTICK的时钟固定为HCLK时钟的1/8
			系统时钟当使用OS的时候，此函数会初始化OS的时钟节拍
*/
void delay_init(void);

/**
 * @brief	微秒延时
 * @param	nus，微秒数
*/
void delay_us(u32 nus);

/**
 * @brief	毫秒延时
 * @param	nms，毫秒数。对72M条件下，nms<=1864
 * @note	SysTick->LOAD为24位寄存器，最大延时为：nms<=0xffffff*8*1000/SYSCLK
*/
void delay_ms(u16 nms);

/**
 * @brief	毫秒延时，直接调用delay_us，不会引起任务调度
 * @param	nms，毫秒数
*/
void delay_xms(u32 nms);

#endif
