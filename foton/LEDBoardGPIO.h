/************************************************************************
* file: LEDBOARDGPIO.h
* author: Kat Derby
* Revision: 2.5
* Last Revision Date: 1/31/2015
*
* Macro Functions:
*	- Macros used in sending data out to the LED board
*		SETP0(int P0_NUM, int CURRENT_ROW, int CURRENT_PIXEL)
*			sets the current pixel for the row in top section of the led display
*		SETP1(int P1_NUM, int CURRENT_ROW, int CURRENT_PIXEL)
*			sets the current pixel for the row in bottom section of the led display
*
*		SETRED(FOTON_RGB color[ ], int P0_NUM, int P1_NUM, int SHIFT)
*			sets R0 and R1 for the current bit and pixel data
*		SETGREEN(FOTON_RGB color[ ], int P0_NUM, int P1_NUM, int SHIFT)
*			sets G0 and G1 for the current bit and pixel data
*		SETBLUE(FOTON_RGB color[ ], int P0_NUM, int P1_NUM, int SHIFT)
*			sets B0 and B1 for the current bit and pixel data
*		SETCOLOR(FOTON_RGB color[ ], int P0_NUM, int P1_NUM, int SHIFT)
*			sets RGB0 and RGB1 for the current bit and pixel data
*
*		SETBLANK()
*			Asserts the BLANK signal
*		CLRBLANK()
*			Negates the BLANK signal
*
*		SETLATCH()
*			Asserts the LATCH signal
*		CLRLATCH()
*			Negates the LATCH signal
*
*		ENABLECLK()
*			Enables the CLK signal
*		DISABLECLK()
*			Disables the CLK signal
*
* Defined Values: 
*		- Defined Values that make using macros and the GPIO for the
*			LED board signals easier
* 
*************************************************************************/

#ifndef LEDBOARDGPIO_H_
#define LEDBOARDGPIO_H_

#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin.h>
#include <driverlib/gpio.h>
#include <driverlib/prcm.h>
#include "timingstuff.h"

/**********************************************************************
 *
 * sets the current pixel for the row in top section of the led display
 * P0_NUM will be used to grab the color from the FOTON_RGB array
 *
 **********************************************************************/
#define SETP0( P0_NUM,  CURRENT_ROW,  CURRENT_PIXEL) \
 P0_NUM = CURRENT_ROW*32  + CURRENT_PIXEL

/**********************************************************************
 *
 * sets the current pixel for the row in bottom section of the led display
 * P1_NUM will be used to grab the color from the FOTON_RGB array
 *
 **********************************************************************/
#define SETP1( P1_NUM,  CURRENT_ROW,  CURRENT_PIXEL) \
 P1_NUM = (CURRENT_ROW+16)*32 + CURRENT_PIXEL

/**********************************************************************
 *
 * sets R0 and R1 for the current bit and pixel data
 * P0_NUM: current pixel for current top row, used for placement in color array
 * P1_NUM: current pixel for current bottom row
 * SHIFT: bit placement in byte 
 *
 **********************************************************************/
#define SETRED( color, P0_NUM, P1_NUM, SHIFT) \
 HWREG(R0_PORT + (GPIO_O_GPIO_DATA + (R0_PIN << 2))) = (color[P0_NUM].red >> SHIFT )& 1; \
 HWREG(R1_PORT + (GPIO_O_GPIO_DATA + (R1_PIN << 2))) = (color[P1_NUM].red >> SHIFT )& 1 

 /**********************************************************************
 *
 * sets G0 and G1 for the current bit and pixel data
 * P0_NUM: current pixel for current top row, used for placement in color array
 * P1_NUM: current pixel for current bottom row
 * SHIFT: bit placement in byte 
 *
 **********************************************************************/
#define SETGREEN( color,  P0_NUM,  P1_NUM,  SHIFT) \
 HWREG(G0_PORT + (GPIO_O_GPIO_DATA + (G0_PIN << 2))) = (color[P0_NUM].green >> SHIFT )& 1; \
 HWREG(G1_PORT + (GPIO_O_GPIO_DATA + (G1_PIN << 2))) = (color[P1_NUM].green >> SHIFT )& 1 
 
 /**********************************************************************
 *
 * sets B0 and B1 for the current bit and pixel data
 * P0_NUM: current pixel for current top row, used for placement in color array
 * P1_NUM: current pixel for current bottom row
 * SHIFT: bit placement in byte 
 *
 **********************************************************************/
#define SETBLUE( color,  P0_NUM,  P1_NUM,  SHIFT) \
 HWREG(B0_PORT + (GPIO_O_GPIO_DATA + (B0_PIN << 2))) = (color[P0_NUM].blue >> SHIFT )& 1; \
 HWREG(B1_PORT + (GPIO_O_GPIO_DATA + (B1_PIN << 2))) = (color[P1_NUM].blue >> SHIFT )& 1 
 
 /**********************************************************************
 *
 * sets RGB0 and RGB1 for the current bit and pixel data
 * P0_NUM: current pixel for current top row, used for placement in color array
 * P1_NUM: current pixel for current bottom row
 * SHIFT: bit placement in byte
 *
 **********************************************************************/
#define SETCOLOR( color,  P0_NUM,  P1_NUM,  SHIFT) \
 SETRED(color, P0_NUM, P1_NUM, SHIFT); \
 SETGREEN(color, P0_NUM, P1_NUM, SHIFT); \
 SETBLUE(color, P0_NUM, P1_NUM, SHIFT)
 
 /**********************************************************************
 *
 * sets ABCD for the current row
 * CURRENT_ROW: integer value of the current row of data (0-15)
 *		For this macro to function correctly: -1 < CURRENT_ROW < 16
 *
 **********************************************************************/
#define SETADDR( CURRENT_ROW ) \
 HWREG(A_PORT + (GPIO_O_GPIO_DATA + (A_PIN << 2))) = CURRENT_ROW & A_MASK; \
 HWREG(B_PORT + (GPIO_O_GPIO_DATA + (B_PIN << 2))) = CURRENT_ROW & B_MASK; \
 HWREG(C_PORT + (GPIO_O_GPIO_DATA + (C_PIN << 2))) = CURRENT_ROW & C_MASK; \
 HWREG(D_PORT + (GPIO_O_GPIO_DATA + (D_PIN << 2))) = CURRENT_ROW & D_MASK 
 
 /**********************************************************************
 *
 * Asserts the BLANK signal
 *
 **********************************************************************/
 #define SETBLANK() HWREG(BLANK_PORT + (GPIO_O_GPIO_DATA + (BLANK_PIN << 2))) = 1
 
 /**********************************************************************
 *
 * Negates the BLANK signal
 *
 **********************************************************************/
 #define CLRBLANK() HWREG(BLANK_PORT + (GPIO_O_GPIO_DATA + (BLANK_PIN << 2))) = 0
 
  /**********************************************************************
 *
 * Asserts the LATCH signal
 *
 **********************************************************************/
 #define SETLATCH() HWREG(LATCH_PORT + (GPIO_O_GPIO_DATA + (LATCH_PIN << 2))) = 1
 
 /**********************************************************************
 *
 * Negates the LATCH signal
 *
 **********************************************************************/
 #define CLRLATCH() HWREG(LATCH_PORT + (GPIO_O_GPIO_DATA + (LATCH_PIN << 2))) = 0
 
 /**********************************************************************
 *
 * Enable the CLK signal
 *
 **********************************************************************/
 #define ENABLECLK() \
 HWREG(CLK_PORT + TIMER_O_CTL) |= TIMER_B & (TIMER_CTL_TAEN | TIMER_CTL_TBEN)
 
 /**********************************************************************
 *
 * Disable the CLK signal
 *
 **********************************************************************/
 #define DISABLECLK() \
 HWREG(CLK_PORT + TIMER_O_CTL) &= ~(TIMER_B & (TIMER_CTL_TAEN | TIMER_CTL_TBEN))
 
  /**********************************************************************
 *
 * Enable the PWM signal
 *
 **********************************************************************/
 #define ENABLEPWM() \
 HWREG(PWM_PORT + TIMER_O_CTL) |= TIMER_A & (TIMER_CTL_TAEN | TIMER_CTL_TBEN)
 
 /**********************************************************************
 *
 * Disable the PWM signal
 *
 **********************************************************************/
 #define DISABLEPWM() \
 HWREG(PWM_PORT + TIMER_O_CTL) &= ~(TIMER_A & (TIMER_CTL_TAEN | TIMER_CTL_TBEN))
 
 /**********************************************************************
 *
 * Update duty cycle of the PWM
 * lvl: current iteration of delay time
 *
 **********************************************************************/
 #define UPDATEDUTYCYCLE ( level ) HWREG(PWM_PORT + TIMER_O_TAMATCHR) = BASE_DELAY_TIME*( 1 << level )
 
 
// bit packed pin representation for each signal
// Control Signals
#define LATCH_PIN 0x01
#define BLANK_PIN 0x04
#define CLK_PIN PIN_02

// Color Coding Signals
// Top Rows
#define R0_PIN 0x40
#define G0_PIN 0x10
#define B0_PIN 0x20

// Bottom Rows
#define R1_PIN 0x20
#define G1_PIN 0x40
#define B1_PIN 0x08

// Address Signals
#define A_PIN 0x10
#define B_PIN 0x80
#define C_PIN 0x40
#define D_PIN 0x02


// Ports for each signal
// Control Signals
#define LATCH_PORT GPIOA0_BASE
#define BLANK_PORT GPIOA1_BASE
#define CLK_PORT TIMERA3_BASE
#define PWM_PORT TIMERA3_BASE

// Color Coding Signals
// Top Rows
#define R0_PORT GPIOA1_BASE
#define G0_PORT GPIOA1_BASE
#define B0_PORT GPIOA1_BASE

// Bottom Rows
#define R1_PORT GPIOA0_BASE
#define G1_PORT GPIOA0_BASE
#define B1_PORT GPIOA0_BASE

// Address Signals
#define A_PORT GPIOA0_BASE
#define B_PORT GPIOA0_BASE
#define C_PORT GPIOA3_BASE
#define D_PORT GPIOA1_BASE

// masks to find value for pin in address
#define A_MASK 0x8
#define B_MASK 0x4
#define C_MASK 0x2
#define D_MASK 0x1

// Clock Values
#define SHIFT_CLK_RELOAD 6
#define TIMER_INTERVAL_RELOAD   24576
#define BASE_DELAY_TIME 192


#endif /* LEDBOARDGPIO_H_ */
