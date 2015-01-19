/*
 * ButtonDriver.h
 *
 *  Created on: Jan 11, 2015
 *      Author: Christopher
 */

#ifndef BUTTONDRIVER_H_
#define BUTTONDRIVER_H_


#include "gpio.h"

// BUTTON FIRE_MODES
#define BUTTON_IGNORE            00  // Ignore all things related to button
#define BUTTON_ON_PRESSED        01  // On pressed (fire once until released and pressed again)
#define BUTTON_ON_RELEASED       02  // Delay until release to run
#define BUTTON_ON_BOTH           03  // Fire once on Pressed and once on Release
#define BUTTON_ON_PRESSED_X      04  // Fire after X presses (X is set in CTRL_DATA).
#define BUTTON_ON_RELEASED_X     05  // Fire after X releases (X is set in CTRL_DATA).
#define BUTTON_ON_HELD_X_SEC     06  // Fire after held for X seconds (X is set in CTRL_DATA).
#define BUTTON_ON_PULSE_AT_X     07  // Fire while held every X seconds (X is set in CTRL_DATA).

typedef struct ButtonSTATUS
{
	unsigned int  PRESSED_COUNT  : 4;
	unsigned int  RELEASED_COUNT : 4;
	unsigned int  BUTTON_STATE   : 1;
	unsigned int  HELD_COUNT     : 7;
}ButtonSTATUS;

typedef struct ButtonFireMode{
unsigned char FIRE_MODE      : 3;
unsigned char CTRL_DATA      : 5;
}ButtonFireMode;


typedef struct PinDefinition{
	unsigned char PIN_NUMBER;
	unsigned int  PORT_ADDRESS;
	unsigned char PIN_ADDRESS;
	unsigned char INT_MODE : 3;
	unsigned char INT_PORT : 5;
}PinDefinition;



class ButtonDriver {

protected:
	// place holder function pointer. Register a function to be called when a button is pressed.
	void 		      (*mButtonFunc)(const ButtonSTATUS & button_data, const bool &button_state);
	void configureFireMode(const ButtonFireMode & firemode);
    static void getPinNumber(unsigned char gpio_pin_number , unsigned int *pin_number, unsigned int *gpio_port_ptr, unsigned char *gpio_pin_ptr);
    static unsigned int gpio2Pin(unsigned char gpio_pin_number);
    static unsigned int getIntPort(unsigned long port_address)
    {
    	switch(port_address)
    	{
    		case GPIOA0_BASE: return INT_GPIOA0;
    		case GPIOA1_BASE: return INT_GPIOA1;
    		case GPIOA2_BASE: return INT_GPIOA2;
    		case GPIOA3_BASE: return INT_GPIOA3;
    		default: break;
    	}
    	return 0;
    }

    void   allocatePins(){}
public:

	ButtonDriver();
    void configureInterrupt(void (*func_ptr)(void),unsigned char edge_type);
    void disableInterrupt();
    void enableInterrupt();

	bool getButtonState(){
		unsigned long value = GPIOPinRead(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
		mStatus.BUTTON_STATE = (value == mPin.PIN_ADDRESS);
	    return mStatus.BUTTON_STATE;}

	unsigned int getFireMode()
	{
		unsigned int fire_mode(0);
		fire_mode &= mFireMode.FIRE_MODE;
		return fire_mode;
	}
	bool setPinNumber(unsigned char pin_number);
	unsigned int getReleasedCount(){
		return mStatus.RELEASED_COUNT;}

	unsigned int getHeldCount(){
		return mStatus.HELD_COUNT;}

	unsigned int getCtrlData(){
		return mFireMode.CTRL_DATA;
	}
	void pressButton(){
		getButtonState();
		mButtonFunc(mStatus, mStatus.BUTTON_STATE);}

	void registerButtonFunc(void (*func_ptr)(const ButtonSTATUS &, const bool &),
			                const ButtonFireMode & fire_mode)
	{
		mButtonFunc = func_ptr;
		configureFireMode(fire_mode);
	}

	~ButtonDriver();

	// Button configure Values
	static const unsigned char FALLING_EDGE = GPIO_FALLING_EDGE;
	static const unsigned char RISING_EDGE  = GPIO_RISING_EDGE;
	static const unsigned char BOTH_EDGES   = GPIO_BOTH_EDGES;
	ButtonSTATUS     	mStatus;
	ButtonFireMode 	    mFireMode;
	PinDefinition       mPin;
};

#endif /* BUTTONDRIVER_H_ */
