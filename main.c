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

void key_test();
void DA_test();
void ADC_test();
void Led_test();

void main (void) 
{
	unsigned int i=0;
	char t;

	WDTCN = 0xDE;                       // Disable Watchdog timer
	WDTCN = 0xAD;
	
	PLL_Init ();			   			// Initialize PLL
	PORT_Init();                        // Initialize Port I/O
	
	EMIF_Init ();                       // Initialize memory interface
	UART0_Init ();                      // Initialize UART0

    ADC0_Init ();                       // Init ADC0
	DAC0_Init();						// Init DAC0
    DAC1_Init();						// Init DAC1
	TIMER4_Init(SYSCLK/SAMPLE_RATE_DAC);// Use Time4 as DAC trigger

	SFRPAGE = CONFIG_PAGE;              // P4 is on SFR Page F
	SFRPAGE = LEGACY_PAGE;

	PNL_30A_Configuration();

    T6963_init_lcd();                   // Initialize T6963

	EA =1;                              // Enable Interrupt
	
	Set_DAC_Frequent(1000);	


	
	while(1)
	{
		 T6963_lcd_clr();
		 T6963_printf("Meun\n");
		 T6963_printf("1:Key and LCD test\n");
		 T6963_printf("2:DA test\n");
		 T6963_printf("3:AD test\n");
		 T6963_printf("4:LED test\n");
		 t = PNL_30A_Key_get();
		 T6963_lcd_clr();
		 switch(t)
		 {
		 	case 2:
				key_test();
				break;	
			case 3:
				DA_test();
				break;	
			case 4:
				ADC_test();
				break;
			case 8:
				Led_test();
				break;					

		 }
	}
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