#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"

// TODO: REPLACE DEPENDANCY ON GPIO WITH CUSTOM GPIO DRIVER
#include "gpio.h"

#include "interrupt.h"

#include "uart_if.h"

#include "pin.h"
#include "ButtonDriver.h"
#include "prcm.h"
#include "utility_functions.h"

#include "systick.h"



// Debounce Task
void BUTTON_DEBOUNCE_TASK(void * debounce_data)
{
	DEBOUNCE_DATA * mydata = (DEBOUNCE_DATA*)debounce_data;
	vTaskDelay(10/SysTickPeriodGet());
	taskENTER_CRITICAL();

}


void BUTTON_ISR(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken;
	// clear the interrupt for all buttons
	// check all buttons at once
	unsigned long button1_state =  GPIOIntStatus(Button1_PTR->mPin.PORT_ADDRESS,1);
	unsigned long button2_state =  GPIOIntStatus(Button2_PTR->mPin.PORT_ADDRESS,1);

	// Clear Interrupt flags for both
	GPIOIntClear(Button1_PTR->mPin.INT_PORT, Button1_PTR->mPin.PIN_ADDRESS || Button2_PTR->mPin.PIN_ADDRESS);
	if(!Button1_PTR->mStatus.IS_DEBOUNCING & !((button1_state && Button1_PTR->mPin.PIN_ADDRESS) != Button1_PTR->mStatus.BUTTON_STATE )) // Check if Button one changed
	{
	//	Button1_PTR->disableInterrupt();
	//	Button1_PTR->pressButton();
	}
//	if(!(button2_state & Button1_PTR->mStatus.BUTTON_STATE)) // Check if Button two changed
//	{

//	}
}


ButtonDriver::ButtonDriver(unsigned char gpio_pin)
 : mButtonFunc(0)
{
	mStatus.BUTTON_STATE = 0;
	mStatus.RELEASED_COUNT = 0;
	mStatus.HELD_COUNT = 0;
	mStatus.PRESSED_COUNT = 0;
    setGPIOPinNumber(gpio_pin);
    buttonpress = NULL;
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


bool ButtonDriver::setGPIOPinNumber(unsigned char gpio_pin_number)
{
	if(gpio_pin_number > MAX_GPIO_NUMBER )
    	return false;
	unsigned int  port_address(0);
	unsigned int  pin_address(0);
	unsigned int pin_number(0);
	getPinNumber(gpio_pin_number,&pin_number,&port_address,&pin_address);

	if(pin_number == PIN_ERROR) // Invalid GPIO PIN NUMBER
		return false;

	// DEBGUG STATEMENT
    Report("Set Pin Number %u with port %x and pin address %x",
    		pin_number, port_address, pin_address);

    mPin.GPIO_PIN_NUM = gpio_pin_number;
    mPin.PIN_NUMBER = pin_number;
    unsigned long prcm_port(getGPIOPRCMPort(port_address));

    // intialize pin
    PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);
	PinTypeGPIO(pin_number, PIN_MODE_0,true);
	GPIODirModeSet(port_address,pin_address,GPIO_DIR_MODE_IN);

    mPin.PORT_ADDRESS = port_address;
    mPin.INT_PORT     = getIntPort(mPin.PORT_ADDRESS);
    mPin.PIN_ADDRESS  = pin_address;
    return true;
}


ButtonDriver::~ButtonDriver()
{


}
