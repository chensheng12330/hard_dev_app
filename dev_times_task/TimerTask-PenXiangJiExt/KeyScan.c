
/***************************************************************************************
****************************************************************************************
* FILE		: KeyScan.c
* Description	: 
*			  
* Copyright (c) 2018 by ��ʤ. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		����			2018/09/15	Initial Version


   https://blog.csdn.net/ma57457/article/details/76363567
   https://blog.csdn.net/a123rou/article/details/71339646
   https://blog.csdn.net/lanmanck/article/details/8707957
****************************************************************************************
****************************************************************************************/



#ifndef _KEYSCAN_C_
#define _KEYSCAN_C_

#include "KeyScan.h"
/*
* description: ������������
* intput�������ṹ�壬��ֵ
* output����ֵ
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
			
			if( Key->CountDowm >= DEBOUNCE && Key->CountDowm < LONGPRESS)//�̰����ж�
			{
				keyret = KeyLast;
			}
			else if( Key->CountDowm == LONGPRESS )//�������ж�				
			{
				keyret = Key->Last;	
				keyret |= 0x0100;		
			}
			else if(KeyCKey->CountDowmountdowm > LONGPRESS+DEBOUNCE)//���������ж�
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
* description: ��ȡ�����ĺ���
* intput��none
* output����ֵ
* 
*/
uint8 GetKeyValue(void)
{
	uint8 ret=NULL;
	/*
	������ֵĴ�����Ҫ�Լ�ʵ�֣�������Դ�ͨ�ź�����iic��spi��ü�ֵ��Ҳ���Դ�gpio�˿ڻ�ü�ֵ
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
* description: ɸѡ�����ĺ���,ֻɸѡ��Ҫ�ļ�ֵ
* intput����ֵ
* output����ֵ
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
*               ���ڰ����ж��ӳ���ÿ10ms�ж�һ�� 
*               cnt_2ms��һ��ͬʱ��TH0��TL0����ֵ 
****************************************************/  

void timer2_int (void) interrupt TIMER2_VECTOR
{
	//key_scan(); 
}


#endif /*_KEYSCAN_C_*/

