#include <C8051F120.h>                 // SFR declarations
#include <stdio.h>
#include <absacc.h>
#include "global.h"
#include "T6963.h"
#include "init.h"
#include "uart.h"
#include "DAC.h"
#include "ADC0.h"
#include "delay.h"
#include "PNL_30A.h"

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void Run();
void print_num(void);

void key_test();
void DA_test();
void ADC_test();
void Led_test();

static	char param[4][4]={0,0,1,0,1,0,0,0,0,0,3,0,1,0,0,0};


void main (void) 
{
	unsigned int i=0;
	char t;

	char run_status=0;
	char cursor_pos=2;
//	char m,n;

	WDTCN = 0xDE;                       // Disable Watchdog timer
	WDTCN = 0xAD;
	
	PLL_Init ();			   			// Initialize PLL
	PORT_Init();                        // Initialize Port I/O
	
	EMIF_Init ();                       // Initialize memory interface
	UART0_Init ();                      // Initialize UART0

    //ADC0_Init ();                       // Init ADC0
	//DAC0_Init();						// Init DAC0
    //DAC1_Init();						// Init DAC1
	TIMER4_Init(SYSCLK/SAMPLE_RATE_DAC);// Use Time4 as DAC trigger

	SFRPAGE = CONFIG_PAGE;              // P4 is on SFR Page F
	SFRPAGE = LEGACY_PAGE;

	PNL_30A_Configuration();

    T6963_init_lcd();                   // Initialize T6963
	EA =1;                              // Enable Interrupt	
	//Set_DAC_Frequent(1000);	

 /****************界面初始化**********************/
	 T6963_lcd_clr();
	 T6963_printf("      Centrifuge\n");	 //打印设备名称
	 Draw_horizon(1,10,126);				 //打印双横线
	 Draw_horizon(1,12,126);
	 set_coordinate(0,2);					 //设置第二行字符位置
	 T6963_printf(" STAGE 1:\n");
	 T6963_printf("   Time :   10 s\n");
	 T6963_printf("   Speed: 1000 r/m\n");
	 T6963_printf(" STAGE 2:\n");
	 T6963_printf("   Time :   30 s\n");
	 T6963_printf("   Speed: 3000 r/m\n");	 
	 Open_Cursor(11,3,7);					 //开启光标
 	
	while(1)
	{
	 print_num();	  //更新设置参数

	if(run_status==0)
	{
	 if(cursor_pos<8)
	 	Open_Cursor(10+cursor_pos%4,3+cursor_pos/4,7);
	 else Open_Cursor(10+cursor_pos%4,4+cursor_pos/4,7);
	 }
	 else NO_Cursor();
	 t = PNL_30A_Key_get();
	 if(run_status==0)	   //非运行状态
	 {
		 switch(t)
		 {	//数字输入功能
		 	case 2:param[cursor_pos/4][cursor_pos%4]=1;	break;	
			case 3:param[cursor_pos/4][cursor_pos%4]=2; break;	
			case 4:param[cursor_pos/4][cursor_pos%4]=3;	break;
			case 8:param[cursor_pos/4][cursor_pos%4]=4;	break;
			case 9:param[cursor_pos/4][cursor_pos%4]=5;	break;
			case 10:param[cursor_pos/4][cursor_pos%4]=6;break;
			case 14:param[cursor_pos/4][cursor_pos%4]=7;break;
			case 15:param[cursor_pos/4][cursor_pos%4]=8;break;
			case 16:param[cursor_pos/4][cursor_pos%4]=9;break;
			case 21:param[cursor_pos/4][cursor_pos%4]=0;break;
			//移动光标方向按键功能
			case 22:if(cursor_pos==0)cursor_pos=15;else cursor_pos--;break;
			case 24:if(cursor_pos==15)cursor_pos=0;else cursor_pos++;break;			
			case 17:if(cursor_pos<4)cursor_pos=cursor_pos+12;else cursor_pos-=4;break;
			case 23:if(cursor_pos>11)cursor_pos=cursor_pos-12;else cursor_pos+=4;break;
			//
			case '-':if(param[cursor_pos/4][cursor_pos%4]<9) param[cursor_pos/4][cursor_pos%4]++;break;
			case '+':if(param[cursor_pos/4][cursor_pos%4]>0) param[cursor_pos/4][cursor_pos%4]--;break;
			//启动执行按键
			case 5:
			case 18:
			case 25:run_status=1;Run();break;	
			default: break;				
	
		  }
   	}
	else		//运行状态，其他按键都失效，只有stop和Esc按键有效
	{
		  switch(t)
		  {
		  	case 6:
			case 12:	run_status=0;break;
			default: break;

		  }

	}
   }
}

//刷新输出设置参数
void print_num(void)
{
	 char m,n;
	 char state;
	 char posi;
	 for(m=0;m<4;m++)
	 {
	 if(m<2)
	 	set_coordinate(10,3+m);
	 else	set_coordinate(10,4+m);
	     state=0;
		 for(n=0;n<4;n++)
		 {
		 	if(param[m][n]==0 && state==0)
		 		T6963_putchar(' ');
		 	else 
			{
			T6963_putchar(param[m][n]+0x30);
			state=1;
			}			
		 }
	 }
}

//运行参数
void Run()
{

	PNL_30A_Buzzer_on();
	delay_ms(200);
	PNL_30A_Buzzer_off();


}

void ADC_test()
{
	float m;
	char  t;
	while(1)
	{
		ADC0_start(ADC_CH0);
		m = ADC0_get()* 2.4300 / 4095;
		T6963_printf("ADC_CH0 voltage:\n %1.3fV\n",m);
		ADC0_start(ADC_CH1);
		m = ADC0_get()* 2.4300 / 4095;
		T6963_printf("ADC_CH1 voltage:\n %1.3fV\n",m);
		ADC0_start(ADC_CH2);
		m = ADC0_get()* 2.430 / 4095;
		T6963_printf("ADC_CH2 voltage:\n %1.3fV\n",m);
		ADC0_start(ADC_CH3);
		m = ADC0_get()* 2.4300 / 4095;
		T6963_printf("ADC_CH3 voltage:\n %1.3fV\n",m);
		t =PNL_30A_Key_return();
		if(t==12)break;
		delay_ms(200);
		T6963_lcd_back();
	}

}

void key_test()
{
	char t;
	while(1)
	{
		t =PNL_30A_Key_get();
		if(t==12)break;
		T6963_printf("%bd",t);
	}
}

void DA_test()
{
	char t;
	int  f=100;
	
	while(1)
	{
		Set_DAC_Frequent(f);
		T6963_printf("The frequent\n");
		T6963_printf("    %02d    Hz",f);
		t =PNL_30A_Key_get();
		if(t==12)break;
		else if(t=='+')
			f+=10;
		else if(t=='-')
			f-=10;
		T6963_lcd_back();
	}	
}

void Led_test()
{
	char t;
	char i=1;
	while(1)
	{
		PNL_30A_Led_set(i);
		t =PNL_30A_Key_return();
		if(t==12)break;
		i<<=1;
		if(i%0xf==0)i=1;	
	}		
}

void Buzzer_test()
{
//	char t;
//	char i=1;
//	while(1)
//	{
//		PNL_30A_Led_set(i);
//		t =PNL_30A_Key_return();
//		if(t==12)break;
//		i<<1;
//		if(i%0xf==0)i=1;	
//	}		
}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------