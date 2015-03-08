/*
 * ButtonFunctions.h
 *
 *  Created on: Mar 6, 2015
 *      Author: Kat
 */

#ifndef BUTTONFUNCTIONS_H_
#define BUTTONFUNCTIONS_H_

#include "ButtonDriver.h"

extern  void StandbyMode(const ButtonSTATUS & button_data, const bool &button_state);
extern  void BluetoothDisabled(const ButtonSTATUS & button_data, const bool &button_state);

#endif /* BUTTONFUNCTIONS_H_ */
