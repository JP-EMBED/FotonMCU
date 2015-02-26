/*
 * GenerateImage.h
 *
 *  Created on: Feb 25, 2015
 *      Author: Kat
 */

#ifndef GENERATEIMAGE_H_
#define GENERATEIMAGE_H_
#include "DisplayDriver.h"

extern void InitStartImage(DisplayDriver * driver);

// FillXXX - start and end are used in a for loop for filling driver IMAGE_ONEBUFF
//format : (a=start; a<end; a++)
extern void FillRed ( int start, int end, DisplayDriver * driver );
extern void FillBlue ( int start, int end, DisplayDriver * driver );
extern void FillGreen ( int start, int end , DisplayDriver * driver);
extern void FillWhite ( int start, int end, DisplayDriver * driver );
extern void FillPurple ( int start, int end, DisplayDriver * driver );
extern void FillTeal ( int start, int end, DisplayDriver * driver );
extern void FillYellow ( int start, int end, DisplayDriver * driver );

extern void FillVary (DisplayDriver * driver);

#define Increment( num, num2 ) \
	num = num + 8; \
	num2 = num2 + 8

#endif /* GENERATEIMAGE_H_ */
