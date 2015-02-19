//*****************************************************************************
// pin_mux_config.c
//
// configure the device pins for different signals
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// This file was automatically generated on 12/19/2014 at 6:12:25 AM
// by TI PinMux version 3.0.321 

//
//*****************************************************************************

#include "pin_mux.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"
#include "prcm.h"

//*****************************************************************************
void PinMuxConfig(void)
{
	//
    // Enable Peripheral Clocks 
    //

	PRCMPeripheralClkEnable(PRCM_UARTA0, PRCM_RUN_MODE_CLK);
	PRCMPeripheralClkEnable(PRCM_UARTA1, PRCM_RUN_MODE_CLK);
	//PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
	//PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK);
	//PRCMPeripheralClkEnable(PRCM_TIMERA3, PRCM_RUN_MODE_CLK);
	//PRCMPeripheralClkEnable(PRCM_TIMERA2, PRCM_RUN_MODE_CLK);
	//PRCMPeripheralClkEnable(PRCM_TIMERA1, PRCM_RUN_MODE_CLK);



    // Configure PIN_03 for UART0 UART0_TX
    //
	//PinTypeUART(PIN_55, PIN_MODE_3);

	//
    // Configure PIN_04 for UART0 UART0_RX
    //
	//PinTypeUART(PIN_57, PIN_MODE_3);

    // Configure PIN_55 for TimerCP1 GT_CCP01
    //
	//PinTypeTimer(PIN_55, PIN_MODE_7);

	//
    // Configure PIN_50 for TimerCP0 GT_CCP00
    //
	//PinTypeTimer(PIN_50, PIN_MODE_7);

	//
    // Configure PIN_63 for SDHost0 SDCARD_IRQ
    //
	//PinTypeSDHost(PIN_63, PIN_MODE_6);

	//
    // Configure PIN_06 for SDHost0 SDCARD_DATA
    //
	//PinTypeSDHost(PIN_06, PIN_MODE_8);

	//
    // Configure PIN_07 for SDHost0 SDCARD_CLK
    //
	//PinTypeSDHost(PIN_07, PIN_MODE_8);

	//
    // Configure PIN_08 for SDHost0 SDCARD_CMD
    //
	//PinTypeSDHost(PIN_08, PIN_MODE_8);

	//
    // Configure PIN_01 for TimerPWM6 GT_PWM06
    //
	//PinTypeTimer(PIN_01, PIN_MODE_3);

	//
    // Configure PIN_64 for TimerPWM5 GT_PWM05
    //
	//PinTypeTimer(PIN_64, PIN_MODE_3);

	//
    // Configure PIN_02 for TimerPWM7 GT_PWM07
    //
	//PinTypeTimer(PIN_02, PIN_MODE_3);

	//
    // Configure PIN_21 for TimerPWM2 GT_PWM02
    //
	//PinTypeTimer(PIN_21, PIN_MODE_9);

	//
    // Configure PIN_19 for TimerPWM3 GT_PWM03
    //
	//PinTypeTimer(PIN_19, PIN_MODE_8);

	//
    // Configure PIN_17 for TimerPWM0 GT_PWM00
    //
	//PinTypeTimer(PIN_17, PIN_MODE_5);
}
