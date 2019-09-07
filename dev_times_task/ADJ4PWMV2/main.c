
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

/*************	����˵��	**************

��������ʾʹ�ö�ʱ�������PWM��

��ʱ��0��16λ�Զ���װ���жϣ���T0CLKO�������PWM��

��������ʹ��STC15F/Lϵ��MCU�Ķ�ʱ��T0��ģ��PWM�����̡�

PWM��������������̡��������������װ��Ҫһ��ʱ�䣬����PWMռ�ձ���СΪ32T/���ڣ����Ϊ(����-32T)/����, TΪʱ�����ڡ�

PWMƵ��Ϊ���ڵĵ�������������Ϊ6000, ʹ��24MHZ����Ƶ����PWMƵ��Ϊ4000HZ��
							  183
******************************************/

/*************	���س�������	**************/

#define		PWM_DUTY		10		//����PWMռ�ձ�	
#define     PWM_CYCLE       100	//����PWM�����ڣ���ֵΪʱ��������������ʹ��24.576MHZ����Ƶ����PWMƵ��Ϊ6000HZ��

#define		PWM_HIGH_MIN	32				//����PWM�������Сռ�ձȡ��û������޸ġ�
#define		PWM_HIGH_MAX	(PWM_DUTY-PWM_HIGH_MIN)	//����PWM��������ռ�ձȡ��û������޸ġ�


/*************	���ر�������	**************/





PWMInfoDef  pwm1Info={0}; //1·pwm
PWMInfoDef  pwm2Info={0}; //2·pwm		   

//u16		PWM_high,PWM_low;	//�м�������û������޸ġ�
//u16		pwm;				//����PWM����ߵ�ƽ��ʱ��ı������û�����PWM�ı�����



/*************	���غ�������	**************/



/*************  �ⲿ�����ͱ������� *****************/



/************************ ��ʱ������ ****************************/
void	Timer_config(void)
{
	//time0
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - PWM_HIGH_MIN;	//��ֵ,
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2

	//time2
	Timer_Inilize(Timer2,&TIM_InitStructure);
	
}



/**************** ����PWM��װֵ���� ******************
//pwm = PWM_DUTY / 10;	//��PWMһ����ֵ������Ϊ10%ռ�ձ�
//LoadPWM(pwm);	
void LoadPWM(u16 i)
{
	u16	j;

	if(i > PWM_HIGH_MAX)		i = PWM_HIGH_MAX;	//���д��������ռ�ձ����ݣ���ǿ��Ϊ���ռ�ձȡ�
	if(i < PWM_HIGH_MIN)		i = PWM_HIGH_MIN;	//���д��С����Сռ�ձ����ݣ���ǿ��Ϊ��Сռ�ձȡ�
	j = 65536UL - PWM_DUTY + i;	//����PWM�͵�ƽʱ��
	i = 65536UL - i;			//����PWM�ߵ�ƽʱ��
	EA = 0;
	PWM_high = i;	//װ��PWM�ߵ�ƽʱ��
	PWM_low  = j;	//װ��PWM�͵�ƽʱ��
	EA = 1;
}
*/


void PWM_Load(PWMInfoDef *pwmDef)
{
	u16 tempPwm = pwmDef->pwm_cycle / pwmDef->pwm_duty;
	u16 lowTime,higTime;
	
	lowTime = 65536UL - pwmDef->pwm_cycle + tempPwm;	//����PWM�͵�ƽʱ��;
	higTime = 65536UL - tempPwm; //����PWM�ߵ�ƽʱ��

	//if(i > PWM_HIGH_MAX)		i = PWM_HIGH_MAX;	//���д��������ռ�ձ����ݣ���ǿ��Ϊ���ռ�ձȡ�
	//if(i < PWM_HIGH_MIN)		i = PWM_HIGH_MIN;	//���д��С����Сռ�ձ����ݣ���ǿ��Ϊ��Сռ�ձȡ�
	
	EA = 0;
	pwmDef->pwm_high= higTime;	//װ��PWM�ߵ�ƽʱ��
	pwmDef->pwm_low = lowTime;	//װ��PWM�͵�ƽʱ��
	EA = 1;
}


void PWM_config(void) {
	//pwm = PWM_DUTY / 10;	//��PWMһ����ֵ������Ϊ10%ռ�ձ�
	pwm1Info.pwm_duty = PWM_DUTY;
	pwm1Info.pwm_cycle= PWM_CYCLE;
	PWM_Load(&pwm1Info);
	
	pwm2Info.pwm_duty = PWM_DUTY+10;
	pwm2Info.pwm_cycle= PWM_CYCLE+5000;
	PWM_Load(&pwm2Info);
}

/******************** ������**************************/
void main(void)
{
	P_PWM1_P35 = 0;
	P_PWM2_p30 = 0;
	P3M1 &= ~(1 << 5);	//P3.5 ����Ϊ�������
	P3M0 |=  (1 << 5);

	P3M1 &= ~(1 << 0);	//P3.0 ����Ϊ�������
	P3M0 |=  (1 << 0);

	Timer_config();
	//EA = 1;
	P35 = 0;	//����STC15W408Sϵ�У�Ҫ����ͣ��������ó������OD����������
	P30 = 0;	  //P_PWM2_p30

	//pwm = PWM_DUTY / 10;	//��PWMһ����ֵ������Ϊ10%ռ�ձ�
	//LoadPWM(pwm);			//����PWM��װֵ

	PWM_config();
	


	
	while (1)
	{
		/*
		while(pwm < (PWM_HIGH_MAX-8))
		{
			pwm += 8;		//PWM�𽥼ӵ����
			LoadPWM(pwm);
			delay_ms(8);
		}
		while(pwm > (PWM_HIGH_MIN+8))
		{
			pwm -= 8;	//PWM�𽥼�����С
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



