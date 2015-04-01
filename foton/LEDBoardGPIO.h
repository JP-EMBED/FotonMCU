/************************************************************************
* file: LEDBOARDGPIO.h
* author: Kat Derby
* Revision: 3.8
* Last Revision Date: 2/18/2015
*
* Defined Values: 
*		- Defined Values that make using macros and the GPIO for the
*			LED board signals easier
* 
*************************************************************************/

#ifndef LEDBOARDGPIO_H_
#define LEDBOARDGPIO_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <hw_types.h>
#include <hw_memmap.h>
#include <hw_gpio.h>
#include <pin.h>
#include <gpio.h>
#include <prcm.h>
#include "fotonrgb.h"

// Image info
#define IMAGE_SIZE 1024

// bit packed pin representation for each signal
// Control Signals
#define LATCH_PIN 0x01
#define BLANK_PIN 0x80
#define CLK_PIN 0x02

extern unsigned char BLANK_PIN_NUM;
extern unsigned char BLANK_PIN_ADDR;
extern unsigned long BLANK_PORT_ADDR;

// Color Coding Signals
// Top Rows
#define R0_PIN 0x40
#define G0_PIN 0x10
#define B0_PIN 0x20

// Bottom Rows
#define R1_PIN 0x20
#define G1_PIN 0x40
#define B1_PIN 0x80

// Address Signals
#define D_PIN 0x10
#define A_PIN 0x80
#define B_PIN 0x40
#define C_PIN 0x40


// Ports for each signal
// Control Signals
#define LATCH_PORT GPIOA0_BASE
#define BLANK_PORT GPIOA3_BASE
#define CLK_PORT GPIOA2_BASE

// Color Coding Signals
// Top Rows
#define R0_PORT GPIOA1_BASE
#define G0_PORT GPIOA1_BASE
#define B0_PORT GPIOA1_BASE

// Bottom Rows
#define R1_PORT GPIOA0_BASE
#define G1_PORT GPIOA0_BASE
#define B1_PORT GPIOA1_BASE

// Address Signals
#define A_PORT GPIOA0_BASE
#define D_PORT GPIOA0_BASE
#define B_PORT GPIOA3_BASE
#define C_PORT GPIOA2_BASE

// masks to find value for pin in address
#define A_MASK 0x8
#define B_MASK 0x4
#define C_MASK 0x2
#define D_MASK 0x1

// Clock Values
#define CLK_PULSE 110/(portTICK_PERIOD_MS*1000000) // 110 nano seconds TODO figure out actual num
#define ALPHA_DELAY 1/(portTICK_PERIOD_MS) //1 millisecond, time between row output
#define SHIFT_DELAY 1/100000

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif
 /* LEDBOARDGPIO_H_ */
