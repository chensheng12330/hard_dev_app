
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
//#include "reg51.h"
#include	"config.h"
#include	"timer.h"
#include	"GPIO.h"
#include	"soft_uart.h"
#include	"delay.h"

/*************	����˵��	**************

��װ[�Զ������]

1��һ����ʱ������Ϊ��7�㣬13�㣬19��
2��T��������ÿ��12���׼ʱ�䣬�������趨��ǰʱ��Ϊ12��.R���ڻָ���ɢ����Ϊ0��ÿ�λ�ƿ������.S �رյ���������.
3��L1��ʾ��ǰ״̬��������ʾ����Һ�岻��. 
L1��һ�Σ���ʾʱ���׼��ɡ�L1�����Σ���ʾ�����ָ���ɡ� L1�����Σ���ʾ�رն�ʱ

******************************************/

/*************	���س�������	**************/


/*************	���ر�������	**************/
u32 g_hour=0;  //ʱ
u32 g_minute=0;//��
u32 g_second=0;//��
u32 g_millisecond=0;//���� 
u8  g_addNum = 50; //50 //��ʱ������

u16 g_key_flag=0; //����״̬,(1:����  0:δ����)
u8  g_key_time=0; //������Ӧ�¼�ʱ���ʱ��.Ĭ��Ϊ20����. ��λΪ����
u8  g_light_on_time=1; //Ĭ��Ϊ20����. ��λΪ����

/*************	���غ�������	**************/

void mintueAction(void);

void secondAction(void);

void printNowTime(void);

void key_scan(void);

/*************  �ⲿ�����ͱ������� *****************/



/************************ IO������ ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//�ṹ����
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;	//ָ��Ҫ��ʼ����IO, 
	// GPIO_Pin_2 ~ GPIO_Pin_3, �����
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//��ʼ��


	GPIO_InitStructure.Pin  = GPIO_Pin_5;	//ָ��Ҫ��ʼ����IO,
	GPIO_InitStructure.Mode = GPIO_PullUp;				//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	
	
}

/************************ ��ʱ������ ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (50.0/1000.0 * MAIN_Fosc/12.0);		
	// //��ֵ,50����һ���ж�.
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2

/*
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ��ʱ��Դ, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 50*12*2);		//��ֵ,
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//��ʼ��Timer1	  Timer0,Timer1,Timer2
*/

//	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE. (ע��: Timer2�̶�Ϊ16λ�Զ���װ, �жϹ̶�Ϊ�����ȼ�)
//	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
//	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
//	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (50*12));		//��ֵ
//	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
//	Timer_Inilize(Timer2,&TIM_InitStructure);				//��ʼ��Timer2	  Timer0,Timer1,Timer2
}


sbit ioWorkLED   = P3^2;  //����ָʾ��
sbit ioSwitchLED = P3^3;  //����ɱ����
sbit ioKEY       = P3^5;  //������ɱ����

/******************** ������**************************/
void main(void)
{

	/*
	p3_2 : ���ڹ���ָʾ��
	p3_3 : ���ڿ���led������ʾ
	*/
	
	//io ������
	GPIO_config();

	// �����ƿ�ʼ
	ioWorkLED = 1;

	// ���Կ���.
	ioSwitchLED = 1;
	ioKEY =1; //�����ر�

	//��ʱ������ 50ms һ�ζ�ʱ����ʱ������ʱ��cpu���뻽��ʱ��.
	Timer_config();

	EA = 1;
		 
	PrintString("\r\n ��ʼ������... = ");

	while (1)
	{
		key_scan();
        /*
		PCON |= 0x01;           //��IDL(PCON.0)��1,MCU���������ģʽ
        _nop_();                //��ʱCPU��ʱ��,��ִ��ָ��
        _nop_();                //�ڲ��ж��źź��ⲿ��λ�źſ�����ֹ����ģʽ
        _nop_();
        _nop_();
        */

			/*
		ִ��������1

		1,4,7,10,13,16,19,22 ��ʼ����.
		*/

		if( g_hour ==0 ||
			g_hour ==4 ||
			g_hour ==7 ||
			g_hour ==10||
			g_hour ==13||
			g_hour ==16||
			g_hour ==19||
			g_hour ==22||
			g_key_flag==1
			){

			//��������20����.
			if(g_minute<g_light_on_time || g_key_flag==1) {

				ioSwitchLED = 1;
			}
			else {
				ioSwitchLED = 0;
			}
		}

		/*
		ִ��������1

		������,ÿ10����һ��
		*/
		if(g_second%10 == 0){
			ioWorkLED = 1;
			delay_ms(2000);
			ioWorkLED = 0;
		}
		else {
			//ioWorkLED = 0;
			delay_ms(900);
		}

		//��������.

		/*	
		   PrintString("\r\n ѭ����... ");

			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
		*/   
	}
}

/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	g_millisecond += g_addNum;

	if( g_millisecond>= 1000) { //����һ��
		g_millisecond = 0;
		secondAction();
		
		g_second++;

		if(g_second >=60){

			//����һ����
			mintueAction();
			g_second =0;
			g_minute ++;

			if(g_minute >=60){

				//����һСʱ
				g_minute =0;
				g_hour ++;

				if(g_hour >=24){

					//����24Сʱ
					g_hour =0;
			
				}
			
			}
		}
	}
}

void mintueAction(void) {
	//PrintString("\r\n mintueAction... ");

	if(g_key_time<1){
		//��λ.
		g_key_flag =0; //���ڿ�������
		g_key_time =0; //���ð������Ƽ�ʱʱ��
	}
	else{
		g_key_flag =1; //���ڿ�������
		g_key_time--;
	}
}

void secondAction(void) {
	//PrintString("\r\n secondAction... ");
	printNowTime();
}

// TxSend(j/1000 + '0');
void printNowTime(void) {

	PrintString("\r\n ����ʱ��:");

	TxSend('H');
	//TxSend(g_hour+ '0');
	TxSend(g_hour%100/10 + '0');
	TxSend(g_hour%10+ '0');
	

	TxSend(' ');
	TxSend('M');
	TxSend(g_minute%100/10 + '0');
	TxSend(g_minute%10+ '0');

	TxSend(' ');
	TxSend('S');
	//TxSend(g_second+ '0');
	TxSend(g_second%100/10 + '0');
	TxSend(g_second%10+ '0');

/*
	TxSend(' ');
	TxSend('U');
	//TxSend(g_millisecond+ '0');
	TxSend(g_millisecond%1000/100 + '0');
	TxSend(g_millisecond%100/10 + '0');
	TxSend(g_millisecond%10+ '0');
*/
	
	PrintString("\r\n ");
}

void key_scan(){

	if(ioKEY == 0)                
	//����м����£��������������м����£���s4Ϊ0���� !key_flagΪ1��
    {
    	delay_ms(10);//��ʱ����
        if(ioKEY == 0)                             //���ȷ���м�����
        {                      
            //�����¼�����
            g_key_flag = 1;
            //�������Ƽ�ʱ
            g_key_time=g_light_on_time;

			PrintString("\r\n �����Ѱ���.");
            return ;
        }
	}
	//g_key_flag = 0;
	return ;
}

