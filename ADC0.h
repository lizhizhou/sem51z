#ifndef __ADC0_H__
#define __ADC0_H__

#define	ADC_CH0 5
#define	ADC_CH1	4
#define	ADC_CH2 3
#define	ADC_CH3	2

extern void ADC0_Init (void);
extern void ADC0_start(char ch);
extern int ADC0_get(void);

#endif