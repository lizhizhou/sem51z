#include<math.h>
#include<stdio.h>
#include<string.h>
#include<userhead.h>

#define f0     XBYTE[0XD100]
#define f1     XBYTE[0XD101]
#define f2     XBYTE[0XD102]
#define f3     XBYTE[0XD103]
#define f4     XBYTE[0XD104]
#define f5     XBYTE[0XD105]
#define f6     XBYTE[0XD106]
#define f7     XBYTE[0XD107]
#define start XBYTE[0xC000]

#define speed0     XBYTE[0XD200]
#define speed1     XBYTE[0XD201]

char a_serial(char c)
{
  EA = 0;
  SBUF = c;
  while (!TI);
  TI = 0;
  EA = 1;
  return (c);
}
unsigned char serial_buffer;
void a_serial_i() interrupt 4
{
	EA=0;
	RI=0;
	serial_buffer = SBUF;
	EA=1;
}
void init_serial()
{
	 T2CON=0x30;   
	 T2MOD=0x00;     	
	 RCAP2H  = 0xff;		//	57600b 11.0592M
	 RCAP2L  = 0xfa;
//	 RCAP2H  = 0xff;		// 38400b  22.1184M
//   RCAP2L  = 0xee; 
     SCON    = 0x50;      //8bit
//	 SCON    = 0xC0;       //9bit
	 EA = 1;              /* Enable global interrupt flag       */
	 TR2= 1;
	 ES = 1;
	 PS = 1;
}
void speed_o(int speed,char n)
{
	a_serial(0xff);
	a_serial(n);
	a_serial(0x02);	
	a_serial(speed/100);
	a_serial(speed%100);
	a_serial(0xf0);
}

void main()
{
    long speed = 1000;
	int  inspeed = 0;
	long frequent;
    long frequent1;
    float result;
	float resulttemp;
	char flag=0;
    init_lcd();
    init_key();
	init_serial();
	speed0 = speed%256;
	speed1 = speed/256;
	while(1){
               printf("Frequent Test\n");
			   scanf("%d",&inspeed);
               msec(2000);
			   speed_o(inspeed,1);
               while(serial_buffer!=0xff&&returnkey()!=0x1B)
               {
               	    start=1;
               		while(P3_4==0); 
               		frequent = f0+ (long)f1*256 + (long)f2*256*256 + (long)f3*256*256*256;
               		frequent1 = f4+ f5*256 + (long)f6*256*256 + (long)f7*256*256*256;
               		result = frequent*40004600.0/frequent1-1;
			   		lcd_clr();
			   		resulttemp = result*20;
					printf("set speed\n %dRound/min\n",inspeed);
               		printf("speed now\n %dRound/min\n",(int)resulttemp);
			   		printf("speed diff\n %dRound/min\n",inspeed - (int)resulttemp);
					speed=speed-(resulttemp-inspeed)*0.5;
			   		speed0 = speed%256;
			   		speed1 = speed/256;
			   		speed_o(resulttemp,2);
	   			    if(inspeed<4000&&flag==0){
						flag=0;
					}
					else
					{
						flag=1;
					}
					if(flag==0)
						inspeed+=50;
					else
						inspeed-=50;
               }
			   serial_buffer=0xf0;
			   lcd_clr();
		}
}
