#ifndef __JOY_STICK_H
#define	__JOY_STICK_H
#include "../../BASIC/sys/sys.h"
#include "../NRF24L01/24l01.h"
#include "../../BASIC/delay/delay.h"

//摇杆adc值对应运动形式的判断阈值
#define FORWARD_THRESHOLD	1800		//小于该值即为前进
#define BACKWARD_THRESHOLD	2150		//大于该值即为后退
#define LEFT_THRESHOLD		2150		//大于该值即为左转
#define RIGHT_THRESHOLD		1800		//小于该值即为右转

//用于DMA访问ADC1
#define ADC1_DR_ADDR ((uint32_t)0x4001244C) //ADC1这个外设的地址（查参考手册得出）
extern vu16 STICK_ADC_BUFFER[4];		//ADC连续10次平均数值

#define LEFT_STICK_PORT		GPIOA		//左摇杆端口
#define LEFT_STICK_X_PIN	GPIO_Pin_0	//左摇杆X轴输入引脚PA0
#define LEFT_STICK_Y_PIN	GPIO_Pin_1	//左摇杆Y轴输入引脚PA1
#define LEFT_STICK_KEY_PIN	GPIO_Pin_2	//左摇杆按键输入引脚PA2

#define RIGHT_STICK_PORT	GPIOB		//右摇杆端口
#define	RIGHT_STICK_X_PIN	GPIO_Pin_0	//左摇杆X轴输入引脚PB0
#define	RIGHT_STICK_Y_PIN	GPIO_Pin_1	//左摇杆Y轴输入引脚PB1
#define	RIGHT_STICK_KEY_PIN	GPIO_Pin_10	//左摇杆X轴输入引脚PB10

#define LEFT_STICK_KEY	(!GPIO_ReadInputDataBit(LEFT_STICK_PORT, LEFT_STICK_KEY_PIN))
#define RIGHT_STICK_KEY	(!GPIO_ReadInputDataBit(RIGHT_STICK_PORT, RIGHT_STICK_KEY_PIN))

//定义连续模式枚举类型
typedef enum _continue_mode_enum {
	NOT_CONTINUE = 0,	/* 连按 */
	CONTINUE			/* 非连按 */
}ContinueMode;

//定义摇杆按键枚举类型
typedef enum _stick_key_enum {
	NONE_STICK = 0,			/* 无按键 */
	LEFT_STICK_KEY_PRESS,	/* 左键按下 */
	RIGHT_STICK_KEY_PRESS	/* 右键按下 */
}StickKeyType;

/**
 * @brief	摇杆初始化，x和y轴的数据由adc读取
*/
void JOY_STICK_Init(void);

/**
 * @brief	调用GET_AverageAdcValue()，判断阈值，返回对应运动方式的命令
 * @param	com，存放命令的数组
			FORWARD_COM		0xA0，
			BACKWARD_COM	0x20，
			LEFT_COM		0x0E，
			RIGHT_COM		0x06
*/
void Motion_Judge(u8* com);	

/**
 * @brief	读取摇杆按键值
 * @param	mode	连续模式，CONTINUE连按，NOT_CONTINUE非连按
 * @retval	按键按下情况		LEFT_STICK_KEY_PRESS 或 RIGHT_STICK_KEY_PRESS
*/
StickKeyType STICK_KEY_Status(ContinueMode mode);

#endif // !__JOY_STICK_H
