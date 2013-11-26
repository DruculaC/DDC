/*------------------------------------------------------------------
	main.h (v1.00)
	
	'project header' (see chap 5)
-------------------------------------------------------------------*/

#ifndef _MAIN_H
#define _MAIN_H

//--------header file------------------------------------------------

// must include the appropriate microcontroller header file here
#include "N79E81x.h"
#include <intrins.h>

// oscillator / resonator frequency (in Hz)
#define OSC_FREQ (11059200UL)

// number of oscillations per instruction (12, etc)
// 12 - original 8051/8052 and numerous modern versions
// 6 - various Infineon and Philips devices, etc.
// 4 - Dallas 320, 520 etc.
// 2 - Dallas 420, etc
#define OSC_PER_INST (12)

//-------------------------------------------------------------------
// should not need to edit the sections below
//-------------------------------------------------------------------

// typedefs
typedef unsigned char tByte;
typedef unsigned int  tWord;
typedef unsigned long tLong;

// Interrupts
#define interrupt_timer_0_overflow 1
#define interrupt_timer_1_overflow 3
#define interrupt_timer_2_overflow 5

#endif

/*---------------------------------------------------
	end of file
----------------------------------------------------*/