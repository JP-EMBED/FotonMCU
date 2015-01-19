#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"

#include "gpio.h"

#include "interrupt.h"

#include "uart_if.h"
#include "button_if.h"
#include "pin.h"
#include "ButtonDriver.h"



static unsigned long GPIO_BASE_REG[]=
{
    GPIOA0_BASE,
    GPIOA1_BASE,
    GPIOA2_BASE,
    GPIOA3_BASE,
    GPIOA4_BASE
};



#define PINS_PER_REG 8
#define MAX_PIN_NUMBER 19
ButtonDriver::ButtonDriver()
 : mButtonFunc(0)
{
	mStatus.BUTTON_STATE = 0;
	mStatus.RELEASED_COUNT = 0;
	mStatus.HELD_COUNT = 0;
	mStatus.PRESSED_COUNT = 0;


}




void ButtonDriver::configureFireMode(const ButtonFireMode & firemode)
{


}

void ButtonDriver::configureInterrupt(void (*func_ptr)(void),unsigned char edge_type)
{
    mPin.INT_MODE = edge_type;
	GPIOIntTypeSet(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS,mPin.INT_MODE);
	GPIOIntRegister(mPin.PORT_ADDRESS,func_ptr);
	GPIOIntClear(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
	GPIOIntEnable(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
}

void ButtonDriver::enableInterrupt()
{
	GPIOIntClear(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
	IntPendClear(mPin.INT_PORT);
	IntEnable(mPin.INT_PORT);
	GPIOIntEnable(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
}

void ButtonDriver::disableInterrupt()
{
	GPIOIntDisable(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
	GPIOIntClear(mPin.PORT_ADDRESS,mPin.PIN_ADDRESS);
	IntDisable(mPin.INT_PORT);
}


void ButtonDriver::getPinNumber(unsigned char gpio_pin_number , unsigned int *pin_number, unsigned int *gpio_port_ptr, unsigned char *gpio_pin_ptr)
{
	// Get the GPIO pin from the external Pin number
	*gpio_pin_ptr = 1 << (gpio_pin_number % PINS_PER_REG);
    *pin_number = gpio2Pin(gpio_pin_number);
	// Get the GPIO port from the external Pin number
	*gpio_port_ptr = (gpio_pin_number / PINS_PER_REG);
	*gpio_port_ptr = GPIO_BASE_REG[*gpio_port_ptr];
}

unsigned int ButtonDriver::gpio2Pin(unsigned char gpio_pin_number)
{
	switch(gpio_pin_number)
	{
		case  13: return PIN_04;
		default : return 0;
	}
}


bool ButtonDriver::setPinNumber(unsigned char gpio_pin_number)
{
	if(gpio_pin_number > MAX_PIN_NUMBER )
    	return false;
	unsigned int  port_address(0);
	unsigned char pin_address(0);
	unsigned int pin_number(0);
	getPinNumber(gpio_pin_number,&pin_number,&port_address,&pin_address);

    Report("Set Pin Number %u with port %x and pin address %x",
    		pin_number, port_address, pin_address);
    mPin.PIN_NUMBER = pin_number;
	PinTypeGPIO(pin_number, PIN_MODE_0,true);
	GPIODirModeSet(port_address,pin_address,GPIO_DIR_MODE_IN);
	//PinModeSet(pin_number, PIN_MODE_0);
   // PinDirModeSet(pin_number, PIN_DIR_MODE_IN);
	//PinConfigSet(pin_number,PIN_STRENGTH_2MA,PIN_TYPE_OD_PD);
    mPin.PORT_ADDRESS = port_address;
    mPin.INT_PORT     = getIntPort(mPin.PORT_ADDRESS);
    mPin.PIN_ADDRESS  = pin_address;



    return true;
}


ButtonDriver::~ButtonDriver()
{


}
