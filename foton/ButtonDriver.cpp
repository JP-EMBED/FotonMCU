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

static void PerformFunctionTaskButton(void * button)
{
	ButtonDriver * bdriver = reinterpret_cast<ButtonDriver * >(button);
	bdriver->pressButton();
	vTaskDelete( NULL );
}


static void BUTTON_SECOND_COUNTER_TASK(void * debounce)
{
	BUTTON_DEBOUNCE_CTRL * debounce_ctrl = reinterpret_cast<BUTTON_DEBOUNCE_CTRL * >(debounce);

	do
	{
		vTaskDelay(995 /portTICK_PERIOD_MS);
		debounce_ctrl->BUTTON->mStatus.HELD_COUNT +=1;
		if(debounce_ctrl->CTRL_DATA <= debounce_ctrl->BUTTON->mStatus.HELD_COUNT)
		{
			TaskHandle_t xHandle = NULL;
			xTaskCreate(PerformFunctionTaskButton, "", 256, reinterpret_cast<void *>(debounce_ctrl->BUTTON) , 3 , &xHandle);
			vTaskDelay(1 / portTICK_PERIOD_MS);
			if(debounce_ctrl->FIRE_MODE == BUTTON_ON_PULSE_AT_X)
				debounce_ctrl->BUTTON->mStatus.HELD_COUNT = 0;
		}
	}while(debounce_ctrl->CTRL_DATA > debounce_ctrl->BUTTON->mStatus.HELD_COUNT);
	debounce_ctrl->BUTTON->mStatus.HELD_COUNT = 0;
	vTaskDelete( NULL );

}




// Debounce Task
void BUTTON_DEBOUNCE_TASK(void * debounce_data)
{

    for( ;; )
    {
		if(Button1_Debounce.IS_DEBOUNCING)
		{
			if(Button1_Debounce.DEBOUNCE_COUNT == DEBOUNCED)
			{
				if(Button1_Debounce.EXPECTED_STATE == Button1_PTR->getButtonState())
				{

					if(CHECK_PROCESS_FIRE(&Button1_Debounce,Button1_PTR->mStatus))
					{
						// call the function from another task (could be long blocking)
						TaskHandle_t xHandle = NULL;
						xTaskCreate(PerformFunctionTaskButton, "", OSI_STACK_SIZE, reinterpret_cast<void *>(Button1_PTR) , 3 , &xHandle);
					}
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
					// call the function from another task (could be long blocking)
				    TaskHandle_t xHandle = NULL;
				    if(CHECK_PROCESS_FIRE(&Button2_Debounce,Button2_PTR->mStatus))
				    	xTaskCreate(PerformFunctionTaskButton, "", OSI_STACK_SIZE, reinterpret_cast<void *>(Button2_PTR) , 3 , &xHandle);
				}
			    Button2_Debounce.IS_DEBOUNCING = false;
			    Button2_Debounce.DEBOUNCE_COUNT = 0;
			}
			Button2_Debounce.DEBOUNCE_COUNT += 1;
		}

		vTaskDelay(10 * portTICK_PERIOD_MS); // sleep 10 milliseconds
	}
}


bool CHECK_PROCESS_FIRE(BUTTON_DEBOUNCE_CTRL * debounce_mode, ButtonSTATUS & current_status)
{
	switch(debounce_mode->FIRE_MODE)
	{
		case BUTTON_IGNORE:
		default :
		{
			break;
		}
		case BUTTON_ON_PRESSED:
		{
			return current_status.BUTTON_STATE == 1;

		}
		case BUTTON_ON_RELEASED:
		{
			return current_status.BUTTON_STATE == 0;
		}
		case BUTTON_ON_BOTH:
		{
			return true;
		}
		case BUTTON_ON_PRESSED_X:
		{
			if(current_status.BUTTON_STATE)
				current_status.PRESSED_COUNT += 1;
			if(current_status.PRESSED_COUNT == debounce_mode->CTRL_DATA)
			{
				current_status.PRESSED_COUNT = 0;
				return true;
			}
			break;
		}
		case BUTTON_ON_RELEASED_X:
		{
			if(!current_status.BUTTON_STATE)
				current_status.RELEASED_COUNT += 1;
			if(current_status.RELEASED_COUNT == debounce_mode->CTRL_DATA)
			{
				current_status.RELEASED_COUNT = 0;
				return true;
			}
			break;
		}
		case BUTTON_ON_PULSE_AT_X:
		case BUTTON_ON_HELD_X_SEC:
		{
			if(!current_status.BUTTON_STATE)
			{
				current_status.HELD_COUNT = 0;
				if(debounce_mode->BUTTON_TIMER != NULL)
				{
					vTaskDelete(debounce_mode->BUTTON_TIMER);
				}
			}
			else
			{
				debounce_mode->BUTTON_TIMER = NULL;
				xTaskCreate(BUTTON_SECOND_COUNTER_TASK, "", OSI_STACK_SIZE, reinterpret_cast<void *>(debounce_mode) , 3 , &debounce_mode->BUTTON_TIMER);
			}
			break;

		}
	}
	return false;
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




void ButtonDriver::configureDebounce(unsigned int button_number, const BUTTON_DEBOUNCE_CTRL & debounce_mode)
{
	if(button_number == 1)
	{
		Button1_Debounce = debounce_mode;
		Button1_Debounce.BUTTON = Button1_PTR;
	}
	else if(button_number == 2)
	{
		Button2_Debounce = debounce_mode;
		Button2_Debounce.BUTTON = Button2_PTR;
	}
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
	unsigned char  pin_address(0);
	unsigned char pin_number(0);
	getPinNumber(gpio_pin_number,&pin_number,&port_address,&pin_address);

	if(pin_number == PIN_ERROR) // Invalid GPIO PIN NUMBER
		return false;

	// DEBGUG STATEMENT
    //Report("Set Pin Number %u with port %x and pin address %x",
    //		pin_number, port_address, pin_address);

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
