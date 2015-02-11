/*
 * led.h
 *
 *  Created on: Jan 25, 2015
 *      Author: Christopher
 */

#ifndef LED_H_
#define LED_H_

#include "gpio.h"
#include "prcm.h"
#include "pin.h"
#include "utility_functions.h"
#include "ButtonDriver.h"


#define ONBOARD_LED   PinDefinition


extern ONBOARD_LED * YELLOW_LEDPTR;

extern ONBOARD_LED * RED_LEDPTR;

extern ONBOARD_LED * GREEN_LEDPTR;

static ONBOARD_LED YELLOW_LED;
static ONBOARD_LED GREEN_LED;
static ONBOARD_LED RED_LED;

ONBOARD_LED * YELLOW_LEDPTR = &YELLOW_LED;

ONBOARD_LED * RED_LEDPTR = &RED_LED;

ONBOARD_LED * GREEN_LEDPTR = &GREEN_LED;

void ConfigureLED(ONBOARD_LED &led, unsigned char ucPin)
{
    // configure LED Pin Definition
	led.GPIO_PIN_NUM = ucPin;
    led.PIN_ADDRESS = 1 << (ucPin % 8);
    led.PIN_NUMBER = GPIO2Pin(ucPin);
    led.PORT_ADDRESS = (ucPin / 8);
    led.PORT_ADDRESS = GPIO_BASE_REG[led.PORT_ADDRESS ];
}


// Sets the state of an led
static void SetLED_ON(ONBOARD_LED led, bool on_off)
{
	GPIOPinWrite(led.PORT_ADDRESS, led.PIN_ADDRESS, (on_off << (led.GPIO_PIN_NUM % PINS_PER_REG)));
}

// Reads the state of an led
static bool IsLED_ON(ONBOARD_LED  led)
{
	unsigned long led_status = MAP_GPIOPinRead(led.PORT_ADDRESS,led.PIN_ADDRESS);
	return  (led_status && led.PIN_ADDRESS);
}

// Toggles the state of the led
static void Toggle_LED(ONBOARD_LED led)
{
	SetLED_ON(led, !IsLED_ON(led));
}

static void InitializeLEDs()
{
	// Configure the LED Pin Definitions for GPIO 9, 10, and 11
	ConfigureLED(YELLOW_LED, 10);
	ConfigureLED(RED_LED,9);
	ConfigureLED(GREEN_LED, 11);

	// LEDs are all on same GPIO_PORT so only need this for one
	unsigned long prcm_port(getGPIOPRCMPort(YELLOW_LED.PORT_ADDRESS));
	PRCMPeripheralClkEnable(prcm_port, PRCM_RUN_MODE_CLK);

	// ************* Configure Each Pin ****************

	// Configure YELLOW LED Pin
	PinTypeLED(YELLOW_LED.PIN_NUMBER , PIN_MODE_0);
	GPIODirModeSet(YELLOW_LED.PORT_ADDRESS,YELLOW_LED.PIN_ADDRESS,GPIO_DIR_MODE_OUT);
	YELLOW_LED.INT_PORT = getIntPort(YELLOW_LED.PORT_ADDRESS);

	// Configure RED LED Pin
	PinTypeLED(RED_LED.PIN_NUMBER , PIN_MODE_0);
	GPIODirModeSet(RED_LED.PORT_ADDRESS,RED_LED.PIN_ADDRESS,GPIO_DIR_MODE_OUT);
	RED_LED.INT_PORT = getIntPort(RED_LED.PORT_ADDRESS);

	// Configure GREEN LED Pin
	PinTypeLED(GREEN_LED.PIN_NUMBER , PIN_MODE_0);
	GPIODirModeSet(GREEN_LED.PORT_ADDRESS,GREEN_LED.PIN_ADDRESS,GPIO_DIR_MODE_OUT);
	GREEN_LED.INT_PORT = getIntPort(GREEN_LED.PORT_ADDRESS);

	Toggle_LED(GREEN_LED);
	Toggle_LED(RED_LED);
	Toggle_LED(YELLOW_LED);
	Toggle_LED(GREEN_LED);
	Toggle_LED(RED_LED);
	Toggle_LED(YELLOW_LED);
}


#endif /* LED_H_ */
