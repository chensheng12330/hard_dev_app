#ifndef		__PWM_H
#define		__PWM_H

#include	"config.h"
typedef struct
{
	u8  pwm_high;		//PWM�ߵ�ƽʱ��
	u8	pwm_low;		//PWM�͵�ƽʱ��
	u8  pwm_duty;  		//ռ�ձ� 0-100
	u16 pwm_cycle;		//����PWM�����ڣ���ֵΪʱ��������,��λHZ
} PWMInfoDef;

#endif