
/***************************************************************************************
****************************************************************************************
* FILE		: Exti.h
* Description	:  �жϴ���
*			  
* Copyright (c) 2018 by XXX. All Rights Reserved.
* 
* History:
* Version		Name       		Date			Description
   0.1		XXX	2018/09/21	Initial Version
   
****************************************************************************************
****************************************************************************************/



#ifndef	__EXTI_H
#define	__EXTI_H

#include	"config.h"

#define	EXT_INT0			0	//��ʼ�����ж�0
#define	EXT_INT1			1	//��ʼ�����ж�1
#define	EXT_INT2			2	//��ʼ�����ж�2
#define	EXT_INT3			3	//��ʼ�����ж�3
#define	EXT_INT4			4	//��ʼ�����ж�4

#define	EXT_MODE_RiseFall	0	//������/�½����ж�
#define	EXT_MODE_Fall		1	//�½����ж�

typedef struct
{
	u8	EXTI_Mode;			//�ж�ģʽ,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	u8	EXTI_Polity;		//���ȼ�����	PolityHigh,PolityLow
	u8	EXTI_Interrupt;		//�ж�����		ENABLE,DISABLE
} EXTI_InitTypeDef;


u8	Ext_Inilize(u8 EXT, EXTI_InitTypeDef *INTx);

#endif
