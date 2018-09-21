
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


/*************	功能说明	**************

改装[自动喷香机]

1、一天喷时间设置为：7点，13点，19点
2、
T(io) 按键用于每天12点较准时间，触发即设定当前时间为12点.
M 用于主动触发喷香机运行一次。
S  关闭当程序运行.

3、L1(绿io0)显示当前状态，
10秒闪一次，表示机器定时运行中。
常亮表示电池不足。

L2(红io1)闪一次，表示时间较准完成。
L2(红)闪二次，表示计数恢复完成。 
L2(红)闪三次，表示关闭定时


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


sbit ioInKeyForTime = P3^2; //时间较准按键输入
sbit ioInKeyForRun  = P3^3;//程序是否运行输入 
sbit ioInKeyForMoto =P3^5;//喷香机触发运行输入 

sbit ioOutForL1Green= P3^0;  //绿灯，程序运行指示灯
sbit ioOutForL2Red  = P3^1;  //红灯，按键响应指示灯
sbit ioOutForMoto   = P3^4;  //喷香机驱动IO口.

u8 ioSwitchLED=0;
u8 ioWorkLED=0;
u8 ioKEY=0;

typedef struct
{
	u8	sKeyForTime;  //时间键状态值.  0: 未按下  //1:已按下
	u8	sKeyForRun;	  //程序键状态值.  0: 未按下  //1:已按下
       u8   sKeyForMoto;  //喷香机键状态值.  0: 未按下  //1:已按下
} KeyStateDef; //当前程序按键的状态,默认为0 

//全局对象
KeyStateDef  g_allKeyState={0,0,0};



/*************	本地函数声明	**************/

void mintueAction(void);

void secondAction(void);

void printNowTime(void);

void key_scan(void);

/*************  外部函数和变量声明 *****************/
void	EXTI_config(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//结构定义

    //初始化INT0
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_RiseFall;	//中断模式,  EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityHigh;			//中断优先级,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//中断允许,     ENABLE或DISABLE
	Ext_Inilize(EXT_INT0,&EXTI_InitStructure);	
    
    //初始化INT1
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//中断模式,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//中断优先级,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//中断允许,     ENABLE或DISABLE
	Ext_Inilize(EXT_INT1,&EXTI_InitStructure);				//	EXT_INT0,EXT_INT1,EXT_INT2,EXT_INT3,EXT_INT4

    //初始化INT3
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//中断模式,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//中断优先级,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//中断允许,     ENABLE或DISABLE
	Ext_Inilize(EXT_INT3,&EXTI_InitStructure);
}



/************************ IO口配置 ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//结构定义
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_5;	//指定要初始化的IO, 
	// GPIO_Pin_2 ~ GPIO_Pin_3, 或操作
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//指定IO的输入或输出方式,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//初始化


	GPIO_InitStructure.Pin   = GPIO_Pin_2 |GPIO_Pin_3 | GPIO_Pin_4 ;	//指定要初始化的IO,
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

}


/******************** 主函数**************************/
void main(void)
{
	
	//io 脚配置
	GPIO_config();

    //全局变量初使化
	// 工作灯开始
      ioOutForL1Green=ioInKeyForMoto=ioInKeyForRun=ioInKeyForTime=1; //key键拉高

      //输出io脚先关闭
      ioOutForL2Red =ioOutForMoto = 0;

        //g_allKeyState = {0,0,0};


        //开启引脚p32 p33的外部中断,下降沿中断.
        EXTI_config();
    
	//定时器配置 50ms 一次定时，定时器触发时，cpu进入唤醒时段.
	Timer_config();

	EA = 1;
		 
	//PrintString("\r\n 开始工作了... = ");

	while (1)
	{
		//key_scan();

			/*
		执行任务动作1

		1,4,7,10,13,16,19,22 开始照明.
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

	//PrintString("\r\n 现在时间:");

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



/********************* INT0中断函数 *************************/
void INT0_int (void) interrupt INT0_VECTOR		//进中断时已经清除标志
{
	//WakeUpSource = 1;	//标记INT0唤醒
	//EX0 = 0;			//INT0 Disable
	IE0  = 0;			//外中断0标志位
    PrintString("\r\n 外部中断0.");
}

/********************* INT1中断函数 *************************/
void INT1_int (void) interrupt INT1_VECTOR		//进中断时已经清除标志
{
	//进入中断时，可以把int1中断关闭。
	EX1 = 0;	
	//tfWakeUpSource = 2;	//标记INT1唤醒
	//EX1 = 0;			//INT1 Disable
	IE1  = 0;			//外中断1标志位

	//处理完中断事件后，可以把 int1中断开启，防止多次进入。

    PrintString("\r\n 外部中断1.延时1秒");
    delay_ms(2000);
	EX1 = 1;
}

/********************* INT3中断函数 ************************/
void INT3_int  (void) interrupt INT3_VECTOR
{
    PrintString("\r\n 外部中断3.延时1秒");

/*
    INT_CLKO |=  (1 << 5);  //允许中断  
    INT_CLKO &= ~(1 << 5);  //禁止中断
*/
	//EX3 = 0;	
    //IE1  = 0;	
}


