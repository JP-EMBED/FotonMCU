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
#include "interrupt.h"

#include "utility_functions.h"

#define BLUETUTH_BAUD_RATE      115200
#define COMMAND_BAUD_RATE       38400
#define BLUETUTHCLK             80000000
#define BLUETOOTH               UARTA1_BASE
#define BLUETOOTH_PERIPH        PRCM_UARTA1
#define BLUETOOTH_INT           INT_UARTA1
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
			//UARTIntEnable(BLUETOOTH, UART_INT_DMARX);
			//UARTDMAEnable(BLUETOOTH, UART_DMA_RX);
		}
		vTaskDelay(30 * portTICK_PERIOD_MS); // sleep 30 milliseconds
	}
}


void BlueToothInterruptHandler()
{
	unsigned long ulstatus =  UARTIntStatus(BLUETOOTH,true);
	UARTIntClear(BLUETOOTH, ulstatus);

	// FIND NEW WAY TO CLEAR DMA
	//UARTDMADisable(BLUETOOTH, UART_DMA_RX);
	UARTIntClear(BLUETOOTH,ulstatus);
	uDMAIntClear(UART_DMA_RX |UART_DMA_TX);
	ulstatus =  UARTIntStatus(BLUETOOTH,true);
	char value_read;
	char empty;
	empty = -1;
	if(UARTCharsAvail(BLUETOOTH))
	{
		while(empty != (value_read = UARTCharGetNonBlocking(BLUETOOTH)))
		{
			// GRAB FROM FIFO UNTIL IT IS EMPTY
			if(FOTON_BLUETOOTH->BackIndex >= MAX_COMMAND_INDEX )
				FOTON_BLUETOOTH->BackIndex = 0;
			FOTON_BLUETOOTH->mMessageBuffer[FOTON_BLUETOOTH->BackIndex] = value_read;
			++FOTON_BLUETOOTH->BackIndex;

		}
	}

}




HC_05Bluetooth::HC_05Bluetooth(unsigned char RX_pin,unsigned long RX_Mode,
							   unsigned char TX_pin,unsigned long TX_Mode,
							   unsigned char POWER_pin,unsigned long GPIO_power,
							   unsigned char STATE_pin,unsigned long GPIO_state)
	: mEnabled(false),mPoweredON(false),FrontIndex(0),BackIndex(0),mPARSE_STATE(START)
{
	PRCMPeripheralReset(BLUETOOTH_PERIPH);
	PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);



	// Set up TX and RX pins
	PinTypeUART(TX_pin,TX_Mode);
	PinTypeUART(RX_pin,RX_Mode);
	MAP_UARTConfigSetExpClk(BLUETOOTH,BLUETUTHCLK,
			BLUETUTH_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		                   UART_CONFIG_PAR_NONE));

	getPinNumber(STATE_pin,&mStatePinNumber,&mStatePortAddress,&mStatePinAddress);

	unsigned long prcm_port(getGPIOPRCMPort(mStatePortAddress));

	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);


	PinTypeGPIO(mStatePinNumber, GPIO_state, false);
	GPIODirModeSet(mStatePortAddress,mStatePinAddress,GPIO_DIR_MODE_OUT);

	getPinNumber(POWER_pin, &mPowerPinNumber, &mPowerPortAddress, &mPowerPinAddress);

	prcm_port = getGPIOPRCMPort(mPowerPortAddress);
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);

	PinTypeGPIO(mPowerPinNumber, GPIO_power, false);
	GPIODirModeSet(mPowerPortAddress,mPowerPinAddress,GPIO_DIR_MODE_OUT);



	FrontIndex = 0;
	BackIndex = 0;
	MessageCount = 0;

}


void HC_05Bluetooth::enableDMA()
{
	uDMAChannelAssign(UDMA_CH10_UARTA1_RX);
	uDMAChannelAssign(UDMA_CH11_UARTA1_TX);
    UARTIntEnable(BLUETOOTH, UART_INT_RX |  UART_INT_DMARX);

    UARTDMAEnable(BLUETOOTH, UART_DMA_RX |UART_DMA_TX);
    IntRegister(BLUETOOTH_INT, BlueToothInterruptHandler);
    IntEnable(BLUETOOTH_INT);
}

void HC_05Bluetooth::configureDMATransfers( bool livemode )
{

}


void HC_05Bluetooth::setLiveMode()
{
	// Set To 4 byte mode
	// 32 bit packets
	UARTFIFOLevelSet(BLUETOOTH, UART_FIFO_TX4_8, UART_FIFO_RX4_8);
	// setup DMA Control Table for 4 byte transfer from FIFO
	//   32 bits  (DEST_SIZE)
	//   32 bits  (SRC_SIZE)
	//   Source Address Increment? - NO
	//   Destination Address Increment? 32 BITS
	//   BLUETOOTH_FIFO + 4 BYTES (MIDDLE FIFO)
	//   NEXT_SRC_ADDRESS (Current + 32 bits)
	// Receive the Data
	//SetupTransfer(UDMA_CH10_UARTA1_RX, UDMA_MODE_BASIC,8,UDMA_SIZE_8,
	//	           UDMA_ARB_2,(void *)(BLUETOOTH+UART_O_DR),
	//	           UDMA_SRC_INC_NONE,mRXDataBuff,UDMA_DST_INC_8);



	// Send The Data
	//SetupTransfer(UDMA_CH11_UARTA1_TX, UDMA_MODE_BASIC,
	//	          8,UDMA_SIZE_8, UDMA_ARB_2, mRXDataBuff,
	//	          UDMA_SRC_INC_8,(void *)(BLUETOOTH+UART_O_DR),
	//	          UDMA_DST_INC_NONE);
}


void HC_05Bluetooth::setOtherMode()
{
	// Set to 8 byte mode (64 bit packets).
	UARTFIFOLevelSet(BLUETOOTH, UART_FIFO_TX7_8, UART_FIFO_RX7_8);
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
	//MAP_PRCMPeripheralReset(BLUETOOTH_PERIPH);
	//PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);
	//long value = GPIOPinRead(mStatePortAddress,mStatePinAddress);

	//PRCMPeripheralReset(BLUETOOTH_PERIPH);
	//PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);
	GPIOPinWrite(mStatePortAddress,mStatePinAddress, mStatePinAddress);


	long value = GPIOPinRead(mStatePortAddress,mStatePinAddress);

	// set baud to 38400
	UARTConfigSetExpClk(BLUETOOTH,BLUETUTHCLK,
		COMMAND_BAUD_RATE, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		UART_CONFIG_PAR_NONE));
	enable();


}


void HC_05Bluetooth::waitForModeChange()
{
	// wait some time for the HC-05 to catch up on mode change
	for(int i = 0; i < 3; ++i)
		 __asm("    nop\n"
		        "    nop\n"
		        "    nop\n"
		        "    nop\n");
}

void HC_05Bluetooth::enterTransferMode()
{
	//MAP_PRCMPeripheralReset(BLUETOOTH_PERIPH);
	//PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);
	//PRCMPeripheralReset(BLUETOOTH_PERIPH);
	//disable();
	GPIOPinWrite(mStatePortAddress,mStatePinAddress, 0);
    //PinConfigSet(mStatePinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD_PD);
	waitForModeChange();
	// set clock speed back to 1382400
	UARTConfigSetExpClk(BLUETOOTH,BLUETUTHCLK,BLUETUTH_BAUD_RATE,
	                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	enable();


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

void HC_05Bluetooth::setPowerOn(bool power_on)
{
	if(power_on)
	{
		if(!mPoweredON)
		{
			GPIOPinWrite(mPowerPortAddress,mPowerPinAddress, mPowerPinAddress);

		}
		else
		{
			Message("Attempted to power on an already on Bluetooth module");
		}
	}
	else
	{

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
	//PinConfigSet(mPowerPinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD_PU);
	GPIOPinWrite(mPowerPortAddress,mPowerPinAddress, mPowerPinAddress);
	waitForModeChange();

}



void HC_05Bluetooth::setWriteMode()
{
	//PinConfigSet(mPowerPinNumber,PIN_STRENGTH_2MA,PIN_TYPE_STD);
	GPIOPinWrite(mPowerPortAddress,mPowerPinAddress, 0);
	waitForModeChange();
}


HC_05Bluetooth::~HC_05Bluetooth()
{
	disable();

}
