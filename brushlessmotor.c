#include <absacc.h>
#define SPEED_BASE 0xe080
#define f0     XBYTE[SPEED_BASE]
#define f1     XBYTE[SPEED_BASE+1]
#define f2     XBYTE[SPEED_BASE+2]
#define f3     XBYTE[SPEED_BASE+3]
#define f4     XBYTE[SPEED_BASE+4]
#define f5     XBYTE[SPEED_BASE+5]
#define f6     XBYTE[SPEED_BASE+6]
#define f7     XBYTE[SPEED_BASE+7]
#define start  XBYTE[SPEED_BASE+8]

#define MOTOER_BASE 0xe060
#define FB			   XBYTE[MOTOER_BASE]
#define BRAKE		   XBYTE[MOTOER_BASE+1]
#define PWM_width0     XBYTE[MOTOER_BASE+2]
#define PWM_width1     XBYTE[MOTOER_BASE+3]
#define PWM_width2     XBYTE[MOTOER_BASE+4]
#define PWM_width3     XBYTE[MOTOER_BASE+5]
#define PWM_frequent0  XBYTE[MOTOER_BASE+6]
#define PWM_frequent1  XBYTE[MOTOER_BASE+7]
#define PWM_frequent2  XBYTE[MOTOER_BASE+8]
#define PWM_frequent3  XBYTE[MOTOER_BASE+9]
void brushless_motor_init()
{
	int frequent  = 5000;
 	PWM_frequent0 = frequent%256;
	PWM_frequent1 = (frequent/256)%256;
	PWM_frequent2 = (frequent/256/256)%256;
	PWM_frequent3 = (frequent/256/256/256)%256;
	FB = 0;
	BRAKE = 0;
}
unsigned long get_speed()
{
	long frequent, frequent1;
	float result;
	start = 1;
	frequent = f0+ (long)f1*256 + (long)f2*256*256 + (long)f3*256*256*256;
	frequent1 = f4+ f5*256 + (long)f6*256*256 + (long)f7*256*256*256;
	result = frequent*40004600.0/frequent1-1;
	return (unsigned long)result /4 * 60;
}

void set_speed(unsigned long speed)
{
	PWM_width0 = speed%256;
	PWM_width1 = (speed/256)%256;
	PWM_width2 = (speed/256/256)%256;
	PWM_width3 = (speed/256/256/256)%256;
} 

void brake()
{
	BRAKE = 1;
}