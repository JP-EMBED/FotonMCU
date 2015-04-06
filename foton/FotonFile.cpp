/*
 * FotonFile.cpp
 *
 *  Created on: Apr 5, 2015
 *      Author: Christopher
 */

#include <hw_types.h>
#include <hw_memmap.h>
#include <hw_ints.h>
#include <gpio.h>

#include <interrupt.h>
#include <pin.h>
#include <prcm.h>

#include <sdhost.h>

#include "FotonFile.h"

void initializeSystemMemory()
{
	// initialize SDCard Pins
	PRCMPeripheralClkEnable(PRCM_SDHOST, PRCM_RUN_MODE_CLK);

	// Configure PIN_06 for SDHOST0 SDHost_D0
	PinTypeSDHost(PIN_64, PIN_MODE_6);

	// Configure PIN_07 for SDHOST0 SDHost_CLK
	PinTypeSDHost(PIN_01, PIN_MODE_8);

	// Configure PIN_08 for SDHOST0 SDHost_CMD
	PinTypeSDHost(PIN_02, PIN_MODE_6);

    // Set the SD card clock as output pin
    PinDirModeSet(PIN_01,PIN_DIR_MODE_OUT);

    // Enable Pull up on data
    PinConfigSet(PIN_64,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);

    // Enable Pull up on CMD
    PinConfigSet(PIN_02,PIN_STRENGTH_4MA, PIN_TYPE_STD_PU);

    // Enable MMCHS
    PRCMPeripheralClkEnable(PRCM_SDHOST,PRCM_RUN_MODE_CLK);

    // Reset MMCHS
    PRCMPeripheralReset(PRCM_SDHOST);

    // Configure MMCHS
    //SDHostInit(SDHOST_BASE);

    // Configure card clock
    //SDHostSetExpClk(SDHOST_BASE,  PRCMPeripheralClockGet(PRCM_SDHOST),15000000);


}

FotonFile::FotonFile()
{


}


bool FotonFile::open(char * file_path_name, StreamType stream_direction )
{
	return false;
}



long FotonFile::readFor(char** buffer, long read_length)
{
	return 0;
}

char ** FotonFile::readAll()
{
	return 0;
}

bool FotonFile::write(char ** data_out) // null terminated string
{
	return false;
}

bool FotonFile::writeFor(char ** data_out, long length)
{
	return false;
}

bool FotonFile::seekTo(long index)
{
	return false;
}

bool FotonFile::close()
{
	return false;
}

FotonFile::~FotonFile() {
	// TODO Auto-generated destructor stub
}

