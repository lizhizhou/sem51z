#include <absacc.h>
#include "math.h"
#include "delay.h"
#define SPEED_BASE 0xe080
#define reset  XBYTE[SPEED_BASE]
#define ready  XBYTE[SPEED_BASE+1]
#define f0     XBYTE[SPEED_BASE+2]
#define f1     XBYTE[SPEED_BASE+3]
#define f2     XBYTE[SPEED_BASE+4]
#define f3     XBYTE[SPEED_BASE+5]
#define f4     XBYTE[SPEED_BASE+6]
#define f5     XBYTE[SPEED_BASE+7]
#define f6     XBYTE[SPEED_BASE+8]
#define f7     XBYTE[SPEED_BASE+9]

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
unsigned long get_speed()
{
	long frequent, frequent1;
	unsigned long i=0;
	float result;
	reset = 1;
	reset = 0;
	while(!ready);
//	{
//		i++;
//		delay_ms(100);
//		if(i>0xf)
//			return 0;
//	}
	frequent = f0+ (long)f1*256 + (long)f2*256*256 + (long)f3*256*256*256;
	frequent1 = f4+ f5*256 + (long)f6*256*256 + (long)f7*256*256*256;
	result = frequent*48000000.0/frequent1-1;
	return (unsigned long)result /4 * 60;
}

#define PWM_MINI 30
void set_pwm(unsigned long pwm)
{
	if(pwm < 0xFFFFFFFF / 100 * PWM_MINI)
		pwm = 0xFFFFFFFF / 100 * PWM_MINI;
	PWM_width0 = pwm%256;
	PWM_width1 = (pwm/256)%256;
	PWM_width2 = (pwm/256/256)%256;
	PWM_width3 = (pwm/256/256/256)%256;
}

unsigned long get_pwm()
{
	unsigned long pwm;
	pwm = PWM_width0;
	pwm += PWM_width1 * 256;
	pwm += (unsigned long)PWM_width2 * 256 * 256;
	pwm += (unsigned long)PWM_width3 * 256 * 256 * 256;
	return pwm;
}

#define MAX_SPEED 3200
#define MAX_DELTA 1000
#define P 100000
void set_speed(unsigned long speed)
{
	long delta;
	unsigned long pwm;
	unsigned long current_pwm;
	set_pwm(0xffffffff/MAX_SPEED * speed);
	delay_ms(100);
	do 
	{
		delta = speed - get_speed();
		current_pwm = get_pwm();
		if (delta > MAX_DELTA)
			delta = MAX_DELTA;
		else if (delta < -MAX_DELTA)
			delta = -MAX_DELTA;
		if( delta > 0 && (0xfffffffff - delta * P) < current_pwm)
			 pwm = 0xfffffffff;
		else
			pwm = current_pwm + delta * P;
		set_pwm(pwm);
	} while ( abs(delta) > 100);
} 

void brake()
{
	BRAKE = 1;
}

void start()
{
	BRAKE = 0;
}

void brushless_motor_init()
{
	long frequent  = 10000.0 * 0xffffffff / 48000000.0;
 	PWM_frequent0 = frequent%256;
	PWM_frequent1 = (frequent/256)%256;
	PWM_frequent2 = (frequent/256/256)%256;
	PWM_frequent3 = (frequent/256/256/256)%256;
	FB = 0;
	BRAKE = 0;
	set_pwm(0xFFFFFFFF / 100 * 30);
	brake();
}