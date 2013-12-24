#ifndef __GLOBAL_H__
#define __GLOBAL_H__

//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
    /* exact-width signed integer types */
typedef   signed           char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;

    /* exact-width unsigned integer types */
typedef unsigned           char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;

#define CLKIN      	 (12000000L)       // Sets Input Clock Frequency

#define PLL_MUL      (8)               // Sets PLL multiplier N
#define PLL_DIV      (1)               // Sets PLL divider M

#define PLL_DIV_CLK  (CLKIN/PLL_DIV)  // PLL divided clock input frequency
#define PLL_OUT_CLK  (PLL_DIV_CLK*PLL_MUL)
                                       // PLL output frequency
#define SYSCLK       PLL_OUT_CLK       // System clock derived from PLL

#define RAM_SIZE     32768

#define SAMPLE_RATE_DAC 100000L        // DAC sampling rate in Hz
#endif
