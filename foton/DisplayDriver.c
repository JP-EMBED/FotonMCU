/************************************************************************
* file: DisplayDriver.c
* author: Kat Derby
* Revision: 2.0
* Last Revision Date: 3/1/2015
*
* Defined Functions:
*		- Defined functions that allow for functionality of the LED Board
*
*		DisplayCurrentImageRGB(void * d)
*			RGB image driver task, only allows for simple RGB values
*		DisplayCurrentImageBCM(void * d)
*			BCM image driver task, allows for wide range of colors
*		ConfigureDisplayDriver(DisplayDriver * driver)
*			Initializes DisplayDriver values to default values
*		ConfigLEDPins(void)
*			Sets up the pins for driving the led display board
*
*************************************************************************/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "osi.h"

#include <hw_types.h>
#include <hw_memmap.h>
#include <hw_gpio.h>
#include <pin.h>
#include <gpio.h>
#include <prcm.h>


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
void DisplayCurrentImageRGB(void * d)
{
	DisplayDriver * driver = (DisplayDriver *)d;
	int CURRENT_PIXEL=0;
	int CURRENT_ROW=0;
	int PIX0=0;
	int PIX1=0;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = ALPHA_DELAY;
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		for (CURRENT_ROW=0; CURRENT_ROW<16;CURRENT_ROW++)
		{
			for (CURRENT_PIXEL=0; CURRENT_PIXEL<32; CURRENT_PIXEL++)
			{
				SETP0( PIX0,  CURRENT_ROW,  CURRENT_PIXEL);
				SETP1( PIX1,  CURRENT_ROW,  CURRENT_PIXEL);
				SETCOLOR( (*driver).CURRENT_DISP_IMAGE, PIX0,  PIX1,  0);

				//PULSECLK();
				SETCLK();
				// TODO < Fix delay issue here somehow
				//vTaskDelay(CLK_PULSE);
				CLRCLK();
			}

				SETBLANK();
						// Change Address in board to correct Address
				SETADDR((*driver).addr[CURRENT_ROW]);
						//UtilsDelay(500);
						// Set Latch Signal
				SETLATCH();
						// Clr Latch Signal
				CLRLATCH();
						// Clr Blank Signal
				CLRBLANK();
				//vTaskDelay(ALPHA_DELAY); // fetch next delay
				vTaskDelayUntil( &xLastWakeTime, xFrequency );
		}
	}
}

// TODO this actually doing a thing
void DisplayCurrentImageBCM(void * d)
{
	DisplayDriver * driver = (DisplayDriver *)d;
	int CURRENT_PIXEL=0;
	int CURRENT_ROW=0;
	int SHIFT=0;
	int PIX0=0;
	int PIX1=0;
	while(1)
	{
		for (CURRENT_ROW=0; CURRENT_ROW<16;CURRENT_ROW++)
		{
			for (SHIFT=0;SHIFT<8;SHIFT++)
			{
				for (CURRENT_PIXEL=0; CURRENT_PIXEL <=31; CURRENT_PIXEL++)
				{
					SETP0( PIX0,  CURRENT_ROW,  CURRENT_PIXEL);
					SETP1( PIX1,  CURRENT_ROW,  CURRENT_PIXEL);
					SETCOLOR( (*driver).CURRENT_DISP_IMAGE, PIX0,  PIX1,  SHIFT);

					//PULSECLK();
					SETCLK();
					// TODO < Fix delay issue here somehow
					//vTaskDelay(CLK_PULSE);
					CLRCLK();
				}

					SETBLANK();
							// Change Address in board to correct Address
					SETADDR((*driver).addr[CURRENT_ROW]);
							//UtilsDelay(500);
							// Set Latch Signal
					SETLATCH();
							// Clr Latch Signal
					CLRLATCH();
							// Clr Blank Signal
					CLRBLANK();
					//vTaskDelay(ALPHA_DELAY); // fetch next delay
					vTaskDelay( SHIFT_DELAY * (1 << SHIFT) );
			}
			vTaskDelay( ALPHA_DELAY );
		}
	}
}

/**********************************************************************
 *
 * This function needs to be performed before outputting
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
	(*driver).GLOBAL_COLOR.red=255;
	(*driver).GLOBAL_COLOR.green=0;
	(*driver).GLOBAL_COLOR.blue=0;
}

/**********************************************************************
 *
 * This function needs to be performed before outputting
 * 		any data to the LED Board.
 *
 **********************************************************************/
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
	    // Configure PIN_57 for GPIO Output
	    // C
	    PinTypeGPIO(PIN_57, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA0_BASE, 0x4, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_21 for GPIO Output
	    // BLANK
	    PinTypeGPIO(PIN_21, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA3_BASE, 0x2, GPIO_DIR_MODE_OUT);


	    //
	    // Configure PIN_03 for GPIO Output
	    // G0
	    PinTypeGPIO(PIN_03, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA1_BASE, 0x10, GPIO_DIR_MODE_OUT);

	    //
	    // Configure PIN_52 for GPIO Output
	    // B0
	    PinTypeGPIO(PIN_52, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA4_BASE, 0x1, GPIO_DIR_MODE_OUT);

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
	    PinTypeGPIO(PIN_08, PIN_MODE_0, false);
	    GPIODirModeSet(GPIOA2_BASE, 0x2, GPIO_DIR_MODE_OUT);
}
