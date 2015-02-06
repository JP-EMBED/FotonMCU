/************************************************************************
* file: DisplayDriver.h
* author: Kat Derby
* Revision: 1.0
* Last Revision Date: 2/5/2015
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
*		ENABLEPWM()
*			Enables the PWM
*		DISABLEPWM()
*			Disables the PWM
*		UPDATEDUTYCYCLE(lvl)
*			Updates the duty cycle of the PWM
*
*		INCREMTENTSHIFT(SHIFT, ADDR)
*			Increment pixel shift for current row accordingl
*		INCREMENTADDR( ADDR )
*			Increment address accordingly
*
*************************************************************************/
#ifndef DISPLAYDRIVER_H_
#define DISPLAYDRIVER_H_

#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin.h>
#include <driverlib/gpio.h>
#include <driverlib/prcm.h>
#include "timingstuff.h"
#include "LEDBoardGPIO.h"

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
 HWREG(R0_PORT + (GPIO_O_GPIO_DATA + (R0_PIN << 2))) = ((color[P0_NUM].red >> SHIFT )& 1)<<6; \
 HWREG(R1_PORT + (GPIO_O_GPIO_DATA + (R1_PIN << 2))) = ((color[P1_NUM].red >> SHIFT )& 1)<<5

 /**********************************************************************
 *
 * sets G0 and G1 for the current bit and pixel data
 * P0_NUM: current pixel for current top row, used for placement in color array
 * P1_NUM: current pixel for current bottom row
 * SHIFT: bit placement in byte
 *
 **********************************************************************/
#define SETGREEN( color,  P0_NUM,  P1_NUM,  SHIFT) \
 HWREG(G0_PORT + (GPIO_O_GPIO_DATA + (G0_PIN << 2))) = ((color[P0_NUM].green >> SHIFT )& 1)<<4; \
 HWREG(G1_PORT + (GPIO_O_GPIO_DATA + (G1_PIN << 2))) = ((color[P1_NUM].green >> SHIFT )& 1)<<6

 /**********************************************************************
 *
 * sets B0 and B1 for the current bit and pixel data
 * P0_NUM: current pixel for current top row, used for placement in color array
 * P1_NUM: current pixel for current bottom row
 * SHIFT: bit placement in byte
 *
 **********************************************************************/
#define SETBLUE( color,  P0_NUM,  P1_NUM,  SHIFT) \
 HWREG(B0_PORT + (GPIO_O_GPIO_DATA + (B0_PIN << 2))) = ((color[P0_NUM].blue >> SHIFT )& 1)<<5; \
 HWREG(B1_PORT + (GPIO_O_GPIO_DATA + (B1_PIN << 2))) = ((color[P1_NUM].blue >> SHIFT )& 1)<<7

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
 HWREG(A_PORT + (GPIO_O_GPIO_DATA + (A_PIN << 2))) = (CURRENT_ROW & A_MASK)<<4; \
 HWREG(B_PORT + (GPIO_O_GPIO_DATA + (B_PIN << 2))) = (CURRENT_ROW & B_MASK)<<4; \
 HWREG(C_PORT + (GPIO_O_GPIO_DATA + (C_PIN << 2))) = (CURRENT_ROW & C_MASK)<<5; \
 HWREG(D_PORT + (GPIO_O_GPIO_DATA + (D_PIN << 2))) = (CURRENT_ROW & D_MASK)<<4

 /**********************************************************************
 *
 * Asserts the BLANK signal
 *
 **********************************************************************/
 #define SETBLANK() HWREG(BLANK_PORT + (GPIO_O_GPIO_DATA + (BLANK_PIN << 2))) = BLANK_PIN

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
 #define SETLATCH() HWREG(LATCH_PORT + (GPIO_O_GPIO_DATA + (LATCH_PIN << 2))) = LATCH_PIN

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
 * Enable the PWM < TODO
 *
 **********************************************************************/
 #define ENABLEPWM() \
 HWREG(PWM_PORT + TIMER_O_CTL) |= TIMER_A & (TIMER_CTL_TAEN | TIMER_CTL_TBEN)

 /**********************************************************************
 *
 * Disable the PWM < TODO
 *
 **********************************************************************/
 #define DISABLEPWM() \
 HWREG(PWM_PORT + TIMER_O_CTL) &= ~(TIMER_A & (TIMER_CTL_TAEN | TIMER_CTL_TBEN))

 /**********************************************************************
 *
 * Update duty cycle of the PWM < TODO
 * lvl: current iteration of delay time
 *
 **********************************************************************/
 #define UPDATEDUTYCYCLE ( level ) HWREG(PWM_PORT + TIMER_O_TAMATCHR) = BASE_DELAY_TIME*( 1 << level )

 /**********************************************************************
 *
 * Increment pixel shift for current row accordingly < TODO
 * SHIFT: current bit placement in color byte
 * ADDR: current row number
 *
 **********************************************************************/
 #define INCREMTENTSHIFT( SHIFT, ADDR ) \
 { if ( SHIFT < 8 ) \
	SHIFT++; \
   else \
	SHIFT=0; INCREMENTADDR(ADDR);}
 /**********************************************************************
 *
 * Increment address accordingly < TODO
 * ADDR: ccurrent row number
 *
 **********************************************************************/
#define INCREMENTADDR( ADDR ) \
 { if ( ADDR < 16 ) \
	ADDR++; \
   else \
	ADDR=0;}


#endif /* DISPLAYDRIVER_H_ */
