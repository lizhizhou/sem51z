#include <c8051f120.h>                 // SFR declarations
#include "global.h"      
#include "delay.h"           

//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F12x
//-----------------------------------------------------------------------------
sfr16 ADC0     = 0xbe;                 // ADC0 data

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
#define SAR_CLK      2500000           // Desired SAR clock speed

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------  
void ADC0_ISR (void);

//-----------------------------------------------------------------------------
// Private Variables
//-----------------------------------------------------------------------------

volatile int ADC0_data;
volatile bit ADC0_ready=0;

//-----------------------------------------------------------------------------
// Initialization Subroutines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// ADC0_Init
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   : None
//
// Configure ADC0 to use Timer3 overflows as conversion source, to
// generate an interrupt on conversion complete, and to use right-justified
// output mode.  Enables ADC end of conversion interrupt. Leaves ADC disabled.
//
//-----------------------------------------------------------------------------
void ADC0_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page

   SFRPAGE = ADC0_PAGE;

   ADC0CN = 0x00;                      // ADC0 disabled; normal tracking
                                       // mode; ADC0 conversions are initiated
                                       // on AD0BUSY set; ADC0 data is
                                       // right-justified, low power tracking 
                                       // mode

   REF0CN = 0x03;                      // Enable on-chip VREF and output buffer

   AMX0CF = 0x00;                      // AIN inputs are single-ended (default)

   AMX0SL = 0x00;                      // Select AIN0.0 pin as ADC mux input 
                                       // ISR will change this to step through 
                                       // inputs

   ADC0CF = (SYSCLK/SAR_CLK) << 3;     // ADC conversion clock = 2.5MHz
   ADC0CF |= 0x00;                     // PGA gain = 1 (default)

   EIE2 |= 0x02;                       // enable ADC interrupts
       
   AD0EN = 1;                          // Enable ADC

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page


}

void ADC0_start(char ch)
{
    char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
	unsigned int i;			   
    SFRPAGE = ADC0_PAGE;
	AMX0SL = ch;
	ADC0 =0x0;
	for(i=0;i<100;i++)                  // Waiting
	AD0BUSY =1;
	SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

unsigned int ADC0_get()
{
    char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page	
	while(ADC0_ready==0);
	ADC0_ready=0;                 
	SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
	return ADC0_data&0xfff;	
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
void ADC0_ISR (void) interrupt 15
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
   
   SFRPAGE = ADC0_PAGE;
   ADC0_data = ADC0;  // Read ADC value and add to running
   ADC0_ready = 1;
   AD0INT = 0;                         //clear ADC conversion complete overflow 
   ADC0 =0;
   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------