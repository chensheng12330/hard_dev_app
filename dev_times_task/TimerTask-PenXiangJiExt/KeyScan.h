
/***************************************************************************************
****************************************************************************************
* FILE		: KeyScan.h
* Description	: 按键扫描程序
*			  
* Copyright (c) 2018 by 陈胜. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		XXX				2018/09/15	Initial Version

*Info   
按键检测分为三个部分，一个是按键获取函数，一个是消抖，一个是按键筛选函数

1. IO引脚位检测扫描.
2. 消抖处理
3. 确认事件，设置相应的按键 标志位，进入当前按键的事件处理程序
4. 事件进行相应的处理，并将当前的按键标志位恢复。

****************************************************************************************
****************************************************************************************/

#ifndef _KEYSCAN_H_
#define _KEYSCAN_H_


typedef struct 
{
	uint8 Last;//最后记录状态
	uint8 CountDowm; //按下的次数
	uint8 CountUp;     //弹起的次数
} KEY_TYPE;
 
#define DEBOUNCE 10//延时消抖时间
#define LONGPRESS 100//长按键判断函数

/*
* description: 按键消抖函数
* intput：按键结构体，键值
* output：键值
* 
*/
uint16 key_scan( KEY_TYPE *Key ,uint8 keycur);


#endif /*_KEYSCAN_H_*/



