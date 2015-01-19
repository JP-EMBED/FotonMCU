//*
// * HC-05driver.c
// *
// *  Created on: Nov 21, 2014
// *      Author: Christopher
// *

#include "HC-05driver.h"

#include <stdlib.h>

#include "string.h"
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

#include "gpio_if.h"
#include "uart_if.h"
#include "udma_if.h"
#include "pin.h"

#define BLUETUTH_BAUD_RATE       1382400
#define BLUETUTHCLK              80000000
#define BLUETOOTH               UARTA1_BASE
#define BLUETOOTH_PERIPH        PRCM_UARTA1
#define SYSTICKS_PER_SECOND     100
#define BUFF_SIZE               20


//HC_05Bluetooth FotonHC_05Bluetooth;

void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < strlen(s)-1; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) >0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign >0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
}


void BlueToothInterruptHandler()
{
	//unsigned char outputval;
	//outputval = UARTIntStatus(UARTA1_BASE,0);
	//UARTIntDisable(UARTA1_BASE,UART_INT_RX);
	//char messageout[200] =  "Bluetooth Message -> ";
	//int cur_index = 21;
	//while((UARTCharsAvail(UARTA1_BASE) == true) && (cur_index < 200))
	//{
	//	messageout[cur_index++] = UARTCharGet(UARTA1_BASE);
	//}

	//messageout[199] = '\0';
	//Report(messageout);

	unsigned long ulStatus;

	//unsigned long ulMode;
	    //
	    // Read the interrupt status of the UART.
	    //
	ulStatus = MAP_UARTIntStatus(UARTA0_BASE, 1);
	MAP_UARTIntClear(UARTA1_BASE,ulStatus);
	/*UARTIntEnable(UARTA1_BASE,UART_INT_RX);

	// move to waking UARTFetch Data task

	    //
	    // Clear any pending status, even though there should be none since no UART
	    // interrupts were enabled.
	    //
	    MAP_UARTIntClear(UARTA0_BASE, ulStatus);
	    if(uiCount<6)
	    {
	    //
	    // Check the DMA control table to see if the ping-pong "A" transfer is
	    // complete.  The "A" transfer uses receive buffer "A", and the primary
	    // control structure.
	    //
	    ulMode = MAP_uDMAChannelModeGet(UDMA_CH8_UARTA0_RX | UDMA_PRI_SELECT);

	    //
	    // If the primary control structure indicates stop, that means the "A"
	    // receive buffer is done.  The uDMA controller should still be receiving
	    // data into the "B" buffer.
	    //
	    if(ulMode == UDMA_MODE_STOP)
	    {
	        //
	        // Increment a counter to indicate data was received into buffer A.
	        //
	        g_ulRxBufACount++;

	        //
	        // Set up the next transfer for the "A" buffer, using the primary
	        // control structure.  When the ongoing receive into the "B" buffer is
	        // done, the uDMA controller will switch back to this one.
	        //
	        SetupTransfer(UDMA_CH8_UARTA0_RX | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
	                        sizeof(g_ucRxBufA),UDMA_SIZE_8, UDMA_ARB_4,
	                        (void *)(UARTA0_BASE + UART_O_DR), UDMA_SRC_INC_NONE,
	                                            g_ucRxBufA, UDMA_DST_INC_8);
	    }

	    //
	    // Check the DMA control table to see if the ping-pong "B" transfer is
	    // complete.  The "B" transfer uses receive buffer "B", and the alternate
	    // control structure.
	    //
	    ulMode = MAP_uDMAChannelModeGet(UDMA_CH8_UARTA0_RX | UDMA_ALT_SELECT);

	    //
	    // If the alternate control structure indicates stop, that means the "B"
	    // receive buffer is done.  The uDMA controller should still be receiving
	    // data into the "A" buffer.
	    //
	    if(ulMode == UDMA_MODE_STOP)
	    {
	        //
	        // Increment a counter to indicate data was received into buffer A.
	        //
	        g_ulRxBufBCount++;

	        //
	        // Set up the next transfer for the "B" buffer, using the alternate
	        // control structure.  When the ongoing receive into the "A" buffer is
	        // done, the uDMA controller will switch back to this one.
	        //
	         SetupTransfer(UDMA_CH8_UARTA0_RX | UDMA_ALT_SELECT,
	                        UDMA_MODE_PINGPONG, sizeof(g_ucRxBufB),UDMA_SIZE_8,
	                        UDMA_ARB_4,(void *)(UARTA0_BASE + UART_O_DR),
	                        UDMA_SRC_INC_NONE, g_ucRxBufB, UDMA_DST_INC_8);
	    }

	    //
	    // If the UART0 DMA TX channel is disabled, that means the TX DMA transfer
	    // is done.
	    //
	    if(!MAP_uDMAChannelIsEnabled(UDMA_CH9_UARTA0_TX))
	    {
	        g_ulTxCount++;
	        //
	        // Start another DMA transfer to UART0 TX.
	        //
	        SetupTransfer(UDMA_CH9_UARTA0_TX| UDMA_PRI_SELECT, UDMA_MODE_BASIC,
	           sizeof(g_ucTxBuf),UDMA_SIZE_8, UDMA_ARB_4,g_ucTxBuf, UDMA_SRC_INC_8,
	                          (void *)(UARTA0_BASE + UART_O_DR), UDMA_DST_INC_NONE);
	        //
	        // The uDMA TX channel must be re-enabled.
	        //
	        MAP_uDMAChannelEnable(UDMA_CH9_UARTA0_TX);
	    }
	    }
	    else
	    {
	        UARTDone=1;
	        MAP_UARTIntUnregister(UARTA0_BASE);
	    }*/

}




HC_05Bluetooth::HC_05Bluetooth()
{


	// Set up TX and RX pins
	PinModeSet(PIN_01, PIN_MODE_9);	//Configure pin 9 TX
	PinDirModeSet(PIN_01, PIN_DIR_MODE_OUT);
	PinConfigSet(PIN_01,PIN_STRENGTH_4MA,PIN_TYPE_STD_PU);

	// Configure PIN_10 for UART1 UART1_RX
	PinModeSet(PIN_02, PIN_MODE_10);	//TX
	PinDirModeSet(PIN_02, PIN_DIR_MODE_IN);
	PinConfigSet(PIN_02,PIN_STRENGTH_4MA,PIN_TYPE_STD_PD);



    // Configure PIN_7 for GPIO Output
	PinModeSet(PIN_62, PIN_MODE_14);
	PinDirModeSet(PIN_62, PIN_DIR_MODE_OUT);
	PinConfigSet(PIN_62,PIN_STRENGTH_2MA,PIN_TYPE_OD_PU);

	//
    // Configure PIN_05 for GPIO Output
    //
	PinModeSet(PIN_05, PIN_MODE_15);
	PinDirModeSet(PIN_05, PIN_DIR_MODE_OUT);
	PinConfigSet(PIN_05,PIN_STRENGTH_2MA,PIN_TYPE_OD_PU);
	enterTransferMode();
	setReadMode();
	MessagesOut[0] ='\0';
	FrontIndex = 0;
	BackIndex = 0;
	MessageCount = 0;

}


void HC_05Bluetooth::configureDMATransfers()
{
	MAP_PRCMPeripheralReset(PRCM_UARTA1);
	MAP_PRCMPeripheralClkEnable(PRCM_UARTA1,PRCM_RUN_MODE_CLK);


	MAP_uDMAChannelAssign(UDMA_CH10_UARTA1_RX);
	MAP_uDMAChannelAssign(UDMA_CH11_UARTA1_TX);



	MAP_UARTIntRegister(UARTA1_BASE,BlueToothInterruptHandler);

	    //
	    // Set both the TX and RX trigger thresholds to 4.  This will be used by
	    // the uDMA controller to signal when more data should be transferred.  The
	    // uDMA TX and RX channels will be configured so that it can transfer 4
	    // bytes in a burst when the UART is ready to transfer more data.
	    //
	MAP_UARTFIFOLevelSet(UARTA1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);

	    //
	    // This register write will set the UART to operate in loopback mode.  Any
	    // data sent on the TX output will be received on the RX input.
	    //
	//HWREG(UARTA1_BASE + UART_O_CTL) |= UART_CTL_LBE;

	    //
	    // Enable the UART peripheral interrupts. uDMA controller will cause an
	    // interrupt on the UART interrupt signal when a uDMA transfer is complete.
	    //

	MAP_UARTIntEnable(UARTA1_BASE,UART_INT_DMATX);
	MAP_UARTIntEnable(UARTA1_BASE,UART_INT_DMARX);

	    //
	    // Configure the control parameters for the UART TX.  The uDMA UART TX
	    // channel is used to transfer a block of data from a buffer to the UART.
	    // The data size is 8 bits.  The source address increment is 8-bit bytes
	    // since the data is coming from a buffer.  The destination increment is
	    // none since the data is to be written to the UART data register.  The
	    // arbitration size is set to 4, which matches the UART TX FIFO trigger
	    // threshold.
	    //
	/*SetupTransfer(UDMA_CH10_UARTA1_RX | UDMA_PRI_SELECT, UDMA_MODE_PINGPONG,
	        sizeof(this->RXMessage1),UDMA_SIZE_8, UDMA_ARB_4,
	        (void *)(UARTA1_BASE + UART_O_DR), UDMA_SRC_INC_NONE,
	        this->RXMessage1, UDMA_DST_INC_8);

	SetupTransfer(UDMA_CH10_UARTA1_RX | UDMA_ALT_SELECT, UDMA_MODE_PINGPONG,
	        sizeof(this->RXMessage2),UDMA_SIZE_8, UDMA_ARB_4,
	         (void *)(UARTA1_BASE + UART_O_DR), UDMA_SRC_INC_NONE,
	         this->RXMessage2, UDMA_DST_INC_8);

	SetupTransfer(UDMA_CH11_UARTA1_TX| UDMA_PRI_SELECT,
	           UDMA_MODE_BASIC, sizeof(this->TXMessage),UDMA_SIZE_8, UDMA_ARB_4,
	           this->TXMessage, UDMA_SRC_INC_8,(void *)(UARTA1_BASE + UART_O_DR),
	                                                UDMA_DST_INC_NONE);
*/
	//MAP_UARTDMAEnable(UARTA1_BASE, UART_DMA_RX | UART_DMA_TX);
}



void HC_05Bluetooth::enable()
{
	UARTEnable(UARTA1_BASE);
}

void HC_05Bluetooth::enterConfigureMode()
{
	UARTDisable(UARTA1_BASE);

	// Enter AT Command mode signal to HC-05 (assume connected to KEY/STATE pin on HC-05)
	MAP_GPIOPinWrite(GPIOA1_BASE,KEY_PIN,1);

	// set baud to 38400
	MAP_UARTConfigSetExpClk(UARTA1_BASE,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),
	    38400, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
		UART_CONFIG_PAR_NONE));

	waitForModeChange();
}


void HC_05Bluetooth::waitForModeChange()
{
	// wait some time for the HC-05 to catch up on mode change
	asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");
	asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");asm(" nop");
}

void HC_05Bluetooth::enterTransferMode()
{
	//UARTDisable(UARTA1_BASE);
	//MAP_GPIOPinWrite(GPIOA1_BASE,KEY_PIN,0);

	// set clock speed back to 1382400
	/*MAP_UARTConfigSetExpClk(BLUETOOTH,MAP_PRCMPeripheralClockGet(BLUETOOTH_PERIPH),BLUETUTH_BAUD_RATE,
	                       (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
	                                             UART_CONFIG_PAR_NONE));*/

	waitForModeChange();
}


void HC_05Bluetooth::sendMessage(unsigned char * message,unsigned int length)
{
	unsigned int i(0);
	for(i = 0; i < length; i++)
	{
		UARTCharPut(UARTA1_BASE,message[i]);
	}
}


void HC_05Bluetooth::setReadMode()
{
	//MAP_GPIOPinWrite(GPIOA1_BASE,STATE_PIN,1);
}



void HC_05Bluetooth::setWriteMode()
{
//	GPIOPinWrite(GPIOA0_BASE,STATE_PIN,0)
}


HC_05Bluetooth::~HC_05Bluetooth()
{


}
