/*
 * FotonCommDevice.h
 *
 *  Created on: Mar 1, 2015
 *      Author: Christopher
 */


#ifndef FOTONCOMMDEVICE_H_
#define FOTONCOMMDEVICE_H_



/* A Foton Comm Device sets the base foundation for Foton Message Communication.
 * Classes should inherit this interface to derive a concrete class upon a physical peripheral.
 */
class FotonCommDevice {

public:
	FotonCommDevice();
	virtual ~FotonCommDevice();
	virtual void configureInterrupts();
	virtual void enableInterrupts();
	virtual void disableInterrupts();
	virtual void configureDMALiveMode();
	virtual void configureDMATransferMode();

};

#endif /* FOTONCOMMDEVICE_H_ */
