
/*************	功能说明	**************

1.电源指示灯每10秒亮起一次
2.杀菌灯光每天 1,4,7,10,13,16,19,22 整点时间段 启动照明.持续20分钟.
3.按键按下,强制开启启动照明,.持续20分钟.

******************************************/


/*************	本地常量声明	**************/
#define MAIN_Fosc		12000000L
#define	g_light_on_time 15 ////默认为20分钟. 单位为分钟
typedef 	unsigned char	u8;
typedef 	unsigned int	u16;


/*************	本地变量声明	**************/
u8 g_hour=8;  //时
u8 g_minute=0;//分
u8 g_second=0;//秒
u16 g_millisecond=0;//毫秒 
//u8  g_addNum = 50; //50 //定时叠加数

//u8 g_key_flag=0; //按键状态,(1:按下  0:未按下)
u8 g_key_time=0; //按键响应事件时间计时数.默认为20分钟. 单位为分钟

//u8 bIsOpen;
/*************	本地函数声明	**************/

void delay_ms(unsigned char ms);
void printNowTime(void);
/*************  外部函数和变量声明 *****************/



/************************ IO口配置 ****************************/

sfr P3M1  = 0xB1;	//P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
sfr P3M0  = 0xB2;	//					=10--->pure input,	11--->open drain

sfr PCON = 0x87;
sfr IE    = 0xA8;
sbit EA   = IE^7;	//中断允许总控制位
sbit ET0  = IE^1;	//定时中断0允许控制位
sfr TCON = 0x88;
sbit TR0  = TCON^4;
sfr TMOD  = 0x89;
sfr	AUXR = 0x8E;
sbit TF0  = TCON^5;
sfr TH0  = 0x8C;
sfr TL0  = 0x8A;
sfr INT_CLKO = 0x8F;

sbit ELVD = IE^6;                   //低压检测中断使能位
#define LVDF    0x20                //PCON.5,低压检测中断标志位


sfr P3    = 0xB0;
sbit ioWorkLED   = P3^3;  //工作指示灯
sbit ioSwitchLED = P3^2;  //控制杀菌灯
sbit ioKEY       = P3^5;  //按键控杀菌灯

//#include	"soft_uart.h"

/******************** 主函数**************************/
void main(void)
{

	/*
	p3_2 : 用于工作指示灯
	p3_3 : 用于控制led照明显示
	*/
	// 0000 0000
	P3 = 0x00;
	
	PCON &= ~LVDF;                  //上电后需要清LVD中断标志位
    ELVD = 1;                       //使能LVD中断

	//INT_CLKO |= 0x20;           //(EX3 = 1)使能INT3下降沿中断 唤醒
	
	//io 脚配置	 0000|0000   0000|1100
	P3M1 =  0x00;
	P3M0 =  0x0c;

	// 工作灯开始
	//ioWorkLED = 1;

	// 测试开启.
	ioSwitchLED = 0;
	ioKEY =1; //按键关闭

	//定时器配置 50ms 一次定时，定时器触发时，cpu进入唤醒时段.
	//Timer_config();
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0xB0;		//设置定时初值
	TH0 = 0x3C;		//设置定时初值
	ET0 = 1;
	//TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时

	EA=1;
	
	while (1)
	{

		//按键扫描 1、按键触发杀毒
		if(ioKEY == 0)                
		//如果有键按下，则条件成立（有键按下，则s4为0；而 !key_flag为1）
	    {
	    	delay_ms(20);//延时消抖
	        if(ioKEY == 0)                             //如果确定有键按下
	        {                      
	            //进行事件处理
	            //g_key_flag = 1;
	            //启动亮灯计时
	            g_key_time=g_light_on_time;
				ioWorkLED = 1;
				//PrintString("\r\n 按键已按下.");
				continue;
	        }
		}

	    //任务判断  2.自动任务触发杀毒
		switch(g_hour) {  //喝水时间点，在这个时候进行杀毒操作. (一天4次，每次20分钟，一天共1个小时.)
			 case 7:
			 case 13:
			 case 19:
			 case 23:
			 {
			 	if(g_key_time==0){ //只有在g_key_time处理非触发状态下，才进行触发操作.
					g_key_time=g_light_on_time;
				}
			 	
			 	break;
			  }
			default:
				//bIsOpen = 0;
			 	break;
		}

		/*
		执行任务动作1

		工作灯,每10秒闪一次
		*/
		if(g_second%10 == 0){
			ioWorkLED = 1;
			delay_ms(2000);
			ioWorkLED = 0;
		}
		else {
			delay_ms(900);
		}

		//结束任务. 
	}
}

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt 1
{
	//PrintString("\r\n T:");
	g_millisecond = g_millisecond + 50;

	if( g_millisecond >= 1000) { //满足一秒
		g_millisecond = 0;
		g_second++;
		//printNowTime();

		/////
		//实时判断
		if(g_key_time>0){
			ioSwitchLED = 1;
		}
		//////

		if(g_second >=60){

			//满足一分钟
			//执行任务
			///////////////////////////
			if(g_key_time<1){
				//复位.
				//g_key_flag =0; //用于控制黑灯
				ioSwitchLED = 0;
				g_key_time =0; //重置按键亮灯计时时间
			}
			else{
				//g_key_flag =1; //用于控制亮灯
				ioSwitchLED = 1;
				g_key_time--;
			}			
			///////////////////////////


			///时间累加
			g_second =0;
			g_minute ++;

			if(g_minute >=60){

				//满足一小时
				g_minute =0;
				g_hour ++;

				if(g_hour ==24){

					//满足24小时
					g_hour =0;
			
				}
			
			}
		}
	}
}

///低电压中断函数
//如果低电压，将	     LVD_VECTOR		6
void LVD_ISR() interrupt 6 using 1
{	
	//
	ioWorkLED = 1; //常亮，表示电压不足
	ioSwitchLED = 0; //熄灯

	PCON &= ~LVDF;                  //向PCON.5写0清LVD中断

	//PCON |= 0x02;               //进入掉电模式
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

	PrintString("\r\n 现在时间:");

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
