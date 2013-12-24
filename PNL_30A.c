/*
       Panel controller v1.0
	   meteroi
*/

#include "global.h"
#include <C8051F120.h>  
#include "PNL_30A.h"
#include "delay.h"

volatile uint16_t PNL_Key=0;
volatile uint16_t PNL_Group=0;
volatile uint8_t PNL_ready=0;


uint16_t cmd_state = 0;
/*******************************************************************************
* Function Name      : PNL_30A_Configuration
* Description        : Configures the PNL_30A.
* Input              : None.
* Output             : None
* Return             : None
*******************************************************************************/
void PNL_30A_Configuration()
{
	PNL_30A_RST = 0x00;
	PNL_30A_RST = 0x03;

//	PNL_30A_Led_set(0x00);
//    PNL_30A_Buzzer_off();

	EA  =1;		   //global interrupt enable
	EX0 =1;		   //External interrupt 0 enable
	IT0 =1; 	   //


}

/*******************************************************************************
* Function Name      : PNL_30A_Led_set
* Description        : Set the led.
* Input              : led.  Only the 0 to 3 bit is used.  1 represents on and 0 represents off
* Output             : None
* Return             : None
*******************************************************************************/
void PNL_30A_Led_set(uint8_t led)
{
	uint16_t i=0;
	uint16_t t;
	PNL_30A_ADDR = PNL_30A_LED_BUZZER;
	t =  (PNL_30A_KEY0&0xfff0)| (~led&PNL_30A_LED_MASK);
	while(PNL_30A_KEY0!=t)
	{
    	PNL_30A_KEY0 = t; 
		PNL_30A_KEY1 = 0x0;
		PNL_30A_ADDR = PNL_30A_LED_BUZZER;
		i++;
		if(i>3000)break; 
	}
}

/*******************************************************************************
* Function Name      : PNL_30A_Buzzer_off
* Description        : Turn off the Buzzer
* Input              : None
* Output             : None
* Return             : None
*******************************************************************************/
void PNL_30A_Buzzer_off()
{
	uint16_t i=0;
	uint16_t t;
	PNL_30A_ADDR = PNL_30A_LED_BUZZER;
	t =  PNL_30A_KEY0 | PNL_30A_BUZZER_MASK;
	while(PNL_30A_KEY0!=t)
	{
    	PNL_30A_KEY0 = t; 
		PNL_30A_KEY1 = 0x0;
		PNL_30A_ADDR = PNL_30A_LED_BUZZER; 
		i++;
		if(i>3000)break; 
	}
}
/*******************************************************************************
* Function Name      : PNL_30A_Buzzer_on
* Description        : Turn on the Buzzer
* Input              : None
* Output             : None
* Return             : None
*******************************************************************************/
void PNL_30A_Buzzer_on()
{
	uint16_t i=0;
	uint16_t t;
	PNL_30A_ADDR = PNL_30A_LED_BUZZER;
	t =  PNL_30A_KEY0 & ~PNL_30A_BUZZER_MASK;
	while(PNL_30A_KEY0!=t)
	{
    	PNL_30A_KEY0 = t; 
		PNL_30A_KEY1 = 0x0;
		PNL_30A_ADDR = PNL_30A_LED_BUZZER; 
		i++;
		if(i>3000)break; 
	}
}


/*******************************************************************************
* Function Name      : PNL_30A_Key_return
* Description        : Return the key that was pressed not wait
* Input              : None
* Output             : Key value defien in the keymap.c 
* Return             : None
*******************************************************************************/
uint16_t PNL_30A_Key_return()
{
	if(PNL_ready==1)
	{
		PNL_ready = 0;
		return PNL_Key;
	}
	return 0;
}

/*******************************************************************************
* Function Name      : PNL_30A_Key_get
* Description        : Wait until the key was pressed and return the key
* Input              : None
* Output             : Key value defien in the keymap.c 
* Return             : None
*******************************************************************************/
uint16_t PNL_30A_Key_get()
{
	while(PNL_ready==0);
	PNL_ready = 0;
	return PNL_Key;
}



//todo
//void PNL_30A_LCD_LIGHT_on()
//{
//	PNL_30A_CMD = PNL_30A_CMD & ~PNL_30A_LCD_LIGHT_MASK;
//}
//void PNL_30A_LCD_LIGHT_off()
//{
//	PNL_30A_CMD = PNL_30A_CMD & ~PNL_30A_LCD_LIGHT_MASK;
//}

//void PNL_30A_LCD_RST_on()
//{
//	PNL_30A_CMD = PNL_30A_CMD & ~PNL_30A_LCD_RST_MASK;
//}

//void PNL_30A_LCD_RST_off()
//{
//	PNL_30A_CMD = PNL_30A_CMD | PNL_30A_LCD_RST_MASK;
//}

void EXTI0_IRQHandler(void)	 interrupt 0
{
	uint16_t Temp_Key;
	int i;
	//PNL_Group = PNL_30A_ADDR;
	Temp_Key   = PNL_30A_KEY1<<8;
	Temp_Key  += PNL_30A_KEY0;
	for(i=0;i<31;i++)
	{
		if(Temp_Key==(0xffff&key_map[i][0]))
		{			
			PNL_Key   = key_map[i][1];
			PNL_ready = 1;	
			break;
		}
	}
}
