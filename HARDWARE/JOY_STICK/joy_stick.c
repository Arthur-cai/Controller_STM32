#include "joy_stick.h"

vu16 STICK_ADC_DMA_IN[4]; //DMA读取ADC暂存数据
vu16 STICK_ADC_BUFFER[4]; //ADC连续10次平均数值

/**
 * @brief	DMA的初始化，用于读取ADC
*/
static void STICK_DMA_Init(void) { //DMA初始化设置
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);//复位DMA通道1

	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDR; //定义 DMA通道外设基地址=ADC1_DR_Address
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&STICK_ADC_DMA_IN; //定义DMA通道ADC数据存储器（其他函数可直接读此变量即是ADC值）
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//指定外设为源地址
	DMA_InitStructure.DMA_BufferSize = 4;//定义DMA缓冲区大小（根据ADC采集通道数量修改）
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//当前外设寄存器地址不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//当前存储器地址：Disable不变，Enable递增（用于多通道采集）
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//定义外设数据宽度16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //定义存储器数据宽度16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA通道操作模式位环形缓冲模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//禁止DMA通道存储器到存储器传输
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//初始化DMA通道1

	DMA_Cmd(DMA1_Channel1, ENABLE); //使能DMA通道1
}

/**
 * @brief	摇杆引脚的初始化，x和y轴的数据由adc读取
*/
static void STICK_GPIO_Init(void) { //两个按键的x轴y轴GPIO接口初始化设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
		RCC_APB2Periph_ADC1, ENABLE); //使能GPIOA、GPIOB和ADC1时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);//使能DMA时钟（用于ADC的数据传送）

	//RCC_ADCCLKConfig(RCC_PCLK2_Div6);  //6分频
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); //8分频, 72/8 = 9Mhz,DMA是高速设备，采样时间太快会有问题

	//左摇杆adc输入引脚配置
	GPIO_InitStructure.GPIO_Pin = LEFT_STICK_X_PIN | LEFT_STICK_Y_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入     
	GPIO_Init(LEFT_STICK_PORT, &GPIO_InitStructure);
	//右摇杆adc输入引脚配置
	GPIO_InitStructure.GPIO_Pin = RIGHT_STICK_X_PIN | RIGHT_STICK_Y_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //模拟输入  
	GPIO_Init(RIGHT_STICK_PORT, &GPIO_InitStructure);

	//左摇杆按键初始化
	GPIO_InitStructure.GPIO_Pin = LEFT_STICK_KEY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(LEFT_STICK_PORT, &GPIO_InitStructure);
	//右摇杆按键初始化
	GPIO_InitStructure.GPIO_Pin = RIGHT_STICK_KEY_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//上拉输入
	GPIO_Init(RIGHT_STICK_PORT, &GPIO_InitStructure);
}

/**
 * @brief	ADC1的初始化
 * @note	ADC要开启连续读取多通道的数据必须开启扫描模式
*/
static void STICK_ADC1_Init(void) { //初始化设置
	ADC_InitTypeDef ADC_InitStructure;

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //使能扫描
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//ADC转换工作在连续模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//有软件控制转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//转换数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 4;//顺序进行规则转换的ADC通道的数目（根据ADC采集通道数量修改）
	ADC_Init(ADC1, &ADC_InitStructure); //根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	//ADC1,ADC通道x,规则采样顺序值为y,采样时间为28周期，采样时间t = 1/ADCCLK * 239.5	 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);//ADC1选择信道x,采样顺序y,采样时间n个周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_28Cycles5);
}

/**
 * @brief	摇杆初始化，x和y轴的数据由adc读取
*/
void JOY_STICK_Init(void){ 
	STICK_GPIO_Init();
	STICK_DMA_Init();
	STICK_ADC1_Init();
	ADC_DMACmd(ADC1, ENABLE);// 开启ADC的DMA支持
	ADC_Cmd(ADC1, ENABLE);//使能ADC1
	ADC_ResetCalibration(ADC1); //重置ADC1校准寄存器
	while (ADC_GetResetCalibrationStatus(ADC1));//等待ADC1校准重置完成
	ADC_StartCalibration(ADC1);//开始ADC1校准
	while (ADC_GetCalibrationStatus(ADC1));//等待ADC1校准完成
	ADC_SoftwareStartConvCmd(ADC1, ENABLE); //使能ADC1软件开始转换
}

/**
 * @brief	连续10次读取ADC值取平均
*/
static void GET_AverageAdcValue(void) {
	u8 i;
	u16 res[4] = {0};

	for (i = 0; i < 10; i++) {
		res[0] += STICK_ADC_DMA_IN[0];
		res[1] += STICK_ADC_DMA_IN[1];
		res[2] += STICK_ADC_DMA_IN[2];
		res[3] += STICK_ADC_DMA_IN[3];
		delay_ms(5);
	}
	STICK_ADC_BUFFER[0] = res[0] / 10;
	STICK_ADC_BUFFER[1] = res[1] / 10;
	STICK_ADC_BUFFER[2] = res[2] / 10;
	STICK_ADC_BUFFER[3] = res[3] / 10;
}

/**
 * @brief	调用GET_AverageAdcValue()，判断阈值，返回对应运动方式的命令
 * @param	com，存放命令的数组 @ref FORWARD_COM, BACKWARD_COM, LEFT_COM, RIGHT_COM
*/
void Motion_Judge(u8* com) {
	GET_AverageAdcValue();
	if (STICK_ADC_BUFFER[0] < FORWARD_THRESHOLD) com[1] = FORWARD_COM;
	else if (STICK_ADC_BUFFER[0] > BACKWARD_THRESHOLD) com[1] = BACKWARD_COM;
	else com[1] = 0x00;

	if (STICK_ADC_BUFFER[3] > LEFT_THRESHOLD) com[2] = LEFT_COM;
	else if (STICK_ADC_BUFFER[3] < RIGHT_THRESHOLD) com[2] = RIGHT_COM;
	else com[2] = 0x00;
}

/**
 * @brief	读取摇杆按键值
 * @param	mode, 连续模式 @ref CONTINUE连按，NOT_CONTINUE非连按
 * @retval	按键按下情况 @ref LEFT_STICK_KEY_PRESS 或 RIGHT_STICK_KEY_PRESS
*/
StickKeyType STICK_KEY_Status(ContinueMode mode) {
	static u8 KEY_UP_FLAG = 1; //按键送开标记
	StickKeyType key = NONE_STICK;

	if (mode == CONTINUE) KEY_UP_FLAG = 1;
	if (KEY_UP_FLAG && (LEFT_STICK_KEY || RIGHT_STICK_KEY)) {
		KEY_UP_FLAG = 0;
		delay_ms(20);
		if (LEFT_STICK_KEY) { key = LEFT_STICK_KEY_PRESS; }
		else if(RIGHT_STICK_KEY) { key = RIGHT_STICK_KEY_PRESS; }
	}
	else if (!LEFT_STICK_KEY && !RIGHT_STICK_KEY) KEY_UP_FLAG = 1;
	return key;
}
