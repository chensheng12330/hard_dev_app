
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

/*************	功能说明	**************

改装[自动喷香机]

1、一天喷时间设置为：7点，13点，19点
2、T按键用于每天12点较准时间，触发即设定当前时间为12点.R用于恢复喷散计数为0。每次换瓶可重置.S 关闭当程序运行.
3、L1显示当前状态，常亮表示喷香液体不足. 
L1闪一次，表示时间较准完成。L1闪二次，表示计数恢复完成。 L1闪三次，表示关闭定时

******************************************/

/*************	本地常量声明	**************/


/*************	本地变量声明	**************/
u32 g_hour=0;  //时
u32 g_minute=0;//分
u32 g_second=0;//秒
u32 g_millisecond=0;//毫秒 
u8  g_addNum = 50; //50 //定时叠加数

u16 g_key_flag=0; //按键状态,(1:按下  0:未按下)
u8  g_key_time=0; //按键响应事件时间计时数.默认为20分钟. 单位为分钟
u8  g_light_on_time=1; //默认为20分钟. 单位为分钟

/*************	本地函数声明	**************/

void mintueAction(void);

void secondAction(void);

void printNowTime(void);

void key_scan(void);

/*************  外部函数和变量声明 *****************/



/************************ IO口配置 ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_2 | GPIO_Pin_3;	//指定要初始化的IO, 
	// GPIO_Pin_2 ~ GPIO_Pin_3, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//初始化


	GPIO_InitStructure.Pin  = GPIO_Pin_5;	//指定要初始化的IO,
	GPIO_InitStructure.Mode = GPIO_PullUp;				//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	
	
}

/************************ 定时器配置 ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//结构定义
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (50.0/1000.0 * MAIN_Fosc/12.0);		
	// //初值,50毫秒一次中断.
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//初始化Timer0	  Timer0,Timer1,Timer2

/*
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//指定工作模式,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//指定中断优先级, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//指定时钟源, TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = ENABLE;				//是否输出高速脉冲, ENABLE或DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / 50*12*2);		//初值,
	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//初始化Timer1	  Timer0,Timer1,Timer2
*/

//	TIM_InitStructure.TIM_Interrupt = ENABLE;				//中断是否允许,   ENABLE或DISABLE. (注意: Timer2固定为16位自动重装, 中断固定为低优先级)
//	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//指定时钟源,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
//	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//是否输出高速脉冲, ENABLE或DISABLE
//	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (50*12));		//初值
//	TIM_InitStructure.TIM_Run       = ENABLE;				//是否初始化后启动定时器, ENABLE或DISABLE
//	Timer_Inilize(Timer2,&TIM_InitStructure);				//初始化Timer2	  Timer0,Timer1,Timer2
}


sbit ioWorkLED   = P3^2;  //工作指示灯
sbit ioSwitchLED = P3^3;  //控制杀菌灯
sbit ioKEY       = P3^5;  //按键控杀菌灯

/******************** 主函数**************************/
void main(void)
{

	/*
	p3_2 : 用于工作指示灯
	p3_3 : 用于控制led照明显示
	*/
	
	//io 脚配置
	GPIO_config();

	// 工作灯开始
	ioWorkLED = 1;

	// 测试开启.
	ioSwitchLED = 1;
	ioKEY =1; //按键关闭

	//定时器配置 50ms 一次定时，定时器触发时，cpu进入唤醒时段.
	Timer_config();

	EA = 1;
		 
	PrintString("\r\n 开始工作了... = ");

	while (1)
	{
		key_scan();
        /*
		PCON |= 0x01;           //将IDL(PCON.0)置1,MCU将进入空闲模式
        _nop_();                //此时CPU无时钟,不执行指令
        _nop_();                //内部中断信号和外部复位信号可以终止空闲模式
        _nop_();
        _nop_();
        */

			/*
		执行任务动作1

		1,4,7,10,13,16,19,22 开始照明.
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

			//持续亮灯20分钟.
			if(g_minute<g_light_on_time || g_key_flag==1) {

				ioSwitchLED = 1;
			}
			else {
				ioSwitchLED = 0;
			}
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
			//ioWorkLED = 0;
			delay_ms(900);
		}

		//结束任务.

		/*	
		   PrintString("\r\n 循环中... ");

			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
			delay_ms(200);
		*/   
	}
}

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	g_millisecond += g_addNum;

	if( g_millisecond>= 1000) { //满足一秒
		g_millisecond = 0;
		secondAction();
		
		g_second++;

		if(g_second >=60){

			//满足一分钟
			mintueAction();
			g_second =0;
			g_minute ++;

			if(g_minute >=60){

				//满足一小时
				g_minute =0;
				g_hour ++;

				if(g_hour >=24){

					//满足24小时
					g_hour =0;
			
				}
			
			}
		}
	}
}

void mintueAction(void) {
	//PrintString("\r\n mintueAction... ");

	if(g_key_time<1){
		//复位.
		g_key_flag =0; //用于控制亮灯
		g_key_time =0; //重置按键亮灯计时时间
	}
	else{
		g_key_flag =1; //用于控制亮灯
		g_key_time--;
	}
}

void secondAction(void) {
	//PrintString("\r\n secondAction... ");
	printNowTime();
}

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
	//如果有键按下，则条件成立（有键按下，则s4为0；而 !key_flag为1）
    {
    	delay_ms(10);//延时消抖
        if(ioKEY == 0)                             //如果确定有键按下
        {                      
            //进行事件处理
            g_key_flag = 1;
            //启动亮灯计时
            g_key_time=g_light_on_time;

			PrintString("\r\n 按键已按下.");
            return ;
        }
	}
	//g_key_flag = 0;
	return ;
}

