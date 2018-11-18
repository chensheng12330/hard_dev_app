
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
//#include	"soft_uart.h"
#include	"delay.h"
#include	"Exti.h"


/*************	����˵��	**************

��װ[�Զ������]

1��һ����ʱ������Ϊ��7�㣬13�㣬19��
2��
T(io) ��������ÿ��12���׼ʱ�䣬�������趨��ǰ�	���Ϊ12��.
M ���������������������һ�Ρ�
S  �رյ���������.

3��L1(��io0)��ʾ��ǰ״̬��
10����һ�Σ���ʾ������ʱ�����С�
������ʾ��ز��㡣

L2(��io1)��һ�Σ���ʾʱ���׼��ɡ�
L2(��)�����Σ���ʾ�����ָ���ɡ� 
L2(��)�����Σ���ʾ�رն�ʱ

/*stc15f104w ���Ű���
*********************************************************
�Զ�ģʽ  p3.4  -- ||--p3.3  �����������������
����Դ       vcc    -- ||--p3.2  ʱ�����ð�����������
�������  p3.5  -- ||--p3.1  ���
����Դ        gnd  -- ||--p3.0  �̵�
*********************************************************

*/

/******************************************/

/*************	���س�������	**************/
#define l2Num_TimeRest_1 1
#define l2Num_MotoRun_2  2
#define k_addNum 50 //50 //��ʱ������
#define k_moto_run_time (500) //500ms //�����ʱ��ֵ��

/*************	���ر�������	**************/
u8 g_hour=6;  //ʱ
u8  g_minute=59;//��
u8  g_second=50;//��
u16 g_millisecond=0;//���� 



sbit ioOutForL1Green= P3^0;  //�̵ƣ���������ָʾ��
sbit ioOutForL2Red  = P3^1;  //��ƣ�������Ӧָʾ��

sbit ioInKeyForMoto = P3^2;//�����������������  int0
sbit ioInKeyForTime = P3^3; //ʱ���׼��������  int1


sbit ioOutForMotoPower=P3^5;  //����������������IO��.
//sbit ioOutForMotoKey= P3^4;  //��ʱ������������IO��



int g_moto_run_time=0; //����������к��ʱ��ֵ
 
typedef struct
{
	u8	sKeyForTime;  //����ʱ���״ֵ̬.  0: δ����  //1:�Ѱ���
	//u8	sKeyForRun;	  //�����״ֵ̬.  0: δ����  //1:�Ѱ���
	u8     sKeyForMoto;  //�������״ֵ̬.  0: δ����  //1:�Ѱ���
} KeyStateDef; //��ǰ���򰴼���״̬,Ĭ��Ϊ0 

//ȫ�ֶ���
KeyStateDef  g_allKeyState={0,0};


/*************	���غ�������	**************/

//void mintueAction(void);

void secondAction(void);

//void printNowTime(void);

void key_scan(void);

void l2ShowWithNum(u8 num);

void motoStart(void);
/*************  �ⲿ�����ͱ������� *****************/
void	EXTI_config(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//�ṹ����

    //��ʼ��INT0
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//�ж�ģʽ,  EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityHigh;			//�ж����ȼ�,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//�ж�����,     ENABLE��DISABLE
	Ext_Inilize(EXT_INT0,&EXTI_InitStructure);	
    
    //��ʼ��INT1
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//�ж�ģʽ,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//�ж����ȼ�,   PolityLow,PolityHigh
	//EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//�ж�����,     ENABLE��DISABLE
	Ext_Inilize(EXT_INT1,&EXTI_InitStructure);				//	EXT_INT0,EXT_INT1,EXT_INT2,EXT_INT3,EXT_INT4

/*
    //��ʼ��INT3
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//�ж�ģʽ,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//�ж����ȼ�,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//�ж�����,     ENABLE��DISABLE
	Ext_Inilize(EXT_INT3,&EXTI_InitStructure);
	*/
}



/************************ IO������ ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//�ṹ����
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4|GPIO_Pin_5;	//ָ��Ҫ��ʼ����IO, 
	// GPIO_Pin_2 ~ GPIO_Pin_3, �����
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//ָ��IO������������ʽ,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//��ʼ��


	GPIO_InitStructure.Pin   = GPIO_Pin_2 |GPIO_Pin_3  ;	//ָ��Ҫ��ʼ����IO,
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
	TIM_InitStructure.TIM_Value     =  0x3cb0;//65536UL - (50.0/1000.0 * MAIN_Fosc/12.0);		
	// //��ֵ,50����һ���ж�.
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2

}


/******************** ������**************************/
void main(void)
{
	//�͵�ѹ��Ᵽ��
	PCON &= ~LVDF;                  //�ϵ����Ҫ��LVD�жϱ�־λ
    ELVD = 1;                       //ʹ��LVD�ж�
    
	//io ������
	GPIO_config();

    //ȫ�ֱ�����ʹ��
	// �����ƿ�ʼ
    ioOutForL1Green=ioInKeyForMoto=ioInKeyForTime=1; //key������

    //���io���ȹر�
    ioOutForL2Red =ioOutForMotoPower = 0;

    //ioOutForMotoKey=1; //����������İ�������. ��λ������λ��.
 	
           
    //g_allKeyState = {0,0,0};


    //��������p32 p33���ⲿ�ж�,�½����ж�.
    //EXTI_config();
    
	//��ʱ������ 50ms һ�ζ�ʱ����ʱ������ʱ��cpu���뻽��ʱ��.
	Timer_config();

	EA = 1;
		 
	//PrintString("\r\n ��ʼ������... = ");

	while (1)
	{
		key_scan();

			/*
		ִ��������1

		һ����ʱ������Ϊ��7�㣬13�㣬19��(��)
		*/
		switch(g_hour){
                        case 7:
                        case 13:
                        case 19:
                            {
                                    if( g_minute==0 && g_second ==0 && 
                                    g_millisecond<51 )
                                     {
                                              motoStart();
                                        }
                            }
                            break;
                            
                         default:
                            break;
		    }

	}
}

/***�ڲ����ܺ���***/

// ����������
void key_scan(void) {

	//moto	 
	
	 if(ioInKeyForMoto==0){
		
	   while(ioInKeyForMoto==0);
	   //
	   //�������������
		motoStart();
		l2ShowWithNum(l2Num_MotoRun_2);
	}

	//time		 ioInKeyForTime
	if(ioInKeyForTime==0){
		
	   while(ioInKeyForTime==0);
	   //
	   //����ʱ��Ϊ12��.
		g_hour = 12;
		g_minute=g_second=g_millisecond =0;

        l2ShowWithNum(l2Num_TimeRest_1);
	}


 /*
	if(g_allKeyState.sKeyForMoto == 1) {
		//���ԭ״̬
		g_allKeyState.sKeyForMoto = 0;

		
		
	}

	if(g_allKeyState.sKeyForRun== 1) {

		//ֹͣAPP����
		while(g_allKeyState.sKeyForRun){;};
		
		//���ԭ״̬
		//g_allKeyState.sKeyForRun = 0;
	}

	if(g_allKeyState.sKeyForTime== 1) {
		//���ԭ״̬
		g_allKeyState.sKeyForTime = 0;

		
		
	}	

	*/
}

//������Ӧ
/*
3��L1(��io0)��ʾ��ǰ״̬��
10����һ�Σ���ʾ������ʱ�����С�
������ʾ��ز��㡣

L2(��io1)��һ�Σ���ʾʱ���׼��ɡ�
L2(��)�����Σ���ʾ��������������� 
L2(��)�����Σ���ʾ�ر�APP����
*/ 

void l2ShowWithNum(u8 num)
{
	int i;
    //PrintString("\r\n l2ShowWithNum.");
		
	for(i=0; i<num; i++)
	{
		delay_ms(100);
		ioOutForL2Red = 1;
		delay_ms(100);
		ioOutForL2Red = 0;

	}	/**/
}	

/*�������

1.��p34,      (10��) ��ر�(�൱�ڿ��ص�Դ)
2.�ȴ����������,Լ(1��)��,��8(k_moto_run_time-2)��ʱ��������.
3.��p35, 20us ��ر�(ģ�ư���)
*/
void motoStart(void) {

			//�����������
			ioOutForL2Red = 1;
			ioOutForMotoPower = 1; //������Դ
	
	        delay_ms(400);
	
	        ioOutForL2Red = 0; //�ص������ָʾ��
	         //�رյ����Դ
	         ioOutForMotoPower= 0;
	         g_moto_run_time=0;
             
    //PrintString("\r\n motoStart. ");
}

//void milliAction(void){
//	 
//        if(g_moto_run_time==0){
//				  //ioOutForMotoPower= 0;
//        }
//	else {
//
//                        
//			 
//             }
//
//        
//
//        return ;   
//}

void secondAction(void) {
	//PrintString("\r\n secondAction... ");
	//printNowTime();
    
	if(g_second%10 == 0){   //����ָʾ��(��)
		ioOutForL1Green = 1;
		//delay_ms(200);
		
	}
    else {
            ioOutForL1Green = 0;
        }
	
}


/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	g_millisecond += k_addNum;

   //50mһ��.

   if(g_millisecond>400){
		 
   }

	if( g_millisecond>= 1000) { //����һ��
		g_millisecond = 0;
        
		secondAction();
		//milliAction();
		g_second++;

		if(g_second >=60){

			//����һ����
			//mintueAction();

			
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

/*
sbit ioInKeyForTime = P3^2; //ʱ���׼��������  int0
sbit ioInKeyForRun  = P3^3;//�����Ƿ���������   int1
sbit ioInKeyForMoto =P3^5;//�����������������  int3
*/

/********************* INT0�жϺ��� *************************/
void INT0_int (void) interrupt INT0_VECTOR		//���ж�ʱ�Ѿ������־
{
	//WakeUpSource = 1;	//���INT0����
	//EX0 = 0;			//INT0 Disable

	IE0  = 0;			//���ж�0��־λ
	while(ioInKeyForMoto==0);
	//g_allKeyState.sKeyForMoto= 1;
        //PrintString("\r\n �ⲿ�ж�0.");
        
        EX0 = 1;

	
}

/********************* INT1�жϺ��� *************************/
void INT1_int (void) interrupt INT1_VECTOR		//���ж�ʱ�Ѿ������־
{
	//�����ж�ʱ�����԰�int1�жϹرա�
	EX1 = 0;	
	//tfWakeUpSource = 2;	//���INT1����
	//EX1 = 0;			//INT1 Disable
	IE1  = 0;			//���ж�1��־λ

	if(ioInKeyForTime!=0) 
	{	 
		EX1 = 1;
		return;
	}

	g_allKeyState.sKeyForTime= 1;
	g_allKeyState.sKeyForMoto= 1;
	//�������ж��¼��󣬿��԰� int1�жϿ�������ֹ��ν��롣

   // PrintString("\r\n �ⲿ�ж�1.��ʱ1��");
   // delay_ms(2000);
    while(ioInKeyForTime==0);
	EX1 = 1;	  //�����״ֵ̬.  0: δ����  //1:�Ѱ���
  
}

/********************* INT3�жϺ��� ***********************
void INT3_int  (void) interrupt INT3_VECTOR
{
    PrintString("\r\n �ⲿ�ж�3.��ʱ1��");

//
    INT_CLKO |=  (1 << 5);  //�����ж�  
    INT_CLKO &= ~(1 << 5);  //��ֹ�ж�
///
	//EX3 = 0;	
    //IE1  = 0;	

	g_allKeyState.sKeyForMoto= 1;
}
*/

///�͵�ѹ�жϺ���
//����͵�ѹ����	     LVD_VECTOR		6
void LVD_ISR() interrupt 6 using 1
{	
	//
	ioOutForL2Red   = 1; //��������ʾ��ѹ����
	ioOutForL1Green= 0; //Ϩ��
	ioOutForMotoPower =0; //ǿ��ֹͣ��ת.

	PCON &= ~LVDF;                  //��PCON.5д0��LVD�ж�

	//PCON |= 0x02;               //�������ģʽ
}

/*
void printNowTime(void) {

	PrintString("\r\n ....");

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

//
	TxSend(' ');
	TxSend('U');
	//TxSend(g_millisecond+ '0');
	TxSend(g_millisecond%1000/100 + '0');
	TxSend(g_millisecond%100/10 + '0');
	TxSend(g_millisecond%10+ '0');
//
	
	PrintString("\r\n ");
}
*/

