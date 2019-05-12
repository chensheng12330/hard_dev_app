#ifndef		__PWM_H
#define		__PWM_H

#include	"config.h"
typedef struct
{
	u8  pwm_high;		//PWM高电平时间
	u8	pwm_low;		//PWM低电平时间
	u8  pwm_duty;  		//占空比 0-100
	u16 pwm_cycle;		//定义PWM的周期，数值为时钟周期数,单位HZ
} PWMInfoDef;

#endif