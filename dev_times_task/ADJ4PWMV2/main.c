
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

#include	"config.h"
#include	"timer.h"
#include	"delay.h"
#include	"soft_uart.h"
#include 	"pwm.h"
//#include	<STDIO.H>

/*************	功能说明	**************

本程序演示使用定时器做软件PWM。

定时器0做16位自动重装，中断，从T0CLKO高速输出PWM。

本例程是使用STC15F/L系列MCU的定时器T0做模拟PWM的例程。

PWM可以是任意的量程。但是由于软件重装需要一点时间，所以PWM占空比最小为32T/周期，最大为(周期-32T)/周期, T为时钟周期。

PWM频率为周期的倒数。假如周期为6000, 使用24MHZ的主频，则PWM频率为4000HZ。
							  183
******************************************/

/*************	本地常量声明	**************/

#define		PWM_DUTY		10		//定义PWM占空比	
#define     PWM_CYCLE       100	//定义PWM的周期，数值为时钟周期数，假如使用24.576MHZ的主频，则PWM频率为6000HZ。

#define		PWM_HIGH_MIN	32				//限制PWM输出的最小占空比。用户请勿修改。
#define		PWM_HIGH_MAX	(PWM_DUTY-PWM_HIGH_MIN)	//限制PWM输出的最大占空比。用户请勿修改。


/*************	本地变量声明	**************/





PWMInfoDef  pwm1Info={0}; //1路pwm
PWMInfoDef  pwm2Info={0}; //2路pwm		   

//u16		PWM_high,PWM_low;	//中间变量，用户请勿修改。
//u16		pwm;				//定义PWM输出高电平的时间的变量。用户操作PWM的变量。



/*************	本地函数声明	**************/



/*************  外部函数和变量声明 *****************/



/************************ 定时器配置 ****************************/
void	Timer_config(void)
{
	//time0
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - PWM_HIGH_MIN;	//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2

	//time2
	Timer_Inilize(Timer2,&TIM_InitStructure);
	
}



/**************** 计算PWM重装值函数 ******************
//pwm = PWM_DUTY / 10;	//给PWM一个初值，这里为10%占空比
//LoadPWM(pwm);	
void LoadPWM(u16 i)
{
	u16	j;

	if(i > PWM_HIGH_MAX)		i = PWM_HIGH_MAX;	//如果写入大于最大占空比数据，则强制为最大占空比。
	if(i < PWM_HIGH_MIN)		i = PWM_HIGH_MIN;	//如果写入小于最小占空比数据，则强制为最小占空比。
	j = 65536UL - PWM_DUTY + i;	//计算PWM低电平时间
	i = 65536UL - i;			//计算PWM高电平时间
	EA = 0;
	PWM_high = i;	//装载PWM高电平时间
	PWM_low  = j;	//装载PWM低电平时间
	EA = 1;
}
*/


void PWM_Load(PWMInfoDef *pwmDef)
{
	u16 tempPwm = pwmDef->pwm_cycle / pwmDef->pwm_duty;
	u16 lowTime,higTime;
	
	lowTime = 65536UL - pwmDef->pwm_cycle + tempPwm;	//计算PWM低电平时间;
	higTime = 65536UL - tempPwm; //计算PWM高电平时间

	//if(i > PWM_HIGH_MAX)		i = PWM_HIGH_MAX;	//如果写入大于最大占空比数据，则强制为最大占空比。
	//if(i < PWM_HIGH_MIN)		i = PWM_HIGH_MIN;	//如果写入小于最小占空比数据，则强制为最小占空比。
	
	EA = 0;
	pwmDef->pwm_high= higTime;	//装载PWM高电平时间
	pwmDef->pwm_low = lowTime;	//装载PWM低电平时间
	EA = 1;
}


void PWM_config(void) {
	//pwm = PWM_DUTY / 10;	//给PWM一个初值，这里为10%占空比
	pwm1Info.pwm_duty = PWM_DUTY;
	pwm1Info.pwm_cycle= PWM_CYCLE;
	PWM_Load(&pwm1Info);
	
	pwm2Info.pwm_duty = PWM_DUTY+10;
	pwm2Info.pwm_cycle= PWM_CYCLE+5000;
	PWM_Load(&pwm2Info);
}

/******************** 主函数**************************/
void main(void)
{
	P_PWM1_P35 = 0;
	P_PWM2_p30 = 0;
	P3M1 &= ~(1 << 5);	//P3.5 设置为推挽输出
	P3M0 |=  (1 << 5);

	P3M1 &= ~(1 << 0);	//P3.0 设置为推挽输出
	P3M0 |=  (1 << 0);

	Timer_config();
	//EA = 1;
	P35 = 0;	//对于STC15W408S系列，要输出低，或者设置成推挽或OD，否则不正常
	P30 = 0;	  //P_PWM2_p30

	//pwm = PWM_DUTY / 10;	//给PWM一个初值，这里为10%占空比
	//LoadPWM(pwm);			//计算PWM重装值

	PWM_config();
	


	
	while (1)
	{
		/*
		while(pwm < (PWM_HIGH_MAX-8))
		{
			pwm += 8;		//PWM逐渐加到最大
			LoadPWM(pwm);
			delay_ms(8);
		}
		while(pwm > (PWM_HIGH_MIN+8))
		{
			pwm -= 8;	//PWM逐渐减到最小
			LoadPWM(pwm);
			delay_ms(8);
		}
		*/
		//const char *fm = 	 "%d";
		//sprintf();

		//(pw1,fm,112);
		
	//sprintf(pw1,"p1 H:%d L:%d",pwm1Info.pwm_high,pwm1Info.pwm_low);
	//sprintf(pw2,"p2 H:%d L:%d",pwm2Info.pwm_high,pwm2Info.pwm_low);
	//PrintString(pw1);
	//PrintString(pw2);
	//PrintString("12345");
	//TxSend('8');

	}
}



