/*
 * ButtonDriver.h
 *
 *  Created on: Jan 11, 2015
 *      Author: Christopher
 */

#ifndef BUTTONDRIVER_H_
#define BUTTONDRIVER_H_


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

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <timers.h>
// BUTTON FIRE_MODES
#define BUTTON_IGNORE            00  // Ignore all things related to button
#define BUTTON_ON_PRESSED        01  // On pressed (fire once until released and pressed again)
#define BUTTON_ON_RELEASED       02  // Delay until release to run
#define BUTTON_ON_BOTH           03  // Fire once on Pressed and once on Release
#define BUTTON_ON_PRESSED_X      04  // Fire after X presses (X is set in CTRL_DATA).
#define BUTTON_ON_RELEASED_X     05  // Fire after X releases (X is set in CTRL_DATA).
#define BUTTON_ON_HELD_X_SEC     06  // Fire after held for X seconds (X is set in CTRL_DATA).
#define BUTTON_ON_PULSE_AT_X     07  // Fire while held every X seconds (X is set in CTRL_DATA).
class ButtonDriver;
typedef struct ButtonSTATUS
{
	unsigned int  PRESSED_COUNT  : 4;
	unsigned int  RELEASED_COUNT : 4;
	unsigned int  BUTTON_STATE   : 1;
	unsigned int  HELD_COUNT     : 7;
}ButtonSTATUS;

typedef struct BUTTON_DEBOUNCE_CTRL
{
	unsigned int  IS_DEBOUNCING  : 1;
	unsigned int  DEBOUNCE_COUNT : 3;
	unsigned int  EXPECTED_STATE : 1;
	unsigned int  FIRE_MODE      : 3;
	unsigned int  CTRL_DATA      : 8;
	TaskHandle_t  BUTTON_TIMER;
	ButtonDriver *BUTTON;
	BUTTON_DEBOUNCE_CTRL()
		:IS_DEBOUNCING(false),DEBOUNCE_COUNT(0),EXPECTED_STATE(false),
		 FIRE_MODE(BUTTON_IGNORE),CTRL_DATA(0)
	{	}

}BUTTON_DEBOUNCE_CTRL;


typedef struct PinDefinition{
	unsigned int  PIN_NUMBER;
	unsigned int  PORT_ADDRESS;
	unsigned int  PIN_ADDRESS;
	unsigned int  GPIO_PIN_NUM;
	unsigned char INT_MODE : 3;
	unsigned char INT_PORT : 5;
	PinDefinition()
			:PIN_NUMBER(0),PORT_ADDRESS(0),
			 PIN_ADDRESS(0),GPIO_PIN_NUM(255),
			 INT_MODE(0),INT_PORT(0)
		{}
}PinDefinition;

// adjust this to be a single struct with debounce data for all buttons

typedef struct CALLBACK_STRUCT
{
    void (*PRESSED_CALLBACK)(short,short);
    void (*RELEASED_CALLBACK)(short,short);
}CALLBACK_DATA;




class ButtonDriver {

protected:
	// place holder function pointer. Register a function to be called when a button is pressed.
	void 		      (*mButtonFunc)(const ButtonSTATUS & button_data, const bool &button_state);

    //bool setPinNumber(unsigned char pin_number);
    bool setGPIOPinNumber(unsigned char pin_number);
public:

	ButtonDriver(unsigned char gpio_pin);
    void configureInterrupt(void (*func_ptr)(void),unsigned char edge_type);
    void disableInterrupt();
    void enableInterrupt();

	bool getButtonState(){
		unsigned long value = GPIOPinRead(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
		mStatus.BUTTON_STATE = (value == mPin.PIN_ADDRESS);
	    return mStatus.BUTTON_STATE;}
	static void configureDebounce(unsigned int button_number, const BUTTON_DEBOUNCE_CTRL & debounce_mode);
	unsigned int getFireMode()
	{
		unsigned int fire_mode(0);
		//fire_mode &= mFireMode.FIRE_MODE;
		return fire_mode;
	}


	unsigned int getReleasedCount(){
		return mStatus.RELEASED_COUNT;}

	unsigned int getHeldCount(){
		return mStatus.HELD_COUNT;}

	unsigned int getCtrlData(){
		return 0;//mFireMode.CTRL_DATA;
	}
	void pressButton(){
			mButtonFunc(mStatus, mStatus.BUTTON_STATE);
	}

	void registerButtonFunc(void (*func_ptr)(const ButtonSTATUS &, const bool &))
	{
		mButtonFunc = func_ptr;
	}

	~ButtonDriver();

	// Button configure Values
	static const unsigned char FALLING_EDGE = GPIO_FALLING_EDGE;
	static const unsigned char RISING_EDGE  = GPIO_RISING_EDGE;
	static const unsigned char BOTH_EDGES   = GPIO_BOTH_EDGES;
	ButtonSTATUS     	mStatus;
	PinDefinition       mPin;
};

extern TaskHandle_t DEBOUNCE_TSK_HNDLE;

extern ButtonDriver * Button1_PTR;
extern ButtonDriver * Button2_PTR;


extern BUTTON_DEBOUNCE_CTRL Button1_Debounce;
extern BUTTON_DEBOUNCE_CTRL Button2_Debounce;

extern void BUTTON_DEBOUNCE_TASK(void * debounce_data);
extern bool CHECK_PROCESS_FIRE(BUTTON_DEBOUNCE_CTRL * debounce_mode, ButtonSTATUS & current_status);
extern void BUTTON_ISR(void);


#endif /* BUTTONDRIVER_H_ */
