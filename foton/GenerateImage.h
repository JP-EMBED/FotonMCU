/*
 * GenerateImage.h
 *
 *  Created on: Feb 25, 2015
 *      Author: Kat
 */

#ifndef GENERATEIMAGE_H_
#define GENERATEIMAGE_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "DisplayDriver.h"

// FillXXX - start and end are used in a for loop for filling driver IMAGE_ONEBUFF
//format : (a=start; a<end; a++)
extern void FillColor (unsigned char red,unsigned char green, unsigned char blue, int start, int end, DisplayDriver * driver );
extern void FillVary (DisplayDriver * driver);
// For work with bluetooth
extern void ledSetColor(unsigned char red,unsigned char green, unsigned char blue, DisplayDriver * driver);
extern void ledSet(unsigned char row, unsigned char col, DisplayDriver * driver);
extern void ledFill(int start, int end, DisplayDriver * driver);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* GENERATEIMAGE_H_ */
