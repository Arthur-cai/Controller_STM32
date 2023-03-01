#include "oled096.h"
#include "ascii_8x16.h"

/**
 * @brief 通过I2C总线发送指令数组到指定从机
 * @param slaveAddr 从机地址
 * @param internalAddr 从机内部指令或数据区地址
 * @param command 指令数组
 * @param size 数组长度
*/
static void WriteMem_Buffer(u8 slaveAddr, u8 internalAddr, u8* command, u16 size) {
	I2C_GenerateSTART(I2C1, ENABLE); //产生开始信号
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == RESET); //确认进入主机模式
	I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter); //传输从机7位地址，主机作为发送方
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == RESET); //确认已经发送完成
	I2C_SendData(I2C1, internalAddr); //开始通过iic传输到从机的指定位置
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET); //确认字节传输完成
	while(size--) {
		I2C_SendData(I2C1, *command++); //传输指令
		while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET); //确认字节传输完成
	}
	I2C_GenerateSTOP(I2C1, ENABLE); //产生停止信号
}

/**
 * @brief 通过I2C总线发送数据到指定从机
 * @param slaveAddr 从机地址
 * @param internalAddr 从机内部指令或数据区地址
 * @param data 数据
*/
static void WriteMem(u8 slaveAddr, u8 internalAddr, u8 data) {
	I2C_GenerateSTART(I2C1, ENABLE); //产生开始信号
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == RESET); //确认进入主机模式
	I2C_Send7bitAddress(I2C1, slaveAddr, I2C_Direction_Transmitter); //传输从机7位地址，主机作为发送方
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == RESET); //确认已经发送完成
	I2C_SendData(I2C1, internalAddr); //开始通过iic传输到从机的指定位置
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET); //确认字节传输完成
	I2C_SendData(I2C1, data); //传输指令
	while(I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == RESET); //确认字节传输完成
	I2C_GenerateSTOP(I2C1, ENABLE); //产生停止信号
}

/**
 * @brief OLED初始化，包含了OLED_Display_On(),OLED_Display_Off(),OLED_Clear()
*/
void OLED_Init(void) { 
	I2C_Configuration();
	OLED_Display_Off();
	OLED_Clear();
	OLED_Display_On();
}

/**
 * @brief 通过发送指令点亮oled
*/
void OLED_Display_On(void) { 
	u8 com[] = {
		0xae,0x00,0x10,0xd5,
		0x80,0xa8,0x3f,0xd3,
		0x00,0xb0,0x40,0x8d,
		0x14,0xa1,0xc8/*从下往上扫描*/,0xda,
		0x12,0x81,0xbf,0xd9,
		0xf1,0xdb,0x30,0x20,
		0x00,0xa4,0xa6,0xaf
	};
	WriteMem_Buffer(OLED_ADDR,COM,com,sizeof(com));
}

/**
 * @brief 通过发送指令熄灭oled
*/
void OLED_Display_Off(void) { 
	u8 com[] = {0xae,0x8d,0x10};
	WriteMem_Buffer(OLED_ADDR,COM,com,sizeof(com));
}

/**
 * @brief 改变OLED对比度，可以看作亮度
 * @param command 要发送的指令，对比度范围00~FF
*/
void OLED_Brightness(u8 command) { 
	WriteMem(OLED_ADDR,COM,0x81);
	WriteMem(OLED_ADDR,COM,command);
}

/**
 * @brief 设置OLED渲染的起点
 * @param row 行数(0，2，4，6)
 * @param col 列数(根据 前面已有字符数*8 往后移)
*/
static void OLED_StartPoint(u8 row, u8 col) { 
	WriteMem(OLED_ADDR, COM, 0xB0+row); //设置页地址
	WriteMem(OLED_ADDR, COM, (col&0x0F)|0x00); //取列低位
	WriteMem(OLED_ADDR, COM, ((col&0xF0)>>4)|0x10); //取列高位
}

/**
 * @brief 填充全部区域
 * @param color 颜色
*/
void OLED_FillAll(u8 color) {
	u8 row; //行	
	u8 column; //列
	//遍历每一页，共8大行
	for (row = 0xB0; row <= 0xB7; row++) {
		OLED_StartPoint(row - 0xB0, 0);
		//遍历每一列，共END_COL列
		for (column = 0; column < END_COL; column++) WriteMem(OLED_ADDR, DAT, color); //填充颜色
	}
}

/**
 * @brief 调用OLED_FillAll(WHITE)实现清屏
*/
void OLED_Clear(void) { 
	OLED_FillAll(white);
}

/**
 * @brief 清除指定区域，WHITE填充
 * @param row 行数（0，2，4，6）
 * @param col 列数(根据 前面已有字符数*8 往后移，注意OLED芯片定义了列数偏移2列）
*/
void OLED_DeleteArea(u8 row, u8 start, u8 end) {
	u8 i;
	u8 startCol = start*8 + OFFSET;
	u8 endFlag = end*8 + OFFSET;

	OLED_StartPoint(row, startCol);
	for(i = startCol; i < endFlag; i++) WriteMem(OLED_ADDR, DAT, white);
	OLED_StartPoint(row+1, startCol);
	for(i = startCol; i < endFlag; i++) WriteMem(OLED_ADDR, DAT, white);
}

/**
 * @brief 清除指定的行列之后的所有内容，以white填充
 * @param row 行数（0，2，4，6）
 * @param col 列数(根据 前面已有字符数*8 往后移，注意OLED芯片定义了列数偏移2列）
*/
void OLED_DeleteFrom(u8 row, u8 col) { 
	u8 cycleCount = (END_ROW - row) / 2 + 1; //循环计数
	u8 i;
	u8 startCol = col*8 + OFFSET; //起始列
	u8 startRow = row;	//起始行
	
	while(cycleCount--) {
		OLED_StartPoint(startRow, startCol);
		for(i = startCol; i < END_COL; i++) WriteMem(OLED_ADDR, DAT, white);
		OLED_StartPoint(startRow+1, startCol);
		for(i = startCol; i < END_COL; i++) WriteMem(OLED_ADDR, DAT, white);
		//从下一次循环开始，就换行并从第0列开始
		startRow += 2;
		startCol = 0;
	}
}

/**
 * @brief 显示字符
 * @param row 行数(0，2，4，6)
 * @param col 列数(根据 前面已有字符数*8 往后移，注意OLED芯片定义了列数偏移2列）
 * @param ch 字符
*/
void OLED_ShowChar(u8 row, u8 col, u8 ch) {
	u8 i;	
	col += OFFSET;
	ch -= 32;
	
	OLED_StartPoint(row, col);
	for (i = 0; i < 8; i++)	WriteMem(OLED_ADDR, DAT, ASCII_8x16[ch*16+i]);
	OLED_StartPoint(row+1, col);
	for (i = 0; i < 8; i++)	WriteMem(OLED_ADDR, DAT, ASCII_8x16[ch*16+i+8]);
}

/**
 * @brief 显示字符串
 * @param row 行数(0，2，4，6)
 * @param col 列数(根据 前面已有字符数*8 往后移，注意OLED芯片定义了列数偏移2列）
 * @param str 字符串
*/
void OLED_ShowStr(u8 row, u8 col, u8* str) {
	while(*str) {
		//超过16列就换行
		if (col > END_COL - 8) {
			row += 2;
			col = 0;
		}
		OLED_ShowChar(row, col, *str);
		col += 8;
		str++;
	}
}

/**
 * @brief 实现m的n次方
*/
static u32 mypow(u8 m, u8 n) {
	u32 res = 1;
	while(n--) res *= m;
	return res;
}

/**
 * @brief 显示数字
 * @param row 行数(0，2，4，6)
 * @param col 列数(根据 前面已有字符数*8 往后移，注意OLED芯片定义了列数偏移2列）
 * @param num 数字
 * @param size 数字的位数
*/
void OLED_ShowNum(u8 row, u8 col, u32 num, u8 len) {
	u8 i;
	u8 temp; //取到的数
	u8 flag = 0;
	for (i = 0; i < len; i++) {
		temp = (num / mypow(10, len - 1 - i)) % 10;
		if (flag == 0 && i < (len - 1)) {
			if (temp == 0) {
				OLED_ShowChar(row, col + (i * 8), ' ');
				continue;
			}
			else flag = 1;
		}
		OLED_ShowChar(row, col + (i * 8), temp + '0');
	}
}

/**
 * @brief	显示箭头符号
 * @param	row，col，行列
 * @param	mode，1表示显示，0表示清除
*/
void OLED_ShowArrow(u8 row, u8 col, u8 mode) { 
	u8 i;

	OLED_StartPoint(row, col);
	for (i = 0; i < 8; i++) {
		if (mode) WriteMem(OLED_ADDR, DAT, ARROW[i]);
		else WriteMem(OLED_ADDR, DAT, white);
	}
	OLED_StartPoint(row + 1, col);
	for (i = 0; i < 8; i++) {
		if (mode) WriteMem(OLED_ADDR, DAT, ARROW[i + 8]);
		else WriteMem(OLED_ADDR, DAT, white);
	}
}

/**
 * @brief	显示圆形符号
 * @param	row，col，行列
 * @param	mode，1表示显示，0表示清除
*/
void OLED_ShowCircle(u8 row, u8 col, u8 mode) {
	u8 i;

	OLED_StartPoint(row, col);
	for (i = 0; i < 8; i++) {
		if (mode) WriteMem(OLED_ADDR, DAT, CIRCLE[i]);
		else WriteMem(OLED_ADDR, DAT, white);
	}
	OLED_StartPoint(row + 1, col);
	for (i = 0; i < 8; i++) {
		if (mode) WriteMem(OLED_ADDR, DAT, CIRCLE[i + 8]);
		else WriteMem(OLED_ADDR, DAT, white);
	}
}
