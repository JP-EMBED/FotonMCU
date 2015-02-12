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
#define BLUETOOTH               UARTA0_BASE
#define BLUETOOTH_PERIPH        PRCM_UARTA0
#define SYSTICKS_PER_SECOND     100
#define BUFF_SIZE               20
#define HI_MASK                 5
#define LOW_MASK                31

//HC_05Bluetooth FotonHC_05Bluetooth;

void BluetoothReadTask(void * nothing)
{
	while(1)
	{
		int index(0);
		if((FOTON_BLUETOOTH->FrontIndex != FOTON_BLUETOOTH->BackIndex))
		{
			while(index < 40 && (FOTON_BLUETOOTH->FrontIndex != FOTON_BLUETOOTH->BackIndex))
			{
				// PROCESS BYTE BY BYTE
				if(FOTON_BLUETOOTH->FrontIndex >= MAX_COMMAND_INDEX  )
					FOTON_BLUETOOTH->FrontIndex = 0;
				FOTON_BLUETOOTH->processNextByte(FOTON_BLUETOOTH->mMessageBuffer[FOTON_BLUETOOTH->FrontIndex]);
				++FOTON_BLUETOOTH->FrontIndex;
			}
			UARTIntEnable(BLUETOOTH, UART_INT_DMARX);
			UARTDMAEnable(BLUETOOTH, UART_DMA_RX);
		}
		vTaskDelay(30 * portTICK_PERIOD_MS); // sleep 30 milliseconds
	}
}


void BlueToothInterruptHandler()
{
	if(UARTCharsAvail(BLUETOOTH))
	{
		while(UARTCharsAvail(BLUETOOTH))
		{
			// GRAB FROM FIFO UNTIL IT IS EMPTY
			if(FOTON_BLUETOOTH->BackIndex >= MAX_COMMAND_INDEX )
				FOTON_BLUETOOTH->BackIndex = 0;
			FOTON_BLUETOOTH->mMessageBuffer[FOTON_BLUETOOTH->BackIndex] = UARTCharGetNonBlocking(BLUETOOTH);
			++FOTON_BLUETOOTH->BackIndex;

		}
		UARTIntDisable(BLUETOOTH, UART_INT_DMARX);
		UARTDMADisable(BLUETOOTH, UART_DMA_RX);
	}
    UARTIntClear(BLUETOOTH,UART_INT_DMARX);
    UARTRxErrorClear(BLUETOOTH);


}




HC_05Bluetooth::HC_05Bluetooth(unsigned char RX_pin,unsigned long RX_Mode,
							   unsigned char TX_pin,unsigned long TX_Mode,
							   unsigned char CTS_pin,unsigned long CTS_Mode,
							   unsigned char STATE_pin,unsigned long GPIO_Mode)
	: mEnabled(false),FrontIndex(0),BackIndex(0),mPARSE_STATE(START)
{
	PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);
	// Set up TX and RX pins
	PinTypeUART(TX_pin,TX_Mode);
	PinTypeUART(RX_pin,RX_Mode);

	getPinNumber(STATE_pin,&mStatePinNumber,&mStatePortAddress,&mStatePinAddress);
	unsigned long prcm_port(getGPIOPRCMPort(mStatePortAddress));

	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);
	waitForModeChange();
	PinTypeGPIO(mStatePinNumber, GPIO_Mode, false);
	getPinNumber(CTS_pin, &mCTSPinNumber, &mCTSPortAddress, &mCTSPinAddress);
	prcm_port = getGPIOPRCMPort(mCTSPortAddress);
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);


	waitForModeChange();
	PinTypeGPIO(mCTSPinNumber, CTS_Mode, false);



    enterTransferMode();
    GPIOPinWrite(mCTSPortAddress,mCTSPinAddress, 0);
	FrontIndex = 0;
	BackIndex = 0;
	MessageCount = 0;

}


void HC_05Bluetooth::configureDMATransfers()
{
	UDMAInit();
	uDMAChannelAssign(UDMA_CH8_UARTA0_RX);
	uDMAChannelAssign(UDMA_CH9_UARTA0_TX);



	UARTIntRegister(BLUETOOTH,BlueToothInterruptHandler);

	UARTFIFOLevelSet(BLUETOOTH, UART_FIFO_TX6_8, UART_FIFO_RX6_8);

	//UARTIntEnable(BLUETOOTH,UART_INT_TX);
	UARTIntEnable(BLUETOOTH,UART_INT_DMARX);

    UARTDMAEnable(BLUETOOTH, UART_DMA_RX);
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
		UARTFIFODisable(BLUETOOTH);
		mEnabled = false;
	}
}

void HC_05Bluetooth::enterConfigureMode()
{
	//disable();
	// Enter AT Command mode signal to HC-05 (assume connected to KEY/STATE pin on HC-05)	//PinConfigSet(mStatePinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
	GPIOPinWrite(mStatePortAddress,mStatePinAddress, 1);
	waitForModeChange();

	// set baud to 38400
	//UARTConfigSetExpClk(BLUETOOTH,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),
	//	COMMAND_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	//	UART_CONFIG_PAR_NONE));
	//enable();


}


void HC_05Bluetooth::waitForModeChange()
{
	// wait some time for the HC-05 to catch up on mode change
	for(int i = 0; i < 50; ++i)
		asm(" nop");
}

void HC_05Bluetooth::enterTransferMode()
{
	//disable();
	GPIOPinWrite(mStatePortAddress,mStatePinAddress, 0);
	//PinConfigSet(mStatePinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD_PD);
	waitForModeChange();
	// set clock speed back to 1382400
	UARTConfigSetExpClk(BLUETOOTH,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),BLUETUTH_BAUD_RATE,
	                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	//enable();


}


void HC_05Bluetooth::processNextByte(char byte)
{
	switch(mPARSE_STATE)
	{
		case START:
		{
			mPARSE_STATE = FUNCTION_MJR;
			break;
		}
		case FUNCTION_MJR:
		{

			CURRENT_MESSAGE->FUNC_MJR = (byte >> HI_MASK);
			CURRENT_MESSAGE->ROW = (byte &LOW_MASK);
			mPARSE_STATE = FUNCTION_MNR;
			break;
		}
		case FUNCTION_MNR:
		{
			CURRENT_MESSAGE->FUNC_MNR = (byte >> HI_MASK);
			CURRENT_MESSAGE->COL = (byte & LOW_MASK);
			mPARSE_STATE = RED;
			break;
		}
		case RED:
		{
			CURRENT_MESSAGE->RED = byte;
			mPARSE_STATE = GREEN;
			break;
		}
		case GREEN:
		{
			CURRENT_MESSAGE->GREEN = byte;
			mPARSE_STATE = BLUE;
			break;
		}
		case BLUE:
		{
			CURRENT_MESSAGE->BLUE = byte;
			// perform or put the command into action
			//LEDBoard->ledSet(CURRENT_MESSAGE);
			mPARSE_STATE = START;
			break;
		}
		default:{break;}
	}
}


void HC_05Bluetooth::sendMessage(const char * message,unsigned int length)
{
	unsigned int i(0);
	for(i = 0; (i < length && message[i] != 0); i++)
	{
		UARTCharPut(BLUETOOTH,message[i]);
	}


}


void HC_05Bluetooth::setReadMode()
{
	//PinConfigSet(mCTSPinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
	GPIOPinWrite(mCTSPortAddress,mCTSPinAddress, 1);
	waitForModeChange();

}



void HC_05Bluetooth::setWriteMode()
{
	//PinConfigSet(mCTSPinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD);
	GPIOPinWrite(mCTSPortAddress,mCTSPinAddress, 0);
	waitForModeChange();
}


HC_05Bluetooth::~HC_05Bluetooth()
{
	disable();

}
