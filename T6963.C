/*
	T6963 driver
		   V1.0
	      meteroi
*/

#include "T6963.h"
#include "T6963_cmd.h"
#include<stdio.h>
#include<stdarg.h>
#include<absacc.h>
#include "delay.h"

/*  printf的缓存区长度  */
#define BUFFER_SIZE 50

#define	T6963_DATA	    	XBYTE[(T6963_BASE_ADDRESS+0)]	     	//CD=0;
#define	T6963_COMMAND 	    XBYTE[(T6963_BASE_ADDRESS+1)]		    //CD=1;


//Local function extern
extern void check_state_0_1(void);
extern void check_state_2(void);
extern void check_state_3(void);
extern void check_state_5(void);
extern void check_state_6(void);
extern void check_state_7(void);
extern void wr_com_2data(unsigned char cmd,unsigned char data1,unsigned char data2);
extern void wr_com_data(unsigned char cmd,unsigned char para);
extern void wr_com(unsigned char Cmd);
extern void wr_data(unsigned char ucData);
extern unsigned char rd_data(void);
extern void set_address(int address);

//Local value extern
static unsigned char show_charater_number;

//Low IO fuction for T6963
void check_state_0_1()                 //指令数据读、写状态
{
	while((T6963_COMMAND&0x03)!=0x03);
}

void check_state_2()                   //数据自动读状态
{
	while((T6963_COMMAND&0x04)!=0x04);
}

void check_state_3()                   //数据自动写状态
{
 	while((T6963_COMMAND&0x08)!=0x08);
}

void check_state_5()				   //控制器运行检测可能
{
	while((T6963_COMMAND&0x20)!=0x20);
}

void check_state_6()                   //屏读/屏拷贝状态
{
	while((T6963_COMMAND&0x40)!=0x00);
}

void check_state_7()				   //闪烁状态
{
	while((T6963_COMMAND&0x80)!=0x80);
}

void wr_com_2data(unsigned char cmd,unsigned char data1,unsigned char data2)
{                                      //写双字节参数指令
  	check_state_0_1();
  	T6963_DATA=data1;
  	check_state_0_1();
  	T6963_DATA=data2;
  	check_state_0_1();
  	T6963_COMMAND=cmd;
}

void wr_com_data(unsigned char cmd,unsigned char para)
{                                      //写单字节参数指令
  	check_state_0_1();
  	T6963_DATA=para;
  	check_state_0_1();
  	T6963_COMMAND=cmd;
}

void wr_com(unsigned char Cmd)  	  //写指令
{
  	check_state_0_1();
  	T6963_COMMAND=Cmd;
}

void wr_data(unsigned char ucData)    //写数据
{
	int i;
	for(i=0;i<100;i++);
  	check_state_0_1();
  	T6963_DATA=ucData;
}

unsigned char rd_data()               //读数据
{
  	check_state_0_1();
  	return T6963_DATA;
}

void set_address(int address)
{
	wr_com_2data(Set_Address_Pointer,address&0x00ff,address>>8);
}

//High level function for T6963
void T6963_clear_graph()             //清图形显示RAM
{
  	unsigned int i;
  	wr_com_2data(Set_Address_Pointer,0x00,0x03);// 置图形区首指针
  	wr_com(Set_Data_Auto_Write);           	    // 自动写
  	for(i=0;i<=22*64;i++)            // 对128×64点阵液晶清屏
  	{
   		wr_data(0x00);               // 写数据
  	}
  	wr_com(Auto_Reset);              // 自动写结束
}								

void T6963_lcd_clr()
{
    T6963_clear_text();
  	T6963_clear_graph();
}

void T6963_lcd_back()
{
	show_charater_number=0;
}

void T6963_init_lcd()                       // LCD 初始化
{
  	wr_com_2data(Set_Text_Home_Address,0x00,0x00);      // 文本显示区首地址
 	wr_com_2data(Set_Text_Area,0x15,0x00);      // 文本显示区宽度,21个字符
  	wr_com_2data(Set_Graphic_Home_address,0x00,0x03);      // 图形显示区首地址
  	wr_com_2data(Set_Graphic_Area,0x16,0x00);      // 图形显示区宽度

  	wr_com(Coursor_8_Line);                 // 光标形状,8列7行
  	wr_com(Mode_set|OR_mode);               // 显示方式设置,逻辑或合成
//  wr_com(Mode_set|EXOR_mode);				  // 显示方式设置,逻辑异或合成
//	wr_com(Mode_set|AND_mode);				  // 显示方式设置,逻辑与合成
//	wr_com(Mode_set|Test_Attribute_mode);				  // 显示方式设置,文本特征
  	wr_com(Display_Mode|Test_on_Graphic_on);                 // 显示开关设置,文本显示启用，图形显示启用，光标显示禁止，光标闪烁禁止
    T6963_clear_text();
  	T6963_clear_graph();
	T6963_clear_graph();
}
void T6963_draw_point(char x,char y)
{
    char command;
    command=Bit_Set|((5-(x%6)));
	set_address(0x0300+x/6+y*22);
	wr_com(command);
}

void T6963_erase_point(char x,char y)
{
    char command;
    command=Bit_Reset|(5-(x%6));
	set_address(0x0300+x/6+y*22);
	wr_com(command);
}
void T6963_printc(unsigned char *table,unsigned char X,unsigned char Y,unsigned char size)
{
  	unsigned char i,count,length;
  	unsigned int address;
  	length=(size*size)>>3;
  	count=size>>3;                             //Size/8得出一个汉字一行的字节数
  	set_address(address=0x0300+Y*count*128+X*count);                    //置地址指针
  	wr_com(Set_Data_Auto_Write);                 //自动写
  	for(i=0;i<length;i++)
  	{
     	    if(i>1&&(i%count==0))                   //显示完count个字节即Size个象素点后换行
     	    {
       		wr_com(Auto_Reset);            //关自动写
       		set_address(address+=16);                                   //置地址指针
       		wr_com(Set_Data_Auto_Write);            //自动写
      	    }
     	    wr_data(table[i]);
   	}
  	wr_com(Auto_Reset);                 //关自动写
}

#if FS==1
void T6963_clear_text()             //清文本显示RAM
{
  	unsigned char i;
  	wr_com_2data(Set_Address_Pointer,0x00,0x00);// 置文本区首指针
  	wr_com(Set_Data_Auto_Write);           // 自动写
  	for(i=0;i<=22*8;i++)                 // 对128×64点阵液晶清屏
  	{
  		wr_data(0x00);              // 写数据
  	}
  	wr_com(Auto_Reset);           // 自动写结束
    show_charater_number=0;
}
char T6963_putchar(char character)
{
        if(character=='\n')
              show_charater_number=21-show_charater_number%21+show_charater_number;
        else if(character=='\r');

        else if(character=='\t')
              show_charater_number=show_charater_number+10;

        else if(character=='\b')
        {
              set_address(0x0000+(--show_charater_number));
              wr_com_data(Data_Write_and_Non_variable_ADP,' '-0x20);
        }
        else{
              set_address(0x0000+show_charater_number++);
              wr_com_data(Data_Write_and_Increment_ADP,character-0x20);              //显示字符，地址加1
        }
	return character;
        
}
#else
void T6963_clear_text()             //清文本显示RAM
{
  	unsigned char i;
  	wr_com_2data(Set_Address_Pointer,0x00,0x00);// 置文本区首指针
  	wr_com(Set_Data_Auto_Write);           // 自动写
  	for(i=0;i<=16*8;i++)                 // 对128×64点阵液晶清屏
  	{
  		wr_data(0x00);              // 写数据
  	}
  	wr_com(Auto_Reset);           // 自动写结束
    show_charater_number=0;
}

char T6963_putchar(char character)
{
        if(character=='\n')
              show_charater_number=16-show_charater_number%16+show_charater_number;
        else if(character=='\r');

        else if(character=='\t')
              show_charater_number=show_charater_number+8;

        else if(character=='\b')
        {
              set_address(0x0000+(--show_charater_number));
              wr_com_data(Data_Write_and_Non_variable_ADP,' '-0x20);
        }
        else{
              set_address(0x0000+show_charater_number++);
              wr_com_data(Data_Write_and_Increment_ADP,character-0x20);              //显示字符，地址加1
        }
	return character;
}
#endif

int T6963_puts(const char* s)
{
	int i=0;
	for(i=0;(*s)!='\0';s++,i++)
		T6963_putchar(*s);
	T6963_putchar('\n');
	return i;	
}	

int T6963_printf(const char *format,...)
{
	xdata char obuffer[BUFFER_SIZE];
	int         i=0,chars;
	va_list		ap;
	va_start( ap, format );
	chars = vsprintf(obuffer,format,ap);
	while(i<chars)
		T6963_putchar((int)obuffer[i++]);
	va_end( ap );
	return chars;
}
