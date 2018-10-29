
/***************************************************************************************
****************************************************************************************
* FILE		: KeyScan.c
* Description	: 
*			  
* Copyright (c) 2018 by 陈胜. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		按键			2018/09/15	Initial Version


   https://blog.csdn.net/ma57457/article/details/76363567
   https://blog.csdn.net/a123rou/article/details/71339646
   https://blog.csdn.net/lanmanck/article/details/8707957
****************************************************************************************
****************************************************************************************/



#ifndef _KEYSCAN_C_
#define _KEYSCAN_C_

#include "KeyScan.h"
/*
* description: 按键消抖函数
* intput：按键结构体，键值
* output：键值
* 
*/
uint16 key_scan( KEY_TYPE *Key ,uint8 keycur)
{
	unsigned uint16 keyret = 0;   	  
 
	keyret = KEY_NULL;
	if(  keycur != KEY_NULL )				
	{
		if( Key->Last == keycur || KEY_NULL == Key->Last)			
		{
			Key->CountDowm++;			
			Key->CountUp = 0;
			
			if( Key->CountDowm >= DEBOUNCE && Key->CountDowm < LONGPRESS)//短按键判断
			{
				keyret = KeyLast;
			}
			else if( Key->CountDowm == LONGPRESS )//长按键判断				
			{
				keyret = Key->Last;	
				keyret |= 0x0100;		
			}
			else if(KeyCKey->CountDowmountdowm > LONGPRESS+DEBOUNCE)//连续按键判断
			{
				Key->CountDowm -= DEBOUNCE;
				keyret = Key->Last;	
				keyret |= 0x0200;	
			}
		}
	}
	else
	{
		Key->CountUp++;	
 
		if( Key->CountUp > 1 ) 
		{
			Key->CountUp = 0;	
			Key->CountDowm = 0;
		}
	}
	
	Key->Last = keycur;	
	return keyret;
}
 
/*
* description: 获取按键的函数
* intput：none
* output：键值
* 
*/
uint8 GetKeyValue(void)
{
	uint8 ret=NULL;
	/*
	这个部分的代码需要自己实现，这里可以从通信函数如iic，spi获得键值，也可以从gpio端口获得键值
	*/
	return ret;
}
 
 
#define KEY_NULL 0
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_ON 3
#define KEY_UP_L 4
#define KEY_DOEN_L 5
#define KEY_ON_L 6
/*
* description: 筛选按键的函数,只筛选需要的键值
* intput：键值
* output：键值
* 
*/
uint8 GetKeyValue(uint8 key)
{
	uint8 ret=KEYNULL;
	switch(key)
	{
		0x01:
		{
			ret = KEY_UP;
		}
			break;
		0x0101:
		{
			ret = KEY_UP_L;
		}
			break;
		default:
			break;
	}
	
	return ret;
}


/*************************************************** 
*               用于按键中断子程序，每10ms中断一次 
*               cnt_2ms加一，同时给TH0和TL0赋初值 
****************************************************/  

void timer2_int (void) interrupt TIMER2_VECTOR
{
	//key_scan(); 
}


#endif /*_KEYSCAN_C_*/

