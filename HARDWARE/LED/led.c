#include "led.h"

/**
 * @brief	初始化LED
*/
void LED_Init(void) {
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_RCC_CLOCK, ENABLE);		//使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = LED_PIN;				//PB8端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//IO口速度为50MHz
	GPIO_Init(LED_PORT, &GPIO_InitStructure);
	GPIO_SetBits(LED_PORT, LED_PIN);
}

