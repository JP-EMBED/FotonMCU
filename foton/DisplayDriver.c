/************************************************************************
* file: DisplayDriver.c
* author: Kat Derby
* Revision: 1.0
* Last Revision Date: 2/18/2015
*
* Defined Functions:
*		- Defined functions that allow for functionality of the LED Board
*
*		DisplayImage
*			- Displays a 1064 pixel image to the LED board
*
*************************************************************************/
#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin.h>
#include <driverlib/gpio.h>
#include <driverlib/prcm.h>

#include "DisplayDriver.h"
#include "LEDBoardGPIO.h"
#include "fotonrgb.h"


/**********************************************************************
 *
 * the image displayed with this function, only uses rgb
 * DisplayDriver is defined in DisplayDriver.h
 * ConfigureDisplayDriver needs to be peroformed before calling this
 * This function needs to be in a continuos looop to work correctly
 *
 **********************************************************************/
void DisplayCurrentImage(DisplayDriver * driver)
{
	int i=0;
	for (i=0; i<32; i++)
	{
		SETP0( (*driver).P0_NUM,  (*driver).ADDR,  i);
		SETP1( (*driver).P1_NUM,  (*driver).ADDR,  i);
		SETCOLOR( (*driver).IMAGE_ONEBUFF,  (*driver).P0_NUM,  (*driver).P1_NUM,  0);

		//PULSECLK();
		SETCLK();
		// TODO < Fix delay issure here somehow
		// UtilsDelay(300);
		CLRCLK();

		}

		SETBLANK();
				// Change Address in board to correct Address
		SETADDR((*driver).addr[(*driver).ADDR]);
				//UtilsDelay(500);
				// Set Latch Signal
		SETLATCH();
				// delay
		//UtilsDelay(50);
				// Clr Latch Signal
		CLRLATCH();
				// Clr Blank Signal
		CLRBLANK();
				// Check if addr == 15, if not add 1 else set it to 0*/
		INCREMENTADDR( (*driver).ADDR );
}

/**********************************************************************
 *
 * This function needs t0 be performed before outputting
 * 		any data to the LED Board.
 *
 **********************************************************************/
void ConfigureDisplayDriver(DisplayDriver * driver)
{
	int i=0;
	int addre[32] = {0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15};
	for (i=0;i<32;i++)
	{
		(*driver).addr[i]=addre[i];
	}

	(*driver).CURRENT_DISP_IMAGE = (*driver).IMAGE_ONEBUFF;
	(*driver).NEXT_DISP_IMAGE = (*driver).IMAGE_TWOBUFF;
	(*driver).SHIFT=0;
	(*driver).ADDR=0;
}

void ConfigLEDPins(void)
{
	 //
	    // Enable Peripheral Clocks
	    //
	    PRCMPeripheralClkEnable(PRCM_GPIOA0, PRCM_RUN_MODE_CLK);
	    PRCMPeripheralClkEnable(PRCM_GPIOA1, PRCM_RUN_MODE_CLK);
	    PRCMPeripheralClkEnable(PRCM_GPIOA2, PRCM_RUN_MODE_CLK);
	    PRCMPeripheralClkEnable(PRCM_GPIOA3, PRCM_RUN_MODE_CLK);
	    PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);

	    //
	    // Configure PIN_50 for GPIO Output
	    // LATCH
	    PinTypeGPIO(PIN_50, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA0_BASE, 0x1, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_06 for GPIO Output
	    // B1
	    PinTypeGPIO(PIN_06, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x80, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_59 for GPIO Output
	    // D
	    PinTypeGPIO(PIN_59, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA0_BASE, 0x10, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_60 for GPIO Output
	    // R1
	    PinTypeGPIO(PIN_60, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA0_BASE, 0x20, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_61 for GPIO Output
	    // G1
	    PinTypeGPIO(PIN_61, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA0_BASE, 0x40, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_62 for GPIO Output
	    // A
	    PinTypeGPIO(PIN_62, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA0_BASE, 0x80, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_15 for GPIO Output
	    // C
	    PinTypeGPIO(PIN_15, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA2_BASE, 0x40, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_01 for GPIO Output
	    // BLANK
	    PinTypeGPIO(PIN_01, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x4, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_03 for GPIO Output
	    // G0
	    PinTypeGPIO(PIN_03, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x10, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_04 for GPIO Output
	    // B0
	    PinTypeGPIO(PIN_04, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x20, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_05 for GPIO Output
	    // R0
	    PinTypeGPIO(PIN_05, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x40, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_53 for GPIO Output
	    // B
	    PinTypeGPIO(PIN_53, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA3_BASE, 0x40, GPIO_DIR_MODE_OUT);

	    // CLK
	    PinTypeGPIO(PIN_02, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x8, GPIO_DIR_MODE_OUT);
}
