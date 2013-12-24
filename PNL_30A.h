#ifndef __PNL_30A_H__
#define __PNL_30A_H__
#include "global.h"
#include "absacc.h"

#define PNL_30A_BASE_ADDRESS   0xe010
#define RST_BASE_ADDRESS       0xe001
#define	PNL_30A_ADDR        XBYTE[PNL_30A_BASE_ADDRESS] 
#define	PNL_30A_KEY0	    XBYTE[PNL_30A_BASE_ADDRESS+1]
#define	PNL_30A_KEY1		XBYTE[PNL_30A_BASE_ADDRESS+2]

#define PNL_30A_SOFTKEY    0x01
#define PNL_30A_ENCODER	   0x02	   //
#define PNL_30A_LED_BUZZER 0x03

//CMD register definition
//Bit0-3 LED	Bit4   BUZZER   Bit5   LCD_LIGHT   Bit6   LCD_RST   Bit7   LCD_FS
#define PNL_30A_LED_MASK        0x0f
#define PNL_30A_BUZZER_MASK     0x10
#define PNL_30A_LCD_LIGHT_MASK	0x20
#define PNL_30A_LCD_RST_MASK    0x40
#define PNL_30A_LCD_FS_MASK		0x80

#define PNL_30A_RST     	XBYTE[(RST_BASE_ADDRESS)]
extern code short key_map[31][2];
extern uint16_t cmd_state;

void PNL_30A_Configuration(void);
void PNL_30A_Led_set(uint8_t led);
void PNL_30A_Buzzer_on (void);
void PNL_30A_Buzzer_off(void);
uint16_t PNL_30A_Key_return(void);
uint16_t PNL_30A_Key_get(void);

#endif
