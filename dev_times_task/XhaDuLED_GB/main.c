
/*************	����˵��	**************

1.��Դָʾ��ÿ10������һ��
2.ɱ���ƹ�ÿ�� 1,4,7,10,13,16,19,22 ����ʱ��� ��������.����20����.
3.��������,ǿ�ƿ�����������,.����20����.

******************************************/


/*************	���س�������	**************/
#define MAIN_Fosc		12000000L
#define	g_light_on_time 15 ////Ĭ��Ϊ20����. ��λΪ����
typedef 	unsigned char	u8;
typedef 	unsigned int	u16;


/*************	���ر�������	**************/
u8 g_hour=8;  //ʱ
u8 g_minute=0;//��
u8 g_second=0;//��
u16 g_millisecond=0;//���� 
//u8  g_addNum = 50; //50 //��ʱ������

//u8 g_key_flag=0; //����״̬,(1:����  0:δ����)
u8 g_key_time=0; //������Ӧ�¼�ʱ���ʱ��.Ĭ��Ϊ20����. ��λΪ����

//u8 bIsOpen;
/*************	���غ�������	**************/

void delay_ms(unsigned char ms);
void printNowTime(void);
/*************  �ⲿ�����ͱ������� *****************/



/************************ IO������ ****************************/

sfr P3M1  = 0xB1;	//P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
sfr P3M0  = 0xB2;	//					=10--->pure input,	11--->open drain

sfr PCON = 0x87;
sfr IE    = 0xA8;
sbit EA   = IE^7;	//�ж������ܿ���λ
sbit ET0  = IE^1;	//��ʱ�ж�0�������λ
sfr TCON = 0x88;
sbit TR0  = TCON^4;
sfr TMOD  = 0x89;
sfr	AUXR = 0x8E;
sbit TF0  = TCON^5;
sfr TH0  = 0x8C;
sfr TL0  = 0x8A;
sfr INT_CLKO = 0x8F;

sbit ELVD = IE^6;                   //��ѹ����ж�ʹ��λ
#define LVDF    0x20                //PCON.5,��ѹ����жϱ�־λ


sfr P3    = 0xB0;
sbit ioWorkLED   = P3^3;  //����ָʾ��
sbit ioSwitchLED = P3^2;  //����ɱ����
sbit ioKEY       = P3^5;  //������ɱ����

//#include	"soft_uart.h"

/******************** ������**************************/
void main(void)
{

	/*
	p3_2 : ���ڹ���ָʾ��
	p3_3 : ���ڿ���led������ʾ
	*/
	// 0000 0000
	P3 = 0x00;
	
	PCON &= ~LVDF;                  //�ϵ����Ҫ��LVD�жϱ�־λ
    ELVD = 1;                       //ʹ��LVD�ж�

	//INT_CLKO |= 0x20;           //(EX3 = 1)ʹ��INT3�½����ж� ����
	
	//io ������	 0000|0000   0000|1100
	P3M1 =  0x00;
	P3M0 =  0x0c;

	// �����ƿ�ʼ
	//ioWorkLED = 1;

	// ���Կ���.
	ioSwitchLED = 0;
	ioKEY =1; //�����ر�

	//��ʱ������ 50ms һ�ζ�ʱ����ʱ������ʱ��cpu���뻽��ʱ��.
	//Timer_config();
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TMOD |= 0x01;		//���ö�ʱ��ģʽ
	TL0 = 0xB0;		//���ö�ʱ��ֵ
	TH0 = 0x3C;		//���ö�ʱ��ֵ
	ET0 = 1;
	//TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ

	EA=1;
	
	while (1)
	{

		//����ɨ�� 1����������ɱ��
		if(ioKEY == 0)                
		//����м����£��������������м����£���s4Ϊ0���� !key_flagΪ1��
	    {
	    	delay_ms(20);//��ʱ����
	        if(ioKEY == 0)                             //���ȷ���м�����
	        {                      
	            //�����¼�����
	            //g_key_flag = 1;
	            //�������Ƽ�ʱ
	            g_key_time=g_light_on_time;
				ioWorkLED = 1;
				//PrintString("\r\n �����Ѱ���.");
				continue;
	        }
		}

	    //�����ж�  2.�Զ����񴥷�ɱ��
		switch(g_hour) {  //��ˮʱ��㣬�����ʱ�����ɱ������. (һ��4�Σ�ÿ��20���ӣ�һ�칲1��Сʱ.)
			 case 7:
			 case 13:
			 case 19:
			 case 23:
			 {
			 	if(g_key_time==0){ //ֻ����g_key_time����Ǵ���״̬�£��Ž��д�������.
					g_key_time=g_light_on_time;
				}
			 	
			 	break;
			  }
			default:
				//bIsOpen = 0;
			 	break;
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
			delay_ms(900);
		}

		//��������. 
	}
}

/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt 1
{
	//PrintString("\r\n T:");
	g_millisecond = g_millisecond + 50;

	if( g_millisecond >= 1000) { //����һ��
		g_millisecond = 0;
		g_second++;
		//printNowTime();

		/////
		//ʵʱ�ж�
		if(g_key_time>0){
			ioSwitchLED = 1;
		}
		//////

		if(g_second >=60){

			//����һ����
			//ִ������
			///////////////////////////
			if(g_key_time<1){
				//��λ.
				//g_key_flag =0; //���ڿ��ƺڵ�
				ioSwitchLED = 0;
				g_key_time =0; //���ð������Ƽ�ʱʱ��
			}
			else{
				//g_key_flag =1; //���ڿ�������
				ioSwitchLED = 1;
				g_key_time--;
			}			
			///////////////////////////


			///ʱ���ۼ�
			g_second =0;
			g_minute ++;

			if(g_minute >=60){

				//����һСʱ
				g_minute =0;
				g_hour ++;

				if(g_hour ==24){

					//����24Сʱ
					g_hour =0;
			
				}
			
			}
		}
	}
}

///�͵�ѹ�жϺ���
//����͵�ѹ����	     LVD_VECTOR		6
void LVD_ISR() interrupt 6 using 1
{	
	//
	ioWorkLED = 1; //��������ʾ��ѹ����
	ioSwitchLED = 0; //Ϩ��

	PCON &= ~LVDF;                  //��PCON.5д0��LVD�ж�

	//PCON |= 0x02;               //�������ģʽ
}

void  delay_ms(unsigned char ms)
{
     unsigned int i;
	 do{
	      i = MAIN_Fosc / 13000;
		  while(--i)	;   //14T per loop
     }while(--ms);
}

 /*
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
	TxSend(g_second%100/10 + '0');
	TxSend(g_second%10+ '0');

///*
	TxSend(' ');
	TxSend('U');
	//TxSend(g_millisecond+ '0');
	TxSend(g_millisecond%1000/100 + '0');
	TxSend(g_millisecond%100/10 + '0');
	TxSend(g_millisecond%10+ '0');


//	PrintString("\r\n ");
//}
*/
