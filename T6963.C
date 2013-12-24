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

/*  printf�Ļ���������  */
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
void check_state_0_1()                 //ָ�����ݶ���д״̬
{
	while((T6963_COMMAND&0x03)!=0x03);
}

void check_state_2()                   //�����Զ���״̬
{
	while((T6963_COMMAND&0x04)!=0x04);
}

void check_state_3()                   //�����Զ�д״̬
{
 	while((T6963_COMMAND&0x08)!=0x08);
}

void check_state_5()				   //���������м�����
{
	while((T6963_COMMAND&0x20)!=0x20);
}

void check_state_6()                   //����/������״̬
{
	while((T6963_COMMAND&0x40)!=0x00);
}

void check_state_7()				   //��˸״̬
{
	while((T6963_COMMAND&0x80)!=0x80);
}

void wr_com_2data(unsigned char cmd,unsigned char data1,unsigned char data2)
{                                      //д˫�ֽڲ���ָ��
  	check_state_0_1();
  	T6963_DATA=data1;
  	check_state_0_1();
  	T6963_DATA=data2;
  	check_state_0_1();
  	T6963_COMMAND=cmd;
}

void wr_com_data(unsigned char cmd,unsigned char para)
{                                      //д���ֽڲ���ָ��
  	check_state_0_1();
  	T6963_DATA=para;
  	check_state_0_1();
  	T6963_COMMAND=cmd;
}

void wr_com(unsigned char Cmd)  	  //дָ��
{
  	check_state_0_1();
  	T6963_COMMAND=Cmd;
}

void wr_data(unsigned char ucData)    //д����
{
	int i;
	for(i=0;i<100;i++);
  	check_state_0_1();
  	T6963_DATA=ucData;
}

unsigned char rd_data()               //������
{
  	check_state_0_1();
  	return T6963_DATA;
}

void set_address(int address)
{
	wr_com_2data(Set_Address_Pointer,address&0x00ff,address>>8);
}

//High level function for T6963
void T6963_clear_graph()             //��ͼ����ʾRAM
{
  	unsigned int i;
  	wr_com_2data(Set_Address_Pointer,0x00,0x03);// ��ͼ������ָ��
  	wr_com(Set_Data_Auto_Write);           	    // �Զ�д
  	for(i=0;i<=22*64;i++)            // ��128��64����Һ������
  	{
   		wr_data(0x00);               // д����
  	}
  	wr_com(Auto_Reset);              // �Զ�д����
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

void T6963_init_lcd()                       // LCD ��ʼ��
{
  	wr_com_2data(Set_Text_Home_Address,0x00,0x00);      // �ı���ʾ���׵�ַ
 	wr_com_2data(Set_Text_Area,0x15,0x00);      // �ı���ʾ������,21���ַ�
  	wr_com_2data(Set_Graphic_Home_address,0x00,0x03);      // ͼ����ʾ���׵�ַ
  	wr_com_2data(Set_Graphic_Area,0x16,0x00);      // ͼ����ʾ������

  	wr_com(Coursor_8_Line);                 // �����״,8��7��
  	wr_com(Mode_set|OR_mode);               // ��ʾ��ʽ����,�߼���ϳ�
//  wr_com(Mode_set|EXOR_mode);				  // ��ʾ��ʽ����,�߼����ϳ�
//	wr_com(Mode_set|AND_mode);				  // ��ʾ��ʽ����,�߼���ϳ�
//	wr_com(Mode_set|Test_Attribute_mode);				  // ��ʾ��ʽ����,�ı�����
  	wr_com(Display_Mode|Test_on_Graphic_on);                 // ��ʾ��������,�ı���ʾ���ã�ͼ����ʾ���ã������ʾ��ֹ�������˸��ֹ
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
  	count=size>>3;                             //Size/8�ó�һ������һ�е��ֽ���
  	set_address(address=0x0300+Y*count*128+X*count);                    //�õ�ַָ��
  	wr_com(Set_Data_Auto_Write);                 //�Զ�д
  	for(i=0;i<length;i++)
  	{
     	    if(i>1&&(i%count==0))                   //��ʾ��count���ֽڼ�Size�����ص����
     	    {
       		wr_com(Auto_Reset);            //���Զ�д
       		set_address(address+=16);                                   //�õ�ַָ��
       		wr_com(Set_Data_Auto_Write);            //�Զ�д
      	    }
     	    wr_data(table[i]);
   	}
  	wr_com(Auto_Reset);                 //���Զ�д
}

#if FS==1
void T6963_clear_text()             //���ı���ʾRAM
{
  	unsigned char i;
  	wr_com_2data(Set_Address_Pointer,0x00,0x00);// ���ı�����ָ��
  	wr_com(Set_Data_Auto_Write);           // �Զ�д
  	for(i=0;i<=22*8;i++)                 // ��128��64����Һ������
  	{
  		wr_data(0x00);              // д����
  	}
  	wr_com(Auto_Reset);           // �Զ�д����
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
              wr_com_data(Data_Write_and_Increment_ADP,character-0x20);              //��ʾ�ַ�����ַ��1
        }
	return character;
        
}
#else
void T6963_clear_text()             //���ı���ʾRAM
{
  	unsigned char i;
  	wr_com_2data(Set_Address_Pointer,0x00,0x00);// ���ı�����ָ��
  	wr_com(Set_Data_Auto_Write);           // �Զ�д
  	for(i=0;i<=16*8;i++)                 // ��128��64����Һ������
  	{
  		wr_data(0x00);              // д����
  	}
  	wr_com(Auto_Reset);           // �Զ�д����
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
              wr_com_data(Data_Write_and_Increment_ADP,character-0x20);              //��ʾ�ַ�����ַ��1
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