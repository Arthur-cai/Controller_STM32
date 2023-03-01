#ifndef __I2C_H
#define __I2C_H	 
#include "../sys/sys.h"

#define I2CPORT		GPIOB	//定义IO接口
#define I2C_SCL		GPIO_Pin_6	//定义IO接口
#define I2C_SDA		GPIO_Pin_7	//定义IO接口

#define HostAddress	0xc0	//总线主机的器件地址
#define BusSpeed	200000	//总线速度（不高于400000）

/**
 * @brief	IIC初始化
*/
void I2C_Configuration(void);

/**
 * @brief	IIC发送一串数据
 * @param	SlaveAddr, 从机地址
 * @param	WriteAddr, 内部数据或命令地址
 * @param	pBuffer, 数据
 * @param	NumByteToWrite, 数据长度
*/
void I2C_SEND_BUFFER(u8 SlaveAddr, u8 WriteAddr, u8* pBuffer, u16 NumByteToWrite);

/**
 * @brief	IIC发送个字节
 * @param	SlaveAddr, 从机地址
 * @param	WriteAddr, 内部数据或命令地址
 * @param	pBuffer, 数据
*/
void I2C_SEND_BYTE(u8 SlaveAddr,u8 writeAddr,u8 pBuffer);

/**
 * @brief	IIC读一串数据
 * @param	SlaveAddr, 从机地址
 * @param	readAddr, 读地址
 * @param	pBuffer, 数据
 * @param	NumByteToWrite, 数据长度
*/
void I2C_READ_BUFFER(u8 SlaveAddr,u8 readAddr,u8* pBuffer,u16 NumByteToRead);

/**
 * @brief	IIC读一个字节
 * @param	SlaveAddr, 从机地址
 * @param	readAddr, 读地址
 * @retval	读取到的字节
*/
u8 I2C_READ_BYTE(u8 SlaveAddr,u8 readAddr);
		 				    
#endif
