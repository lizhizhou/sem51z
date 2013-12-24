#include <C8051F120.h>                 // SFR declarations
#include "global.h"

//-----------------------------------------------------------------------------
// Macros used to calculate Loop Filter Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 14.8. PLL0FLT: PLL Filter
//
// Note that PLL Loop filter bits give a suitable frequency range for each 
// setting. In some cases more than one setting may be acceptable for a 
// particular frequency. This macro will select the best fit setting for
// a particular frequency. 
//
//-----------------------------------------------------------------------------
#if (PLL_DIV_CLK)<(5000000L)
   #error "error: PLL divided clock frequency Too Low!"
#elif (PLL_DIV_CLK)<(8000000L)
   #define PLLFLT_LOOP 0x0F
#elif (PLL_DIV_CLK)<(12500000L)
   #define PLLFLT_LOOP 0x07
#elif (PLL_DIV_CLK)<(19500000L)
   #define PLLFLT_LOOP 0x03
#elif (PLL_DIV_CLK)<(30000001L)
   #define PLLFLT_LOOP 0x01
#else
   #error "error: PLL divided clock frequency Too High!"
#endif

//-----------------------------------------------------------------------------
// Macros used to calculate ICO  Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 14.8. PLL0FLT: PLL Filter
//
// Note that PLL ICO bits give a suitable frequency range for each setting. 
// In some cases more than one setting may be acceptable for a particular 
// frequency. This macro will select the best fit setting for a particular
// frequency. 
//
//-----------------------------------------------------------------------------

#if (PLL_OUT_CLK)<(25000000L)
   #error "error: PLL output frequency Too Low!"
#elif (PLL_OUT_CLK)<(42500000L)
   #define PLLFLT_ICO 0x30
#elif (PLL_OUT_CLK)<(52500000L)
   #define PLLFLT_ICO 0x20
#elif (PLL_OUT_CLK)<(72500000L)
   #define PLLFLT_ICO 0x10
#elif (PLL_OUT_CLK)<(100000001L)
   #define PLLFLT_ICO 0x00
#else
   #error "error: PLL output frequency Too High"
#endif


//-----------------------------------------------------------------------------
// Macros used to calculate  Flash Read Time  Bits
//
// Reference
// C8051F121x-13x.pdf 
// SFR Definition 15.2. FLSCL: Flash Memory Control
//-----------------------------------------------------------------------------

#if (SYSCLK)<(25000001L)
   #define FLSCL_FLRT 0x00
#elif (SYSCLK)<(50000001L)
   #define FLSCL_FLRT 0x10
#elif (SYSCLK)<(50000001L)
   #define FLSCL_FLRT 0x20
#elif (SYSCLK)<(100000001L)
   #define FLSCL_FLRT 0x30
#else
   #error "error: SYSCLK Too High"
#endif



//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the Crossbar and GPIO ports.
//
//    P0.0 - UART0 TX (push-pull)
//    P0.1 - UART0 RX
//
//    P4.4 - RAM_CS (RAM Chip Select) (push-pull)
//    P4.5 - RAM_BANK (RAM Bank Select) (push-pull)
//    P4.6 - /RD (EMIF) (push-pull)
//    P4.7 - /WR (EMIF) (push-pull)
//
//    P5, P6, P7 used for EMIF (push-pull)
//
//    all other port pins unused
//
//-----------------------------------------------------------------------------
void PORT_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = CONFIG_PAGE;              // Switch to the necessary SFRPAGE

   XBR0    = 0xF7; 
   XBR1    = 0xFF;                    
  // XBR2    = 0x5C;  
   XBR2    = 0x7D;  
                      
                                       // Enable crossbar and enable
                                       // weak pull-ups

   //P1MDIN  = 0xFF;

   P0MDOUT |= 0x01;                    // Enable UTX as push-pull output
   P3MDOUT |= 0x08;                    // Enable CLKOUT as push-pull output

   // EMIF Initializations
   P4MDOUT |= 0xc0;//0xF0;                    // /WR, /RD, RAM_CS, and RAM_BANK are
   P1MDOUT |= 0x00;
   P2MDOUT |= 0x00;	
                                       // push-pull
   P5MDOUT |= 0xFF;
   P6MDOUT |= 0xFF;
   P7MDOUT |= 0xFF;


   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}

//-----------------------------------------------------------------------------
// ExtCrystalOsc_Init ()
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// This routine initializes for and switches to the External Crystal
// Oscillator.
//
//-----------------------------------------------------------------------------
void PLL_Init (void)
{
   unsigned char n;                    // n used for short delay counter

   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   OSCXCN = 0x20;                      // External Oscillator is a CMOS clock
                                       // (no divide by 2 stage)
                                       // XFCN bit settings do not apply to a
                                       // CMOS clock

   PLL0CN  = 0x04;                     // Set external clock as PLL source

   SFRPAGE = LEGACY_PAGE;
   FLSCL   = FLSCL_FLRT;               // Set FLASH read time according to SYSCLK
   SFRPAGE = CONFIG_PAGE;

   PLL0CN |= 0x01;                     // Enable Power to PLL
   PLL0DIV = PLL_DIV;                  // Set PLL divider value using macro
   PLL0MUL = PLL_MUL;                  // Set PLL multiplier value using macro
   PLL0FLT = PLLFLT_ICO|PLLFLT_LOOP;   // Set the PLL filter loop and ICO bits

   for (n=0xFF;n!=0;n--);              // Wait at least 5us
   PLL0CN  |= 0x02;                    // Enable the PLL
   while(!(PLL0CN & 0x10));            // Wait until PLL frequency is locked

   CLKSEL  = 0x12;                     // Select PLL as SYSCLK source

}



//-----------------------------------------------------------------------------
// EMIF_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure the external memory interface to non-multiplexed split-mode
// with bank select.
//
//-----------------------------------------------------------------------------
void EMIF_Init (void)
{
   unsigned char SFRPAGE_save = SFRPAGE; // Save the current SFRPAGE

   SFRPAGE = EMI0_PAGE;                // Switch to the necessary SFRPAGE

   EMI0CF = 0x3C; //0x38                      // Non-muxed mode; split mode with bank
                                       // select
   EMI0CN = 0xFF;  //0x40                    // Access addresses on page 0x4000 for
                                       // 8-bit XRAM moves
   EMI0TC = 0xFF;

   SFRPAGE = SFRPAGE_save;             // Restore the SFRPAGE
}



