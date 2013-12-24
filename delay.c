#include <c8051f120.h>                 // SFR declarations
#include "global.h"                 

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------
sfr16 RCAP3    = 0xca;                 // Timer3 capture/reload
sfr16 TMR3     = 0xcc;                 // Timer3
//-----------------------------------------------------------------------------
// delay_ms
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters:
//   1) unsigned int ms - number of milliseconds of delay
//                        range is full range of integer: 0 to 65335
//
// This routine inserts a delay of <ms> milliseconds.
//
//-----------------------------------------------------------------------------

void delay_ms(unsigned int ms)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = TMR3_PAGE;

   TMR3CN = 0x00;                      // Stop Timer3; Clear TF3;
   TMR3CF = 0x00;                      // use SYSCLK/12 as timebase

   RCAP3 = -(SYSCLK/1000/12);          // Timer 3 overflows at 1 kHz
   TMR3 = RCAP3;

   TR3 = 1;                            // Start Timer 3

   while(ms)
   {
      TF3 = 0;                         // Clear flag to initialize
      while(!TF3);                     // Wait until timer overflows
      ms--;                            // Decrement ms
   }

   TR3 = 0;                            // Stop Timer 3

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}
