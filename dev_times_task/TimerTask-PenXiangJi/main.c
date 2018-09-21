
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
#include	"Exti.h"


/*************	����˵��	**************

��װ[�Զ������]

1��һ����ʱ������Ϊ��7�㣬13�㣬19��
2��
T(io) ��������ÿ��12���׼ʱ�䣬�������趨��ǰʱ��Ϊ12��.
M ���������������������һ�Ρ�
S  �رյ���������.

3��L1(��io0)��ʾ��ǰ״̬��
10����һ�Σ���ʾ������ʱ�����С�
������ʾ��ز��㡣

L2(��io1)��һ�Σ���ʾʱ���׼��ɡ�
L2(��)�����Σ���ʾ�����ָ���ɡ� 
L2(��)�����Σ���ʾ�رն�ʱ


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


sbit ioInKeyForTime = P3^2; //ʱ���׼��������
sbit ioInKeyForRun  = P3^3;//�����Ƿ��������� 
sbit ioInKeyForMoto =P3^5;//����������������� 

sbit ioOutForL1Green= P3^0;  //�̵ƣ���������ָʾ��
sbit ioOutForL2Red  = P3^1;  //��ƣ�������Ӧָʾ��
sbit ioOutForMoto   = P3^4;  //���������IO��.

u8 ioSwitchLED=0;
u8 ioWorkLED=0;
u8 ioKEY=0;

typedef struct
{
	u8	sKeyForTime;  //ʱ���״ֵ̬.  0: δ����  //1:�Ѱ���
	u8	sKeyForRun;	  //�����״ֵ̬.  0: δ����  //1:�Ѱ���
       u8   sKeyForMoto;  //�������״ֵ̬.  0: δ����  //1:�Ѱ���
} KeyStateDef; //��ǰ���򰴼���״̬,Ĭ��Ϊ0 

//ȫ�ֶ���
KeyStateDef  g_allKeyState={0,0,0};



/*************	���غ�������	**************/

void mintueAction(void);

void secondAction(void);

void printNowTime(void);

void key_scan(void);

/*************  �ⲿ�����ͱ������� *****************/
void	EXTI_config(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//�ṹ����

    //��ʼ��INT0
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_RiseFall;	//�ж�ģʽ,  EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityHigh;			//�ж����ȼ�,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//�ж�����,     ENABLE��DISABLE
	Ext_Inilize(EXT_INT0,&EXTI_InitStructure);	
    
    //��ʼ��INT1
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//�ж�ģʽ,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//�ж����ȼ�,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//�ж�����,     ENABLE��DISABLE
	Ext_Inilize(EXT_INT1,&EXTI_InitStructure);				//	EXT_INT0,EXT_INT1,EXT_INT2,EXT_INT3,EXT_INT4

    //��ʼ��INT3
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//�ж�ģʽ,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//�ж����ȼ�,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//�ж�����,     ENABLE��DISABLE
	Ext_Inilize(EXT_INT3,&EXTI_InitStructure);
}



/************************ IO������ ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//�ṹ����
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_5;	//ָ��Ҫ��ʼ����IO, 
	// GPIO_Pin_2 ~ GPIO_Pin_3, �����
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//��ʼ��


	GPIO_InitStructure.Pin   = GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 ;	//ָ��Ҫ��ʼ����IO,
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

}


/******************** ������**************************/
void main(void)
{
	
	//io ������
	GPIO_config();

    //ȫ�ֱ�����ʹ��
	// �����ƿ�ʼ
      ioOutForL1Green=ioInKeyForMoto=ioInKeyForRun=ioInKeyForTime=1; //key������

      //���io���ȹر�
      ioOutForL2Red =ioOutForMoto = 0;

        //g_allKeyState = {0,0,0};


        //��������p32 p33���ⲿ�ж�,�½����ж�.
        EXTI_config();
    
	//��ʱ������ 50ms һ�ζ�ʱ����ʱ������ʱ��cpu���뻽��ʱ��.
	Timer_config();

	EA = 1;
		 
	//PrintString("\r\n ��ʼ������... = ");

	while (1)
	{
		//key_scan();

			/*
		ִ��������1

		1,4,7,10,13,16,19,22 ��ʼ����.
		*/

		if(   g_hour ==0 ||
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

	//PrintString("\r\n ����ʱ��:");

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



/********************* INT0�жϺ��� *************************/
void INT0_int (void) interrupt INT0_VECTOR		//���ж�ʱ�Ѿ������־
{
	//WakeUpSource = 1;	//���INT0����
	//EX0 = 0;			//INT0 Disable
	IE0  = 0;			//���ж�0��־λ
    PrintString("\r\n �ⲿ�ж�0.");
}

/********************* INT1�жϺ��� *************************/
void INT1_int (void) interrupt INT1_VECTOR		//���ж�ʱ�Ѿ������־
{
	//�����ж�ʱ�����԰�int1�жϹرա�
	EX1 = 0;	
	//tfWakeUpSource = 2;	//���INT1����
	//EX1 = 0;			//INT1 Disable
	IE1  = 0;			//���ж�1��־λ

	//�������ж��¼��󣬿��԰� int1�жϿ�������ֹ��ν��롣

    PrintString("\r\n �ⲿ�ж�1.��ʱ1��");
    delay_ms(2000);
	EX1 = 1;
}

/********************* INT3�жϺ��� ************************/
void INT3_int  (void) interrupt INT3_VECTOR
{
    PrintString("\r\n �ⲿ�ж�3.��ʱ1��");

/*
    INT_CLKO |=  (1 << 5);  //�����ж�  
    INT_CLKO &= ~(1 << 5);  //��ֹ�ж�
*/
	//EX3 = 0;	
    //IE1  = 0;	
}


