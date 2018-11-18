
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


/*************	¹¦ÄÜËµÃ÷	**************

¸Ä×°[×Ô¶¯ÅçÏã»ú]

1¡¢Ò»ÌìÅçÊ±¼äÉèÖÃÎª£º7µã£¬13µã£¬19µã
2¡¢
T(io) °´¼üÓÃÓÚÃ¿Ìì12µã½Ï×¼Ê±¼ä£¬´¥·¢¼´Éè¶¨µ±Ç°Ê	±¼äÎª12µã.
M ÓÃÓÚÖ÷¶¯´¥·¢ÅçÏã»úÔËĞĞÒ»´Î¡£
S  ¹Ø±Õµ±³ÌĞòÔËĞĞ.

3¡¢L1(ÂÌio0)ÏÔÊ¾µ±Ç°×´Ì¬£¬
10ÃëÉÁÒ»´Î£¬±íÊ¾»úÆ÷¶¨Ê±ÔËĞĞÖĞ¡£
³£ÁÁ±íÊ¾µç³Ø²»×ã¡£

L2(ºìio1)ÉÁÒ»´Î£¬±íÊ¾Ê±¼ä½Ï×¼Íê³É¡£
L2(ºì)ÉÁ¶ş´Î£¬±íÊ¾¼ÆÊı»Ö¸´Íê³É¡£ 
L2(ºì)ÉÁÈı´Î£¬±íÊ¾¹Ø±Õ¶¨Ê±

/*stc15f104w Òı½Å°²ÅÅ
*********************************************************
×Ô¶¯Ä£Ê½  p3.4  -- ||--p3.3  ÅçÏã»ú°´¼ü¿ØÖÆÊäÈë
ÕıµçÔ´       vcc    -- ||--p3.2  Ê±¼äÖØÖÃ°´¼ü¿ØÖÆÊäÈë
µç»ú¿ª¹Ø  p3.5  -- ||--p3.1  ºìµÆ
¸ºµçÔ´        gnd  -- ||--p3.0  ÂÌµÆ
*********************************************************

*/

/******************************************/

/*************	±¾µØ³£Á¿ÉùÃ÷	**************/
#define l2Num_TimeRest_1 1
#define l2Num_MotoRun_2  2
#define k_addNum 50 //50 //¶¨Ê±µş¼ÓÊı
#define k_moto_run_time (500) //500ms //µç»úÆôÊ±¼äÖµ¡£

/*************	±¾µØ±äÁ¿ÉùÃ÷	**************/
u8 g_hour=6;  //Ê±
u8  g_minute=59;//·Ö
u8  g_second=50;//Ãë
u16 g_millisecond=0;//ºÁÃë 



sbit ioOutForL1Green= P3^0;  //ÂÌµÆ£¬³ÌĞòÔËĞĞÖ¸Ê¾µÆ
sbit ioOutForL2Red  = P3^1;  //ºìµÆ£¬°´¼üÏìÓ¦Ö¸Ê¾µÆ

sbit ioInKeyForMoto = P3^2;//ÅçÏã»ú´¥·¢ÔËĞĞÊäÈë  int0
sbit ioInKeyForTime = P3^3; //Ê±¼ä½Ï×¼°´¼üÊäÈë  int1


sbit ioOutForMotoPower=P3^5;  //ÅçÏã»úµç»úÆô¶¯¿ª¹ØIO¿Ú.
//sbit ioOutForMotoKey= P3^4;  //ÔİÊ±²»×ö´¦Àí£¬±£ÁôIO¿Ú



int g_moto_run_time=0; //µç»úÆô¶¯ÔËĞĞºóµÄÊ±¼äÖµ
 
typedef struct
{
	u8	sKeyForTime;  //ÖØÖÃÊ±¼ä¼ü×´Ì¬Öµ.  0: Î´°´ÏÂ  //1:ÒÑ°´ÏÂ
	//u8	sKeyForRun;	  //³ÌĞò¼ü×´Ì¬Öµ.  0: Î´°´ÏÂ  //1:ÒÑ°´ÏÂ
	u8     sKeyForMoto;  //ÅçÏã»ú¼ü×´Ì¬Öµ.  0: Î´°´ÏÂ  //1:ÒÑ°´ÏÂ
} KeyStateDef; //µ±Ç°³ÌĞò°´¼üµÄ×´Ì¬,Ä¬ÈÏÎª0 

//È«¾Ö¶ÔÏó
KeyStateDef  g_allKeyState={0,0};


/*************	±¾µØº¯ÊıÉùÃ÷	**************/

//void mintueAction(void);

void secondAction(void);

//void printNowTime(void);

void key_scan(void);

void l2ShowWithNum(u8 num);

void motoStart(void);
/*************  Íâ²¿º¯ÊıºÍ±äÁ¿ÉùÃ÷ *****************/
void	EXTI_config(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//½á¹¹¶¨Òå

    //³õÊ¼»¯INT0
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//ÖĞ¶ÏÄ£Ê½,  EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityHigh;			//ÖĞ¶ÏÓÅÏÈ¼¶,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//ÖĞ¶ÏÔÊĞí,     ENABLE»òDISABLE
	Ext_Inilize(EXT_INT0,&EXTI_InitStructure);	
    
    //³õÊ¼»¯INT1
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//ÖĞ¶ÏÄ£Ê½,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//ÖĞ¶ÏÓÅÏÈ¼¶,   PolityLow,PolityHigh
	//EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//ÖĞ¶ÏÔÊĞí,     ENABLE»òDISABLE
	Ext_Inilize(EXT_INT1,&EXTI_InitStructure);				//	EXT_INT0,EXT_INT1,EXT_INT2,EXT_INT3,EXT_INT4

/*
    //³õÊ¼»¯INT3
	EXTI_InitStructure.EXTI_Mode      = EXT_MODE_Fall;	//ÖĞ¶ÏÄ£Ê½,  	EXT_MODE_RiseFall, EXT_MODE_Fall
	EXTI_InitStructure.EXTI_Polity    = PolityLow;			//ÖĞ¶ÏÓÅÏÈ¼¶,   PolityLow,PolityHigh
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;				//ÖĞ¶ÏÔÊĞí,     ENABLE»òDISABLE
	Ext_Inilize(EXT_INT3,&EXTI_InitStructure);
	*/
}



/************************ IO¿ÚÅäÖÃ ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;				//½á¹¹¶¨Òå
	GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_4|GPIO_Pin_5;	//Ö¸¶¨Òª³õÊ¼»¯µÄIO, 
	// GPIO_Pin_2 ~ GPIO_Pin_3, »ò²Ù×÷
	GPIO_InitStructure.Mode = GPIO_OUT_PP;				//Ö¸¶¨IOµÄÊäÈë»òÊä³ö·½Ê½,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);			//³õÊ¼»¯


	GPIO_InitStructure.Pin   = GPIO_Pin_2 |GPIO_Pin_3  ;	//Ö¸¶¨Òª³õÊ¼»¯µÄIO,
	GPIO_InitStructure.Mode = GPIO_PullUp;				//Ö¸¶¨IOµÄÊäÈë»òÊä³ö·½Ê½,GPIO_PullUp,GPIO_HighZ,GPIO_OUT_OD,GPIO_OUT_PP
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);	
	
}

/************************ ¶¨Ê±Æ÷ÅäÖÃ ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//½á¹¹¶¨Òå
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//Ö¸¶¨¹¤×÷Ä£Ê½,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//Ö¸¶¨ÖĞ¶ÏÓÅÏÈ¼¶, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//ÖĞ¶ÏÊÇ·ñÔÊĞí,   ENABLE»òDISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//Ö¸¶¨Ê±ÖÓÔ´,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//ÊÇ·ñÊä³ö¸ßËÙÂö³å, ENABLE»òDISABLE
	TIM_InitStructure.TIM_Value     =  0x3cb0;//65536UL - (50.0/1000.0 * MAIN_Fosc/12.0);		
	// //³õÖµ,50ºÁÃëÒ»´ÎÖĞ¶Ï.
	TIM_InitStructure.TIM_Run       = ENABLE;				//ÊÇ·ñ³õÊ¼»¯ºóÆô¶¯¶¨Ê±Æ÷, ENABLE»òDISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//³õÊ¼»¯Timer0	  Timer0,Timer1,Timer2

}


/******************** Ö÷º¯Êı**************************/
void main(void)
{
	//µÍµçÑ¹¼ì²â±£»¤
	PCON &= ~LVDF;                  //ÉÏµçºóĞèÒªÇåLVDÖĞ¶Ï±êÖ¾Î»
    ELVD = 1;                       //Ê¹ÄÜLVDÖĞ¶Ï
    
	//io ½ÅÅäÖÃ
	GPIO_config();

    //È«¾Ö±äÁ¿³õÊ¹»¯
	// ¹¤×÷µÆ¿ªÊ¼
    ioOutForL1Green=ioInKeyForMoto=ioInKeyForTime=1; //key¼üÀ­¸ß

    //Êä³öio½ÅÏÈ¹Ø±Õ
    ioOutForL2Red =ioOutForMotoPower = 0;

    //ioOutForMotoKey=1; //´¥·¢ÅçÏã»úµÄ°´¼ü¿ª¹Ø. µÍÎ»¿ª£¬¸ßÎ»¹Ø.
 	
           
    //g_allKeyState = {0,0,0};


    //¿ªÆôÒı½Åp32 p33µÄÍâ²¿ÖĞ¶Ï,ÏÂ½µÑØÖĞ¶Ï.
    //EXTI_config();
    
	//¶¨Ê±Æ÷ÅäÖÃ 50ms Ò»´Î¶¨Ê±£¬¶¨Ê±Æ÷´¥·¢Ê±£¬cpu½øÈë»½ĞÑÊ±¶Î.
	Timer_config();

	EA = 1;
		 
	//PrintString("\r\n ¿ªÊ¼¹¤×÷ÁË... = ");

	while (1)
	{
		key_scan();

			/*
		Ö´ĞĞÈÎÎñ¶¯×÷1

		Ò»ÌìÅçÊ±¼äÉèÖÃÎª£º7µã£¬13µã£¬19µã(Õû)
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

/***ÄÚ²¿¹¦ÄÜº¯Êı***/

// °´¼ü´¦Àí¹¦ÄÜ
void key_scan(void) {

	//moto	 
	
	 if(ioInKeyForMoto==0){
		
	   while(ioInKeyForMoto==0);
	   //
	   //´¥·¢ÅçÏã»ú¹¤×÷
		motoStart();
		l2ShowWithNum(l2Num_MotoRun_2);
	}

	//time		 ioInKeyForTime
	if(ioInKeyForTime==0){
		
	   while(ioInKeyForTime==0);
	   //
	   //ÖØÖÃÊ±¼äÎª12µã.
		g_hour = 12;
		g_minute=g_second=g_millisecond =0;

        l2ShowWithNum(l2Num_TimeRest_1);
	}


 /*
	if(g_allKeyState.sKeyForMoto == 1) {
		//Çå¿ÕÔ­×´Ì¬
		g_allKeyState.sKeyForMoto = 0;

		
		
	}

	if(g_allKeyState.sKeyForRun== 1) {

		//Í£Ö¹APPÔËĞĞ
		while(g_allKeyState.sKeyForRun){;};
		
		//Çå¿ÕÔ­×´Ì¬
		//g_allKeyState.sKeyForRun = 0;
	}

	if(g_allKeyState.sKeyForTime== 1) {
		//Çå¿ÕÔ­×´Ì¬
		g_allKeyState.sKeyForTime = 0;

		
		
	}	

	*/
}

//ÁÁµÆÏìÓ¦
/*
3¡¢L1(ÂÌio0)ÏÔÊ¾µ±Ç°×´Ì¬£¬
10ÃëÉÁÒ»´Î£¬±íÊ¾»úÆ÷¶¨Ê±ÔËĞĞÖĞ¡£
³£ÁÁ±íÊ¾µç³Ø²»×ã¡£

L2(ºìio1)ÉÁÒ»´Î£¬±íÊ¾Ê±¼ä½Ï×¼Íê³É¡£
L2(ºì)ÉÁ¶ş´Î£¬±íÊ¾´¥·¢ÅçÏã»ú¹¤×÷¡£ 
L2(ºì)ÉÁÈı´Î£¬±íÊ¾¹Ø±ÕAPPÔËĞĞ
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

/*µç»ú¿ØÖÆ

1.´ò¿ªp34,      (10Ãë) ºó¹Ø±Õ(Ïàµ±ÓÚ¿ª¹ØµçÔ´)
2.µÈ´ıÅçÏã»úÆô¶¯,Ô¼(1Ãë)ÖÓ,¼´8(k_moto_run_time-2)ÃëÊ±Æô¶¯²Ù×÷.
3.´ò¿ªp35, 20us Ãë¹Ø±Õ(Ä£ËÆ°´¼ü)
*/
void motoStart(void) {

			//Æô¶¯µç»ú³ÌĞò
			ioOutForL2Red = 1;
			ioOutForMotoPower = 1; //¿ªÆôµçÔ´
	
	        delay_ms(400);
	
	        ioOutForL2Red = 0; //¹Øµç»úÔËĞĞÖ¸Ê¾µÆ
	         //¹Ø±Õµç»úµçÔ´
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
    
	if(g_second%10 == 0){   //¹¤×÷Ö¸Ê¾µÆ(ÂÌ)
		ioOutForL1Green = 1;
		//delay_ms(200);
		
	}
    else {
            ioOutForL1Green = 0;
        }
	
}


/********************* Timer0ÖĞ¶Ïº¯Êı************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	g_millisecond += k_addNum;

   //50mÒ»´Î.

   if(g_millisecond>400){
		 
   }

	if( g_millisecond>= 1000) { //Âú×ãÒ»Ãë
		g_millisecond = 0;
        
		secondAction();
		//milliAction();
		g_second++;

		if(g_second >=60){

			//Âú×ãÒ»·ÖÖÓ
			//mintueAction();

			
			g_second =0;
			g_minute ++;

			if(g_minute >=60){

				//Âú×ãÒ»Ğ¡Ê±
				g_minute =0;
				g_hour ++;

				if(g_hour >=24){

					//Âú×ã24Ğ¡Ê±
					g_hour =0;
			
				}
			
			}
		}
	}
}

/*
sbit ioInKeyForTime = P3^2; //Ê±¼ä½Ï×¼°´¼üÊäÈë  int0
sbit ioInKeyForRun  = P3^3;//³ÌĞòÊÇ·ñÔËĞĞÊäÈë   int1
sbit ioInKeyForMoto =P3^5;//ÅçÏã»ú´¥·¢ÔËĞĞÊäÈë  int3
*/

/********************* INT0ÖĞ¶Ïº¯Êı *************************/
void INT0_int (void) interrupt INT0_VECTOR		//½øÖĞ¶ÏÊ±ÒÑ¾­Çå³ı±êÖ¾
{
	//WakeUpSource = 1;	//±ê¼ÇINT0»½ĞÑ
	//EX0 = 0;			//INT0 Disable

	IE0  = 0;			//ÍâÖĞ¶Ï0±êÖ¾Î»
	while(ioInKeyForMoto==0);
	//g_allKeyState.sKeyForMoto= 1;
        //PrintString("\r\n Íâ²¿ÖĞ¶Ï0.");
        
        EX0 = 1;

	
}

/********************* INT1ÖĞ¶Ïº¯Êı *************************/
void INT1_int (void) interrupt INT1_VECTOR		//½øÖĞ¶ÏÊ±ÒÑ¾­Çå³ı±êÖ¾
{
	//½øÈëÖĞ¶ÏÊ±£¬¿ÉÒÔ°Ñint1ÖĞ¶Ï¹Ø±Õ¡£
	EX1 = 0;	
	//tfWakeUpSource = 2;	//±ê¼ÇINT1»½ĞÑ
	//EX1 = 0;			//INT1 Disable
	IE1  = 0;			//ÍâÖĞ¶Ï1±êÖ¾Î»

	if(ioInKeyForTime!=0) 
	{	 
		EX1 = 1;
		return;
	}

	g_allKeyState.sKeyForTime= 1;
	g_allKeyState.sKeyForMoto= 1;
	//´¦ÀíÍêÖĞ¶ÏÊÂ¼şºó£¬¿ÉÒÔ°Ñ int1ÖĞ¶Ï¿ªÆô£¬·ÀÖ¹¶à´Î½øÈë¡£

   // PrintString("\r\n Íâ²¿ÖĞ¶Ï1.ÑÓÊ±1Ãë");
   // delay_ms(2000);
    while(ioInKeyForTime==0);
	EX1 = 1;	  //³ÌĞò¼ü×´Ì¬Öµ.  0: Î´°´ÏÂ  //1:ÒÑ°´ÏÂ
  
}

/********************* INT3ÖĞ¶Ïº¯Êı ***********************
void INT3_int  (void) interrupt INT3_VECTOR
{
    PrintString("\r\n Íâ²¿ÖĞ¶Ï3.ÑÓÊ±1Ãë");

//
    INT_CLKO |=  (1 << 5);  //ÔÊĞíÖĞ¶Ï  
    INT_CLKO &= ~(1 << 5);  //½ûÖ¹ÖĞ¶Ï
///
	//EX3 = 0;	
    //IE1  = 0;	

	g_allKeyState.sKeyForMoto= 1;
}
*/

///µÍµçÑ¹ÖĞ¶Ïº¯Êı
//Èç¹ûµÍµçÑ¹£¬½«	     LVD_VECTOR		6
void LVD_ISR() interrupt 6 using 1
{	
	//
	ioOutForL2Red   = 1; //³£ÁÁ£¬±íÊ¾µçÑ¹²»×ã
	ioOutForL1Green= 0; //Ï¨µÆ
	ioOutForMotoPower =0; //Ç¿ÖÆÍ£Ö¹ÔË×ª.

	PCON &= ~LVDF;                  //ÏòPCON.5Ğ´0ÇåLVDÖĞ¶Ï

	//PCON |= 0x02;               //½øÈëµôµçÄ£Ê½
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

