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
#include "FreeRTOS.h"
#include "task.h"

#define IS_MASKED  1
#define DEBOUNCED  3

// Debounce Task
void BUTTON_DEBOUNCE_TASK(void * debounce_data)
{
    // TODO: Add debounce task initialization
	//ButtonDriver * button1 = (ButtonDriver *)debounce_data;
    for( ;; )
    {
		if(Button1_Debounce.IS_DEBOUNCING)
		{
			if(Button1_Debounce.DEBOUNCE_COUNT == DEBOUNCED)
			{
				if(Button1_Debounce.EXPECTED_STATE == Button1_PTR->getButtonState())
				{
					Button1_PTR->pressButton();
				}
				else
				{
					//Report("Debounce failed Pin Number %u with port %x and pin address %x: Expected %u and got %u",
					//		Button1_PTR->mPin.PIN_NUMBER, Button1_PTR->mPin.PORT_ADDRESS ,
					//		Button1_PTR->mPin.PIN_ADDRESS, Button1_Debounce.EXPECTED_STATE,
					//		Button1_PTR->mStatus.BUTTON_STATE);
				}
				Button1_Debounce.IS_DEBOUNCING = false;
				Button1_Debounce.DEBOUNCE_COUNT = 0;
			}
			Button1_Debounce.DEBOUNCE_COUNT += 1;
		}
		if(Button2_Debounce.IS_DEBOUNCING)
		{
			if(Button2_Debounce.DEBOUNCE_COUNT == DEBOUNCED)
			{
				if(Button2_Debounce.EXPECTED_STATE == Button2_PTR->getButtonState())
				{
					Button2_PTR->pressButton();
				}
				else
				{
					/*Report("Debounce failed Pin Number %u with port %x and pin address %x: Expected %u and got %u",
							Button2_PTR->mPin.PIN_NUMBER, Button2_PTR->mPin.PORT_ADDRESS ,
							Button2_PTR->mPin.PIN_ADDRESS, Button2_Debounce.EXPECTED_STATE,
							Button2_PTR->mStatus.BUTTON_STATE);*/
				}
			    Button2_Debounce.IS_DEBOUNCING = false;
			    Button2_Debounce.DEBOUNCE_COUNT = 0;
			}
			Button2_Debounce.DEBOUNCE_COUNT += 1;
		}

		vTaskDelay(10 * portTICK_PERIOD_MS); // sleep 10 milliseconds
	}
}


void BUTTON_ISR(void)
{

	unsigned long button1_int =  GPIOIntStatus(Button1_PTR->mPin.PORT_ADDRESS,0);
	unsigned long button2_int =  GPIOIntStatus(Button2_PTR->mPin.PORT_ADDRESS,0);

	unsigned char button_state;
	GPIOIntClear(Button1_PTR->mPin.PORT_ADDRESS,Button1_PTR->mPin.PIN_ADDRESS);
	GPIOIntClear(Button2_PTR->mPin.PORT_ADDRESS,Button2_PTR->mPin.PIN_ADDRESS);
	// Check if Button one is debouncing.
	if(!Button1_Debounce.IS_DEBOUNCING && (button1_int && Button1_PTR->mPin.PIN_ADDRESS))
	{
		//Button1_PTR->disableInterrupt();
		button_state = GPIOPinRead(Button1_PTR->mPin.PORT_ADDRESS,Button1_PTR->mPin.PIN_ADDRESS);
		// if not debouncing, set debouncing true if the current state doesnt not match stored state
		Button1_Debounce.IS_DEBOUNCING = ((button_state && Button1_PTR->mPin.PIN_ADDRESS )  ^ Button1_PTR->mStatus.BUTTON_STATE );
		//if(Button1_Debounce.IS_DEBOUNCING)
		Button1_Debounce.EXPECTED_STATE = (button_state && Button1_PTR->mPin.PIN_ADDRESS);
		//Button1_PTR->enableInterrupt();
	}

	if(!Button2_Debounce.IS_DEBOUNCING && (button2_int && Button2_PTR->mPin.PIN_ADDRESS))
	{
		//Button2_PTR->disableInterrupt();
		button_state = GPIOPinRead(Button2_PTR->mPin.PORT_ADDRESS,Button2_PTR->mPin.PIN_ADDRESS);
			// if not debouncing, set debouncing true if the current state doesnt not match stored state
		Button2_Debounce.IS_DEBOUNCING = ((button_state && Button2_PTR->mPin.PIN_ADDRESS) ^ Button2_PTR->mStatus.BUTTON_STATE );
		//if(Button2_Debounce.IS_DEBOUNCING)
		Button2_Debounce.EXPECTED_STATE = (button_state && Button2_PTR->mPin.PIN_ADDRESS);
		//Button2_PTR->enableInterrupt();
	}


}


ButtonDriver::ButtonDriver(unsigned char gpio_pin)
 : mButtonFunc(0)
{
	mStatus.BUTTON_STATE = false;
	mStatus.RELEASED_COUNT = 0;
	mStatus.HELD_COUNT = 0;
	mStatus.PRESSED_COUNT = 0;
    setGPIOPinNumber(gpio_pin);
}




void ButtonDriver::configureDebounce(const BUTTON_DEBOUNCE_CTRL & debounce_mode)
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
	IntDisable(mPin.INT_PORT);

}


bool ButtonDriver::setGPIOPinNumber(unsigned char gpio_pin_number)
{
	if(gpio_pin_number > MAX_GPIO_NUMBER )
    	return false;
	unsigned long  port_address(0);
	unsigned long  pin_address(0);
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
