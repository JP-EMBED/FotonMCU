/************************************************************************
* file: LEDBOARDGPIO.h
* author: Kat Derby
* Revision: 3.8
* Last Revision Date: 2/18/2015
*
* Defined Values: 
*		- Defined Values that make using macros and the GPIO for the
*			LED board signals easier
* 
*************************************************************************/

#ifndef LEDBOARDGPIO_H_
#define LEDBOARDGPIO_H_

#include <inc/hw_types.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin.h>
#include <driverlib/gpio.h>
#include <driverlib/prcm.h>
#include "fotonrgb.h"

#define IMAGE_SIZE 1024

// bit packed pin representation for each signal
// Control Signals
#define LATCH_PIN 0x01
#define BLANK_PIN 0x04
#define CLK_PIN 0x08

// Color Coding Signals
// Top Rows
#define R0_PIN 0x40
#define G0_PIN 0x10
#define B0_PIN 0x20

// Bottom Rows
#define R1_PIN 0x20
#define G1_PIN 0x40
#define B1_PIN 0x80

// Address Signals
#define D_PIN 0x10
#define A_PIN 0x80
#define B_PIN 0x40
#define C_PIN 0x40


// Ports for each signal
// Control Signals
#define LATCH_PORT GPIOA0_BASE
#define BLANK_PORT GPIOA1_BASE
#define CLK_PORT GPIOA1_BASE
#define PWM_PORT TIMERA3_BASE

// Color Coding Signals
// Top Rows
#define R0_PORT GPIOA1_BASE
#define G0_PORT GPIOA1_BASE
#define B0_PORT GPIOA1_BASE

// Bottom Rows
#define R1_PORT GPIOA0_BASE
#define G1_PORT GPIOA0_BASE
#define B1_PORT GPIOA1_BASE

// Address Signals
#define A_PORT GPIOA0_BASE
#define D_PORT GPIOA0_BASE
#define B_PORT GPIOA3_BASE
#define C_PORT GPIOA2_BASE

// masks to find value for pin in address
#define A_MASK 0x8
#define B_MASK 0x4
#define C_MASK 0x2
#define D_MASK 0x1

// Clock Values
#define CLK_TIMER TIMER_B
#define PWM TIMER TIMER_A
#define SHIFT_CLK_RELOAD 96
#define TIMER_INTERVAL_RELOAD   24576
#define BASE_DELAY_TIME 192


#endif /* LEDBOARDGPIO_H_ */
