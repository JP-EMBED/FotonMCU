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
#include "DisplayDriver.h"
#define BLUETUTH_BAUD_RATE      1382400
#define COMMAND_BAUD_RATE       38400
#define BLUETUTHCLK             80000000
#ifdef USING_SERIAL_FOR_BLUETOOTH
#define BLUETOOTH               UARTA0_BASE
#define BLUETOOTH_PERIPH        PRCM_UARTA0
#define BLUETOOTH_INT           INT_UARTA0
#define RX_DMA_CHANNEL          UDMA_CH8_UARTA0_RX
#define TX_DMA_CHANNEL          UDMA_CH9_UARTA0_TX
#else
#define BLUETOOTH               UARTA1_BASE
#define BLUETOOTH_PERIPH        PRCM_UARTA1
#define BLUETOOTH_INT           INT_UARTA1
#define RX_DMA_CHANNEL          UDMA_CH10_UARTA1_RX
#define TX_DMA_CHANNEL          UDMA_CH11_UARTA1_TX
#endif

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
			while((FOTON_BLUETOOTH->FrontIndex != FOTON_BLUETOOTH->BackIndex))
			{
				CURRENT_MESSAGE->FUNC_CTRL = RXDATABUFF[++FOTON_BLUETOOTH->FrontIndex];
				if(FOTON_BLUETOOTH->FrontIndex >= MAX_COMMAND_INDEX  )
					FOTON_BLUETOOTH->FrontIndex = 0;
			    CURRENT_MESSAGE->DATA1 = RXDATABUFF[++FOTON_BLUETOOTH->FrontIndex];
				if(FOTON_BLUETOOTH->FrontIndex >= MAX_COMMAND_INDEX  )
					FOTON_BLUETOOTH->FrontIndex = 0;
			    CURRENT_MESSAGE->DATA2 = RXDATABUFF[++FOTON_BLUETOOTH->FrontIndex];
				if(FOTON_BLUETOOTH->FrontIndex >= MAX_COMMAND_INDEX  )
					FOTON_BLUETOOTH->FrontIndex = 0;
			    CURRENT_MESSAGE->DATA3 =  RXDATABUFF[++FOTON_BLUETOOTH->FrontIndex];

				switch(CURRENT_MESSAGE->FUNC_CTRL)
				{
					case LED_SET_COLOR:
					{
						ledSetColor(CURRENT_MESSAGE->DATA1,CURRENT_MESSAGE->DATA2,CURRENT_MESSAGE->DATA3);
						break;
					}
					case LED_SET_AT:
					{
						ledSet(CURRENT_MESSAGE->DATA1,CURRENT_MESSAGE->DATA2);
						break;
					}
					default:{break;}
				}
				++FOTON_BLUETOOTH->FrontIndex;
				if(FOTON_BLUETOOTH->FrontIndex >= MAX_COMMAND_INDEX  )
					FOTON_BLUETOOTH->FrontIndex = 0;
			}
		}
		vTaskDelay(30 * portTICK_PERIOD_MS); // sleep 30 milliseconds
	}
}

void BluetoothProcessATTask(void*nothing)
{
	while(1)
	{
		int index(0);
		char last = '\0';
		char current = '\0';
		char command_read[200];
		if((FOTON_BLUETOOTH->FrontIndex != FOTON_BLUETOOTH->BackIndex))
		{
			while(FOTON_BLUETOOTH->FrontIndex != FOTON_BLUETOOTH->BackIndex)
			{
				last = current;
				// PROCESS BYTE BY BYTE
				if(FOTON_BLUETOOTH->FrontIndex >= MAX_COMMAND_INDEX  )
				{
					FOTON_BLUETOOTH->FrontIndex = 0;
				}
				current =  RXDATABUFF[FOTON_BLUETOOTH->FrontIndex];
				command_read[index++] = current;
				if(last == '\r' && current == '\n')
					FOTON_BLUETOOTH->processATCommandResponse(command_read, index);
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

    // Only Fires For UART_INT_DMARX
	if(UARTCharsAvail(BLUETOOTH))
	{
		FOTON_BLUETOOTH->BackIndex+= 4; // THE DMA ALREADY TRANSFERED 4 bytes
		if(FOTON_BLUETOOTH->BackIndex >= MAX_COMMAND_INDEX ) // Ring buffer, check end
			FOTON_BLUETOOTH->BackIndex = (FOTON_BLUETOOTH->BackIndex - MAX_COMMAND_INDEX); // adjust index

		// Grab any characters left in the fifo (or are comming in live)
		while(UARTCharsAvail(BLUETOOTH))
		{
			RXDATABUFF[FOTON_BLUETOOTH->BackIndex++] = UARTCharGet(BLUETOOTH); // stuff in Ring Buffer
			if(FOTON_BLUETOOTH->BackIndex >= MAX_COMMAND_INDEX )
					FOTON_BLUETOOTH->BackIndex = 0;
		}

		// Reconfigure DMA for next transfer
		SetupTransfer(RX_DMA_CHANNEL, UDMA_MODE_BASIC,4,UDMA_SIZE_8,
				       UDMA_ARB_1,(void *)(BLUETOOTH+UART_O_DR),
			           UDMA_SRC_INC_NONE,(RXDATABUFF + FOTON_BLUETOOTH->BackIndex),UDMA_DST_INC_8);
	}
	UARTIntClear(BLUETOOTH,UART_INT_DMARX); // Clear the interrupt


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


	// Initialize State GPIO pin
	getPinNumber(STATE_pin,&mStatePinNumber,&mStatePortAddress,&mStatePinAddress);
	unsigned long prcm_port(getGPIOPRCMPort(mStatePortAddress));
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);
	PinTypeGPIO(mStatePinNumber, GPIO_state, false);
	GPIODirModeSet(mStatePortAddress,mStatePinAddress,GPIO_DIR_MODE_OUT);


	// Initialize Power GPIO pin
	getPinNumber(POWER_pin, &mPowerPinNumber, &mPowerPortAddress, &mPowerPinAddress);
	prcm_port = getGPIOPRCMPort(mPowerPortAddress);
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);
	PinTypeGPIO(mPowerPinNumber, GPIO_power, false);
	GPIODirModeSet(mPowerPortAddress,mPowerPinAddress,GPIO_DIR_MODE_OUT);

}


void HC_05Bluetooth::enableDMA()
{
	uDMAChannelAssign(RX_DMA_CHANNEL);
	uDMAChannelAssign(TX_DMA_CHANNEL);
    UARTIntEnable(BLUETOOTH,  UART_INT_DMARX);

    UARTDMAEnable(BLUETOOTH, UART_DMA_RX );
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
	UARTFIFOLevelSet(BLUETOOTH, UART_FIFO_TX4_8, UART_FIFO_RX1_8);

	// setup DMA Control Table for 4 byte transfer from FIFO
	//   32 bits (4x8bits)  (DEST_SIZE)
	//   32 bits(4x8bits)  (SRC_SIZE)
	//   Source Address Increment? - NO
	//   Destination Address Increment? 32 BITS
	//   BLUETOOTH_FIFO (MIDDLE FIFO)
	//   Initial set uses RXDATABUFF with not offset
	// Receive the Data
	FrontIndex = 0;
	BackIndex = 0;
	SetupTransfer(RX_DMA_CHANNEL, UDMA_MODE_BASIC,4,UDMA_SIZE_8,
		           UDMA_ARB_1,(void *)(BLUETOOTH+UART_O_DR),
		           UDMA_SRC_INC_NONE,RXDATABUFF,UDMA_DST_INC_8);

	// Send The Data
	// enabled TX to come
	/*	SetupTransfer(UDMA_CH11_UARTA1_TX, UDMA_MODE_BASIC,
		          8,UDMA_SIZE_8, UDMA_ARB_2, mRXDataBuff,
     	          UDMA_SRC_INC_8,(void *)(BLUETOOTH+UART_O_DR),
		          UDMA_DST_INC_NONE);*/
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
	// Disable the bluetooth and raise State pin
	disable();
	GPIOPinWrite(mStatePortAddress,mStatePinAddress, mStatePinAddress);
	setPowerOn(false);
	waitForModeChange();

	// set baud to 38400
	UARTConfigSetExpClk(BLUETOOTH,BLUETUTHCLK,COMMAND_BAUD_RATE,
		               (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	// Power bluetooth back on
	setPowerOn(true);
	waitForModeChange();

	// If transfer task exists, delete it and create command task
	if(BLUETOOTH_READ_HNDLE)
		vTaskDelete(BLUETOOTH_READ_HNDLE);
	xTaskCreate( BluetoothProcessATTask, "BLE",OSI_STACK_SIZE, NULL, 2, &BLUETOOTH_CMD_READ_HNDLE);
	enable(); // enabled once again

	// Sleep for a while to let HC-05 restart
	waitForModeChange();
	waitForModeChange();
	waitForModeChange();
	waitForModeChange();
	mTransferModeEnabled = false;

}


void HC_05Bluetooth::waitForModeChange()
{
	// wait some time for the HC-05 to catch up on mode change
	for(int i = 0; i < 100000; ++i)
		 __asm("    nop\n"
		        "    nop\n"
		        "    nop\n"
		        "    nop\n"
				"    nop\n"
				"    nop\n"
				"    nop\n"
				"    nop\n");
}

void HC_05Bluetooth::enterTransferMode()
{

	disable();
	GPIOPinWrite(mStatePortAddress,mStatePinAddress, 0);
	setPowerOn(false);

	waitForModeChange();
	//PRCMPeripheralReset(BLUETOOTH_PERIPH);
	//PRCMPeripheralClkEnable(BLUETOOTH_PERIPH, PRCM_RUN_MODE_CLK);
	// set clock speed back to 1382400
	UARTConfigSetExpClk(BLUETOOTH,BLUETUTHCLK,BLUETUTH_BAUD_RATE,
	                   (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));



	setPowerOn(true);
	waitForModeChange();
	if(BLUETOOTH_CMD_READ_HNDLE)
		vTaskDelete(BLUETOOTH_CMD_READ_HNDLE);
	xTaskCreate( BluetoothReadTask, "BLE",OSI_STACK_SIZE, NULL, 2, &BLUETOOTH_READ_HNDLE);
	enable();
	waitForModeChange();
	waitForModeChange();
	waitForModeChange();
	waitForModeChange();
	mTransferModeEnabled = true;

}


void HC_05Bluetooth::processATCommandResponse(char command[], int last_index)
{
	int j;
}



void HC_05Bluetooth::setPowerOn(bool power_on)
{
	if(power_on)
	{
		if(!mPoweredON)
		{
			GPIOPinWrite(mPowerPortAddress,mPowerPinAddress, mPowerPinAddress);
			mPoweredON = true;
		}
		else
		{
			Message("Attempted to power on an already on Bluetooth module");
		}
	}
	else
	{
		if(mPoweredON)
		{
			GPIOPinWrite(mPowerPortAddress,mPowerPinAddress, 0);
			mPoweredON = false;
		}
		else
		{
			Message("Attempted to power off an already off module");
		}
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



HC_05Bluetooth::~HC_05Bluetooth()
{
	disable();

}
