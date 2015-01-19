/*
 * UartDriver.h
 *
 *  Created on: Dec 31, 2014
 *      Author: Christopher
 */

#ifndef UARTDRIVER_H_
#define UARTDRIVER_H_

#include "hw_memmap.h"
#include "hw_types.h"
#include "uart.h"


class UartDriver {
public:
	UartDriver();
	virtual ~UartDriver();
};

#endif /* UARTDRIVER_H_ */
