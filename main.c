#include <C8051F120.h>                 // SFR declarations
#include <stdio.h>
#include <stdlib.h>
#include <absacc.h>
#include "global.h"
#include "T6963.h"
#include "init.h"
#include "uart.h"
#include "DAC.h"
#include "ADC0.h"
#include "delay.h"
#include "PNL_30A.h"
#include "brushlessmotor.h"

//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
void Run();
void print_num(char str[4][4]);
void itoaTest(unsigned int num,char str[]);

//void key_test();
//void DA_test();
//void ADC_test();
//void Led_test();


void main (void) 
{
	char t;
	bit run_status=0;
	char cursor_pos=2;
	xdata unsigned int cur_time1,cur_time2,cur_speed1,cur_speed2;
	char m,n;
	xdata char temp_a[4][4];
	char param[4][4]={0,0,1,0,1,0,0,0,0,0,3,0,3,0,0,0};
	WDTCN = 0xDE;                       // Disable Watchdog timer
	WDTCN = 0xAD;
	
	PLL_Init ();			   			// Initialize PLL
	PORT_Init();                        // Initialize Port I/O
	
	EMIF_Init ();                       // Initialize memory interface
	UART0_Init ();                      // Initialize UART0

    //ADC0_Init ();                       // Init ADC0
	//DAC0_Init();						// Init DAC0
    //DAC1_Init();						// Init DAC1
	//TIMER4_Init(SYSCLK/SAMPLE_RATE_DAC);// Use Time4 as DAC trigger

	SFRPAGE = CONFIG_PAGE;              // P4 is on SFR Page F
	SFRPAGE = LEGACY_PAGE;

	PNL_30A_Configuration();

    T6963_init_lcd();                   // Initialize T6963
	EA =1;                              // Enable Interrupt	
	//Set_DAC_Frequent(1000);	
	brushless_motor_init();

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
		if(run_status==0)
		{
			 print_num(param);	  //更新设置参数
			 if(cursor_pos<8)
			 	Open_Cursor(10+cursor_pos%4,3+cursor_pos/4,7);
			 else Open_Cursor(10+cursor_pos%4,4+cursor_pos/4,7);

 			 t = PNL_30A_Key_get(); 			
			 switch(t)
				 {	//数字输入功能
				 	case 2:param[cursor_pos/4][cursor_pos%4]=1;if(cursor_pos<15)cursor_pos++;break;	
					case 3:param[cursor_pos/4][cursor_pos%4]=2;if(cursor_pos<15)cursor_pos++;break;	
					case 4:param[cursor_pos/4][cursor_pos%4]=3;if(cursor_pos<15)cursor_pos++;break;
					case 8:param[cursor_pos/4][cursor_pos%4]=4;if(cursor_pos<15)cursor_pos++;break;
					case 9:param[cursor_pos/4][cursor_pos%4]=5;if(cursor_pos<15)cursor_pos++;break;
					case 10:param[cursor_pos/4][cursor_pos%4]=6;if(cursor_pos<15)cursor_pos++;break;
					case 14:param[cursor_pos/4][cursor_pos%4]=7;if(cursor_pos<15)cursor_pos++;break;
					case 15:param[cursor_pos/4][cursor_pos%4]=8;if(cursor_pos<15)cursor_pos++;break;
					case 16:param[cursor_pos/4][cursor_pos%4]=9;if(cursor_pos<15)cursor_pos++;break;
					case 21:param[cursor_pos/4][cursor_pos%4]=0;if(cursor_pos<15)cursor_pos++; break;
					//移动光标方向按键功能
					case 22:if(cursor_pos==0)cursor_pos=15;else cursor_pos--;break;
					case 24:if(cursor_pos==15)cursor_pos=0;else cursor_pos++;break;			
					case 17:if(cursor_pos<4)cursor_pos=cursor_pos+12;else cursor_pos-=4;break;
					case 23:if(cursor_pos>11)cursor_pos=cursor_pos-12;else cursor_pos+=4;break;
					//旋钮功能区
					case '-':if(param[cursor_pos/4][cursor_pos%4]<9) param[cursor_pos/4][cursor_pos%4]++;break;
					case '+':if(param[cursor_pos/4][cursor_pos%4]>0) param[cursor_pos/4][cursor_pos%4]--;break;
					//启动执行按键
					case 5:
					case 18:
					case 25:run_status=1;
							cur_time1=cur_time2=0;
							for(m=0;m<4;m++){cur_time1=cur_time1*10+param[0][m];cur_time2=cur_time2*10+param[2][m];}
							for(m=0;m<4;m++)  for(n=0;n<4;n++)	temp_a[m][n]=param[m][n];
							cur_speed1 = cur_speed2 = 0; 
							for(m=0;m<4;m++)
							{
								cur_speed1=cur_speed1*10+param[1][m];
								cur_speed2=cur_speed2*10+param[3][m];
							}
							start();
							set_speed(cur_speed1);
							break;	
					default: break;							
				  }
		}
		else //run_status==1 	//执行阶段的程序
			{
			  NO_Cursor();
			  itoaTest(cur_time1,temp_a[0]);	//实时更新剩余时间			  
			  itoaTest(cur_time2,temp_a[2]);
			  print_num(temp_a);
			  if(cur_time1+cur_time2>0)		 //倒计时运行阶段
			  {
			  	delay_ms(1000);
			  	if(cur_time1>0){
			  		cur_time1--;
					if(cur_time1 == 0)
					set_speed(cur_speed2);
				}
				else {	
					cur_time2--;
				}
			  }
			  if(cur_time1==0 && cur_time2==0 ){
			     run_status=0; 
				 set_speed(0); //运行结束，判断退出
			  }	
			  t = PNL_30A_Key_return(); 				//运行状态，其他按键都失效，只有stop和Esc按键有效
			  switch(t)
			  {
			  	case 6:
				case 12:	run_status=0; set_speed(0);break;
				default: break;
			  }
		   }
     }

}

//刷新输出设置参数
void print_num(char str[4][4])
{
	 char m,n;
	 char state;

	 for(m=0;m<4;m++)
	 {
	 if(m<2)
	 	set_coordinate(10,3+m);
	 else	set_coordinate(10,4+m);
	     state=0;
		 for(n=0;n<4;n++)
		 {
		 	if(str[m][n]==0 && state==0)
		 		T6963_putchar(' ');
		 	else 
			{
			T6963_putchar(str[m][n]+0x30);
			state=1;
			}
			if(n==2)state=1;			
		 }
	 }
}

//运行参数
void Run()
{

//	PNL_30A_Buzzer_on();
//	delay_ms(200);
//	PNL_30A_Buzzer_off();

}

void itoaTest(unsigned int num,char str[])
{

unsigned char i;
unsigned char temp[4];
	for (i=0;i<4;i++)
	{
		str[3-i]=num%10;
		num/=10;
	}
}

//void ADC_test()
//{
//	float m;
//	char  t;
//	while(1)
//	{
//		ADC0_start(ADC_CH0);
//		m = ADC0_get()* 2.4300 / 4095;
//		T6963_printf("ADC_CH0 voltage:\n %1.3fV\n",m);
//		ADC0_start(ADC_CH1);
//		m = ADC0_get()* 2.4300 / 4095;
//		T6963_printf("ADC_CH1 voltage:\n %1.3fV\n",m);
//		ADC0_start(ADC_CH2);
//		m = ADC0_get()* 2.430 / 4095;
//		T6963_printf("ADC_CH2 voltage:\n %1.3fV\n",m);
//		ADC0_start(ADC_CH3);
//		m = ADC0_get()* 2.4300 / 4095;
//		T6963_printf("ADC_CH3 voltage:\n %1.3fV\n",m);
//		t =PNL_30A_Key_return();
//		if(t==12)break;
//		delay_ms(200);
//		T6963_lcd_back();
//	}
//
//}


//
//void key_test()
//{
//	char t;
//	while(1)
//	{
//		t =PNL_30A_Key_get();
//		if(t==12)break;
//		T6963_printf("%bd",t);
//	}
//}
//
//void DA_test()
//{
//	char t;
//	int  f=100;
//	
//	while(1)
//	{
//		Set_DAC_Frequent(f);
//		T6963_printf("The frequent\n");
//		T6963_printf("    %02d    Hz",f);
//		t =PNL_30A_Key_get();
//		if(t==12)break;
//		else if(t=='+')
//			f+=10;
//		else if(t=='-')
//			f-=10;
//		T6963_lcd_back();
//	}	
//}
//
//void Led_test()
//{
//	char t;
//	char i=1;
//	while(1)
//	{
//		PNL_30A_Led_set(i);
//		t =PNL_30A_Key_return();
//		if(t==12)break;
//		i<<=1;
//		if(i%0xf==0)i=1;	
//	}		
//}

//void Buzzer_test()
//{
////	char t;
////	char i=1;
////	while(1)
////	{
////		PNL_30A_Led_set(i);
////		t =PNL_30A_Key_return();
////		if(t==12)break;
////		i<<1;
////		if(i%0xf==0)i=1;	
////	}		
//}
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------