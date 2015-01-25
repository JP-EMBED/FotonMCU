/*
 * HC-05driver.h
 *
 *  Created on: Nov 21, 2014
 *      Author: Christopher
 */

#ifndef HC_05DRIVER_HP_
#define HC_05DRIVER_HP_



#define KEY_PIN 0x00000020
#define STATE_PIN PIN_62
#include "hw_types.h"
#include "udma.h"
#include "FreeRTOS.h"
//#include "queue.h"






extern void BlueToothInterruptHandler();





class HC_05Bluetooth
{

public:

	HC_05Bluetooth(unsigned long RX_pin,unsigned long RX_Mode,
			   unsigned long TX_pin,unsigned long TX_Mode,
			   unsigned long RTS_pin,unsigned long RTS_Mode,
			   unsigned long CONFIG_EN_PIN,unsigned long GPIO_Mode);


	void configureDMATransfers();

	void enable(void);

	void enterConfigureMode(void);
	void enterTransferMode(void);
	void sendMessage(unsigned char * message, unsigned int length);

	void setReadMode(void);

	void setWriteMode(void);

	~HC_05Bluetooth();

private:
	void waitForModeChange(void);

	unsigned short  InputDevice;
	unsigned short  OutputDevice;
	unsigned char   RXMessage1[256];
	unsigned char   RXMessage2[256];
	unsigned char   TXMessage[256];
	unsigned char*  MessagesOut[100];
	unsigned short  FrontIndex;
	unsigned short  BackIndex;
	unsigned short  MessageCount;

};





#endif /* HC_05DRIVER_HPP_ */
