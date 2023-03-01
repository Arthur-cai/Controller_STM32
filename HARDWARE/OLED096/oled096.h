#ifndef __OLED096_H
#define __OLED096_H
/* 128*64像素数，显示8*16字符只能4行*16列 */
#include "../../BASIC/sys/sys.h"
#include "../../BASIC/i2c/i2c.h"

#define OLED_ADDR	0x78    //OLED在IIC上的地址
#define COM		    0x00    //OLED命令区
#define DAT 	    0x40    //OLED数据区

#define END_COL     128     //尾列
#define END_ROW     6       //末行
#define OFFSET      0	    //根据不同驱动芯片有不同的列起始偏移量，132列的芯片偏移量为2
#define white       0x00

/**
 * @brief OLED初始化，包含了OLED_Display_On(),OLED_Display_Off(),OLED_Clear()
*/
void OLED_Init(void);

/**
 * @brief 通过发送指令点亮oled
*/
void OLED_Display_On(void);

/**
 * @brief 通过发送指令熄灭oled
*/
void OLED_Display_Off(void);

/**
 * @brief 改变OLED对比度，可以看作亮度
 * @param command 要发送的指令，对比度范围00~FF
*/
void OLED_Brightness(u8 command);

/**
 * @brief 填充全部区域
 * @param color 颜色
*/
void OLED_FillAll(u8 color);

/**
 * @brief 调用OLED_FillAll(WHITE)实现清屏
*/
void OLED_Clear(void);

/**
 * @brief 清除指定区域，WHITE填充
 * @param row 行数(0，2，4，6)
 * @param col 列数, 根据前面已有字符数*8 往后移，注意是否定义了列数偏移2列
*/
void OLED_DeleteArea(u8 row, u8 start, u8 end);

/**
 * @brief 清除指定的行列之后的所有内容，以white填充
 * @param row 行数(0，2，4，6)
 * @param col 列数, 根据前面已有字符数*8 往后移，注意是否定义了列数偏移2列
*/
void OLED_DeleteFrom(u8 row, u8 col);

/**
 * @brief 显示字符
 * @param row 行数(0，2，4，6)
 * @param col 列数, 根据前面已有字符数*8 往后移，注意是否定义了列数偏移2列
 * @param ch 字符
*/
void OLED_ShowChar(u8 row, u8 col, u8 ch);

/**
 * @brief 显示字符串
 * @param row 行数(0，2，4，6)
 * @param col 列数, 根据前面已有字符数*8 往后移，注意是否定义了列数偏移2列
 * @param str 字符串
*/
void OLED_ShowStr(u8 row, u8 col, u8* str);

/**
 * @brief 显示数字
 * @param row 行数(0，2，4，6)
 * @param col 列数, 根据前面已有字符数*8 往后移，注意是否定义了列数偏移2列
 * @param num 数字
 * @param size 数字的位数
*/
void OLED_ShowNum(u8 row, u8 col, u32 num, u8 size);

/**
 * @brief	显示箭头符号
 * @param	row，col，行列
 * @param	mode，1表示显示，0表示清除
*/
void OLED_ShowArrow(u8 row, u8 col, u8 mode);

/**
 * @brief	显示菱形符号
 * @param	row，col，行列
 * @param	mode，1表示显示，0表示清除
*/
void OLED_ShowCircle(u8 row, u8 col, u8 mode);

#endif
