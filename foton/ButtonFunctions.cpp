/*
 * ButtonFunctions.c
 *
 *  Created on: Mar 6, 2015
 *      Author: Kat
 */
#include "ButtonFunctions.h"
#include "ButtonDriver.h"
#include "HC-05driver.h"
#include "GenerateImage.h"

//Pretend to be off
void StandbyMode(const ButtonSTATUS & button_data, const bool &button_state)
{
	if ( FOTON_BLUETOOTH->getPower() ) // Turn on Standby
	{
		// Send message to connected devices that ledboard is shutting off
		// Sleep Bluetooth
		FOTON_BLUETOOTH->setPowerOn(0);
		// Clear led screen
		FillColor(0,0,0,0,1024, FOTON_LED_BOARD);
		// Set mode to OFF
	}
	else // Turn off Standby
	{
		// Turn bluetooth on
		FOTON_BLUETOOTH->setPowerOn(1);
		// Set mode to default mode
	}
}

// Disable Bluetooth
void BluetoothDisabled(const ButtonSTATUS & button_data, const bool &button_state)
{
	if ( FOTON_BLUETOOTH->getPower() ) // Turn on Standby
	{
		// Send message to connected devices that ledboard is shutting off
		// Sleep Bluetooth
		FOTON_BLUETOOTH->setPowerOn(0);
		// Turn off live mode if enabled
		// assert live mode disabled bit
	}
	else // Turn off Standby
	{
		// Turn bluetooth on
		FOTON_BLUETOOTH->setPowerOn(1);
		// negate live mode disabled bit
	}
}
