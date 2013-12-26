#include <absacc.h>
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

float get_speed()
{
	long frequent, frequent1;
	float result;
	//while(P3_4==0); 
	frequent = f0+ (long)f1*256 + (long)f2*256*256 + (long)f3*256*256*256;
	frequent1 = f4+ f5*256 + (long)f6*256*256 + (long)f7*256*256*256;
	result = frequent*40004600.0/frequent1-1;
	return result;
}

void set_speed(unsigned int speed)
{
	speed0 = speed%256;
	speed1 = speed/256;
} 