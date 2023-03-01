#ifndef __TIMER_H
#define __TIMER_H
#include "../BASIC/sys/sys.h"
#include "../BASIC/usart/usart.h"
#include "string.h"

//extern volatile unsigned long long FreeRTOSRunTimeTicks;
//void ConfigureTimerForRunTimeStats(void);

/**
 * @brief	定时器2初始化
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM2_Int_Init(u16 arr, u16 psc);

/**
 * @brief	定时器3初始化
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM3_Int_Init(u16 arr, u16 psc);

/**
 * @brief	定时器4初始化
 * @param	arr，计数器值
 * @param	psc，预分频值
 */
void TIM4_Int_Init(u16 arr, u16 psc);

#endif // !__TIMER_H
