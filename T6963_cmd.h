//registers setting
#define Set_Corsor_Pointer                 	  0x21
#define Set_Offset_Register					  0x22
#define Set_Address_Pointer	                  0x24
//set control word
#define Set_Text_Home_Address				  0x40
#define Set_Text_Area						  0x41
#define Set_Graphic_Home_address			  0x42
#define Set_Graphic_Area					  0x43
//mode set
#define Mode_set							  0x80
#define OR_mode								  0x00
#define EXOR_mode							  0x01
#define AND_mode							  0x03
#define Test_Attribute_mode					  0x04
#define Internal_CG_ROM_mode				  0x00
#define External_CG_RAM_mode				  0x08
//display mode
#define Display_Mode					      0x90
#define Display_off							  0x00
#define Cursor_on_Blink_off					  0x02
#define Cursor_on_Blink_on					  0x03
#define Text_on_Graphic_off					  0x04
#define Text_off_Graphic_on					  0x08
#define Test_on_Graphic_on					  0x0c
//cursor pattern select
#define Coursor_1_Line						  0xa0
#define Coursor_2_Line						  0xa1
#define Coursor_3_Line						  0xa2
#define Coursor_4_Line						  0xa3
#define Coursor_5_Line						  0xa4
#define Coursor_6_Line						  0xa5
#define Coursor_7_Line						  0xa6
#define Coursor_8_Line						  0xa7
//data auto read/write
#define Set_Data_Auto_Write					  0xb0
#define Set_Data_Auto_Read					  0xb1
#define Auto_Reset							  0xb2
#define Data_Write_and_Increment_ADP		  0xc0
#define Data_Read_and_Increment_ADP			  0xc1
#define Data_Write_and_Decrement_ADP		  0xc2
#define Data_Read_and_Decrement_ADP			  0xc3
#define Data_Write_and_Non_variable_ADP		  0xc4
#define Data_Read_and_Non_variable_ADP		  0xc5
//screen peek
#define Screen_Peek							  0xe0
//screen copy
#define Screen_Copy							  0xe8
//bit set/reset
#define Bit_Reset							  0xf0
#define Bit_Set								  0xf8
#define Bit0								  0x00
#define Bit1								  0x01
#define Bit2								  0x02
#define Bit3								  0x03
#define Bit4								  0x04
#define Bit5								  0x05
#define Bit6								  0x06
#define Bit7								  0x07