#ifndef __LED_H
#define __LED_H	 
#include "../../BASIC/sys/sys.h"

#define	LED_PIN			GPIO_Pin_13
#define LED_PORT		GPIOC
#define LED_RCC_CLOCK	RCC_APB2Periph_GPIOC

#define LED PCout(13)

void LED_Init(void);//初始化

#endif
