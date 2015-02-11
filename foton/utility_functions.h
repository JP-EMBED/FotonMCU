/*
 * utility_functions.h
 *
 *  Created on: Jan 19, 2015
 *      Author: Christopher
 */

#ifndef UTILITY_FUNCTIONS_H_
#define UTILITY_FUNCTIONS_H_

#include "pin.h"
#include "prcm.h"
#include "hw_ints.h"
#include <stdlib.h>
#include "string.h"

#define PIN_ERROR  999999
#define PINS_PER_REG 8
#define MAX_GPIO_NUMBER 32
#define MAX_PIN_NUMBER  64


static unsigned char PinToPadMap[64] =
{
	10,11,12,13,14,15,16,17,255,255,18,
	19,20,21,22,23,24,40,28,29,25,255,
	255,255,255,255,255,255,255,255,255,255,255,
	255,255,255,255,255,255,255,255,255,255,255,
	31,255,255,255,255,0,255,32,30,255,1,
	255,2,3,4,5,6,7,8,9
};


static void INITIALIZE_UTILITIES()
{
	//g_ulPinToPadMap = PinToPadMap;
}


static unsigned long GPIO_BASE_REG[]=
{
    GPIOA0_BASE,
    GPIOA1_BASE,
    GPIOA2_BASE,
    GPIOA3_BASE,
    GPIOA4_BASE
};


// Pins defined to be used by our device
static unsigned int GPIO2Pin(unsigned char gpio_pin_number)
{
	switch(gpio_pin_number)
	{
	    case   9: return PIN_64;
		case  10: return PIN_01;
		case  11: return PIN_02;
		case  13: return PIN_04;
		case  22: return PIN_15;
		case  28: return PIN_18;
		case   3: return PIN_58;
		default : return PIN_ERROR;
	}
}

static unsigned int Pin2GPIO(unsigned int pin_number)
{
	if(pin_number >0 && pin_number <= 64 )
		return PinToPadMap[pin_number-1];
	return 255;
}

static void PinTypeLED(unsigned long ulPin,unsigned long ulPinMode)
{
	PinConfigSet(ulPin, PIN_STRENGTH_2MA, PIN_TYPE_STD);
	PinModeSet(ulPin,ulPinMode);
}


static void getPinNumber(unsigned int gpio_pin_number , unsigned int *pin_number, unsigned long  *gpio_port_ptr, unsigned long *gpio_pin_ptr)
{
	// Get the GPIO pin from the external Pin number
	*pin_number = GPIO2Pin(gpio_pin_number);
	if(*pin_number == PIN_ERROR)
		return;
	*gpio_pin_ptr = 1 << (gpio_pin_number % PINS_PER_REG);

	// Get the GPIO port from the external Pin number
	*gpio_port_ptr = (gpio_pin_number / PINS_PER_REG);
	*gpio_port_ptr = GPIO_BASE_REG[*gpio_port_ptr];
}


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

static unsigned int getGPIOPRCMPort(unsigned long gpio_port_address)
{
   	switch(gpio_port_address)
   	{
   		case GPIOA0_BASE: return PRCM_GPIOA0;
   		case GPIOA1_BASE: return PRCM_GPIOA1;
   		case GPIOA2_BASE: return PRCM_GPIOA2;
   		case GPIOA3_BASE: return PRCM_GPIOA3;
   		default: break;
   	}
   	return 0;
}

static void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i < strlen(s)-1; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


static void itoa(int n, char s[])
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




#endif /* UTILITY_FUNCTIONS_H_ */
