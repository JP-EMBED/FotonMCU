//*
// * HC-05driver.c
// *
// *  Created on: Nov 21, 2014
// *      Author: Christopher
// *

#include "HC-05driver.h"


#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "hw_uart.h"
#include "uart.h"
#include "gpio.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "udma.h"
#include "uart_if.h"
#include "udma_if.h"
#include "pin.h"

#include "utility_functions.h"

#define BLUETUTH_BAUD_RATE      1382400
#define COMMAND_BAUD_RATE       38400
#define BLUETUTHCLK             80000000
#define BLUETOOTH               UARTA1_BASE
#define BLUETOOTH_PERIPH        PRCM_UARTA1
#define SYSTICKS_PER_SECOND     100
#define BUFF_SIZE               20


//HC_05Bluetooth FotonHC_05Bluetooth;





void BlueToothInterruptHandler()
{

	unsigned long ulStatus;

	//unsigned long ulMode;
	    //
	    // Read the interrupt status of the UART.
	    //
	ulStatus = MAP_UARTIntStatus(BLUETOOTH, 1);
	UARTIntClear(BLUETOOTH,ulStatus);

	UARTIntEnable(BLUETOOTH,UART_INT_RX);

	  /*  if(!bRxDone)
	    {
	        MAP_UARTDMADisable(UARTA0_BASE,UART_DMA_RX);
	        bRxDone = true;
	    }
	    else
	    {
	        MAP_UARTDMADisable(UARTA0_BASE,UART_DMA_TX);
	    }

	    MAP_UARTIntClear(UARTA0_BASE,UART_INT_DMATX|UART_INT_DMARX);*/

}




HC_05Bluetooth::HC_05Bluetooth(unsigned long RX_pin,unsigned long RX_Mode,
							   unsigned long TX_pin,unsigned long TX_Mode,
							   unsigned long CTS_pin,unsigned long CTS_Mode,
							   unsigned long STATE_pin,unsigned long GPIO_Mode)
	: mEnabled(false)
{

	// Set up TX and RX pins
	PinTypeUART(TX_pin,TX_Mode);
	PinTypeUART(RX_pin,RX_Mode);

	getPinNumber(STATE_pin,&mStatePinNumber,&mStatePortAddress,&mStatePinAddress);
	unsigned long prcm_port(getGPIOPRCMPort(mStatePortAddress));
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);
	PinTypeGPIO(mStatePinNumber, GPIO_Mode,false);
	GPIODirModeSet(mStatePortAddress,mStatePinAddress,GPIO_DIR_MODE_OUT);

	getPinNumber(CTS_pin, &mCTSPinNumber, &mCTSPortAddress, &mCTSPinAddress);
	prcm_port = getGPIOPRCMPort(mCTSPortAddress);
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);
	PinTypeGPIO(mCTSPinNumber, CTS_Mode,false);
	GPIODirModeSet(mCTSPortAddress,mCTSPinAddress,GPIO_DIR_MODE_OUT);

	GPIOPinWrite(mStatePortAddress,mStatePinNumber,0);
	GPIOPinWrite(mCTSPortAddress,mCTSPinNumber,0);
	PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);


		// set clock speed back to 1382400
	UARTConfigSetExpClk(BLUETOOTH,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),BLUETUTH_BAUD_RATE,
		               (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	configureDMATransfers();
	enable();
	FrontIndex = 0;
	BackIndex = 0;
	MessageCount = 0;

}


void HC_05Bluetooth::configureDMATransfers()
{

	uDMAChannelAssign(UDMA_CH10_UARTA1_RX);
	uDMAChannelAssign(UDMA_CH11_UARTA1_TX);



	UARTIntRegister(BLUETOOTH,BlueToothInterruptHandler);

	UARTFIFOLevelSet(BLUETOOTH, UART_FIFO_TX4_8, UART_FIFO_RX4_8);

	//UARTIntEnable(BLUETOOTH,UART_INT_DMATX);
	UARTIntEnable(BLUETOOTH,UART_INT_DMARX);

    UARTDMAEnable(UARTA1_BASE, UART_DMA_RX | UART_DMA_TX);
}



void HC_05Bluetooth::enable()
{
	if(!mEnabled)
	{
		UARTFIFOEnable(BLUETOOTH);
		UARTEnable(BLUETOOTH);
		mEnabled = true;
	}
}

void HC_05Bluetooth::disable()
{
	if(mEnabled)
	{
		UARTDisable(BLUETOOTH);
		PRCMPeripheralClkDisable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);
		UARTFIFODisable(BLUETOOTH);
		mEnabled = false;
	}
}

void HC_05Bluetooth::enterConfigureMode()
{
	// Enter AT Command mode signal to HC-05 (assume connected to KEY/STATE pin on HC-05)
	GPIOPinWrite(mStatePortAddress,mStatePinNumber,mStatePinAddress);

	// set baud to 38400
	UARTConfigSetExpClk(BLUETOOTH,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),
		COMMAND_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		UART_CONFIG_PAR_NONE));

	waitForModeChange();
}


void HC_05Bluetooth::waitForModeChange()
{
	// wait some time for the HC-05 to catch up on mode change
	for(int i = 0; i < 40; ++i)
		asm(" nop");
}

void HC_05Bluetooth::enterTransferMode()
{
	GPIOPinWrite(mStatePortAddress,mStatePinNumber,0);

	// set clock speed back to 1382400
	UARTConfigSetExpClk(BLUETOOTH,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),BLUETUTH_BAUD_RATE,
	                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	waitForModeChange();
}


void HC_05Bluetooth::sendMessage(const char * message,unsigned int length)
{
	unsigned int i(0);
	for(i = 0; (i < length && message[i] != 0); i++)
	{
		UARTCharPut(BLUETOOTH,message[i]);
	}

/*	SetupTransfer(UDMA_CH11_UARTA1_TX| UDMA_PRI_SELECT,
	           UDMA_MODE_BASIC, length,UDMA_SIZE_8, UDMA_ARB_4,
	           message, UDMA_SRC_INC_8,(void *)(UARTA1_BASE + UART_O_DR),
	                                                UDMA_DST_INC_NONE);*/
}


void HC_05Bluetooth::setReadMode()
{
    GPIOPinWrite(mCTSPortAddress,mCTSPinNumber,mCTSPinAddress);

}



void HC_05Bluetooth::setWriteMode()
{
	GPIOPinWrite(mCTSPortAddress,mCTSPinNumber,0);
}


HC_05Bluetooth::~HC_05Bluetooth()
{
	disable();

}
