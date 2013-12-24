#ifndef __DAC_H__
#define __DAC_H__

extern void DAC0_Init(void);
extern void DAC1_Init(void);
extern void TIMER4_Init (int counts);
extern void Set_DAC_Frequent(unsigned int f);

#endif