#include <C8051F120.h>                 // SFR declarations
#include <stdio.h>
#include "global.h"

#define BAUDRATE     115200            // Baud rate of UART in bps

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------

sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 TMR2     = 0xcc;                 // Timer2
//-----------------------------------------------------------------------------
// UART0_Init   Variable baud rate, Timer 2, 8-N-1
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure UART0 for operation at <baudrate> 8-N-1 using Timer2 as
// baud rate source.
//
//-----------------------------------------------------------------------------
void UART0_Init (void)
{
   char SFRPAGE_SAVE;

   SFRPAGE_SAVE = SFRPAGE;             // Preserve SFRPAGE

   SFRPAGE = TMR2_PAGE;

   TMR2CN = 0x00;                      // Timer in 16-bit auto-reload up timer
                                       // mode
   TMR2CF = 0x08;                      // SYSCLK is time base; no output;
                                       // up count only
   RCAP2 = - ((long) SYSCLK/BAUDRATE/16);
   TMR2 = RCAP2;
   TR2= 1;                             // Start Timer2

   SFRPAGE = UART0_PAGE;

   SCON0 = 0x50;                       // 8-bit variable baud rate;
                                       // 9th bit ignored; RX enabled
                                       // clear all flags
   SSTA0 = 0x15;                       // Clear all flags; enable baud rate
                                       // doubler (not relevant for these
                                       // timers);
                                       // Use Timer2 as RX and TX baud rate
                                       // source;
   TI0     = 1;                        // Indicate TX0 ready

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFRPAGE
}

//-----------------------------------------------------------------------------
// putchar
//-----------------------------------------------------------------------------
//
/*  Copyright KEIL ELEKTRONIK GmbH 1990 - 2002                         */
/*
 * putchar (basic version): expands '\n' into CR LF
 */
// Modified by BW
//
// This routine overloads the standard putchar() library function to support
// either UART0 or UART1, depending on the state of the global variable
// <Uart>.
//
char putchar (char c)  
{
   char SFRPAGE_SAVE = SFRPAGE;

   SFRPAGE = UART0_PAGE;
   
   if (c == '\n')                      // If carriage return
   {
      while (!TI0);
      TI0 = 0;
      SBUF0 = 0x0d;                    // Output CR 
   }

   while (!TI0);                       // Wait for transmit complete
   TI0 = 0;

   SBUF0 = c;                          // Send character

   SFRPAGE = SFRPAGE_SAVE;

   return c;
}