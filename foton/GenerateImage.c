/*
 * GenerateImage.c
 *
 *  Created on: Feb 25, 2015
 *      Author: Kat
 */


#include "GenerateImage.h"
#include "LEDBoardGPIO.h"
#include "DisplayDriver.h"

// Change global color
// red, green, blue -> color values to set RGB_COLOR to
void ledSetColor(unsigned char red,unsigned char green, unsigned char blue, DisplayDriver * driver)
{
	(*driver).GLOBAL_COLOR.blue = blue;
	(*driver).GLOBAL_COLOR.red = red;
	(*driver).GLOBAL_COLOR.green = green;
}

// Set an led to the globol color
// row, col -> position of led in array to set
void ledSet(unsigned char row, unsigned char col, DisplayDriver * driver)
{
	int pixel=0;
	// find actual position in color array
	if(row<16)
		SETP0( pixel,  row,  col);
	else
		SETP1( pixel,  row-16,  col);

	(*driver).CURRENT_DISP_IMAGE[pixel]=(*driver).GLOBAL_COLOR;
}

// fill the image with various colors
void FillVary (DisplayDriver * driver)
{
	int a;
		for (a=0; a<IMAGE_SIZE; a+=8)
		{
			FillRed(a,a+1,driver);
			FillGreen(a+1,a+2,driver);
			FillBlue(a+2,a+3,driver);
			FillWhite(a+3,a+4,driver);
			FillPurple(a+4,a+5,driver);
			FillYellow(a+5,a+6,driver);
			FillTeal(a+6,a+7,driver);
			FillPurple(a+7,a+8,driver);
		}
}

// fill with color from start pixel to end pixel
void FillColor (unsigned char red,unsigned char green, unsigned char blue,
						int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = red;
		(*driver).CURRENT_DISP_IMAGE[a].green = green;
		(*driver).CURRENT_DISP_IMAGE[a].blue = blue;
	}
}

// fill with red from start pixel to end pixel
void FillRed ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 1;
		(*driver).CURRENT_DISP_IMAGE[a].green = 0;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 0;
	}
}

// fill with blue from start pixel to end pixel
void FillBlue ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 0;
		(*driver).CURRENT_DISP_IMAGE[a].green = 0;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 1;
	}
}

// fill with green from start pixel to end pixel
void FillGreen ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 0;
		(*driver).CURRENT_DISP_IMAGE[a].green = 1;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 0;
	}
}

// fill with white from start pixel to end pixel
void FillWhite ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 1;
		(*driver).CURRENT_DISP_IMAGE[a].green = 1;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 1;
	}
}

// fill with purple from start pixel to end pixel
void FillPurple ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 1;
		(*driver).CURRENT_DISP_IMAGE[a].green = 0;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 1;
	}
}

// fill with yellow from start pixel to end pixel
void FillYellow ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 1;
		(*driver).CURRENT_DISP_IMAGE[a].green = 1;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 0;
	}
}

// fill with teal from start pixel to end pixel
void FillTeal ( int start, int end, DisplayDriver * driver )
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = 0;
		(*driver).CURRENT_DISP_IMAGE[a].green = 1;
		(*driver).CURRENT_DISP_IMAGE[a].blue = 1;
	}
}

// Fill array of colors with start image
void InitStartImage(DisplayDriver * driver)
{
	int a=0;
	int curstrt = 0;
	int curend = 8;

		// BBBB 1
		// BBBB 2
		for (a=0;a<8;a++)
		{
		FillBlue( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		}

		// BWWB 3
		// BWWB 4
		for (a=0;a<2;a++)
		{
		FillBlue( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		FillWhite( curstrt, curend, driver ); // 8-15
		Increment(curstrt,curend);
		FillWhite( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		FillBlue(curstrt,curend, driver); // 8-15
		Increment(curstrt,curend);
		}

		// BBBB 5
		// BBBB 6
		for (a=0;a<8;a++)
		{
		FillBlue( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		}

		// WWWW 7
		// WWWW 8
		for (a=0;a<4;a++)
		{
		FillWhite( curstrt, curend, driver );
		Increment(curstrt,curend);
		}

		// GGGG 9
		// GGGG 10
		for (a=0;a<20;a++)
		{
		FillGreen( curstrt, curend, driver );
		Increment(curstrt,curend);
		}

		// GRRG 11
		// GRRG 12
		//
		// GRRG 13
		// GRRG 14
		//
		for (a=0;a<8;a++)
		{
		FillGreen( curstrt, curend, driver );
		Increment(curstrt,curend);
		FillRed( curstrt, curend, driver );
		Increment(curstrt,curend);
		FillRed( curstrt, curend, driver );
		Increment(curstrt,curend);
		FillGreen( curstrt, curend, driver );
		Increment(curstrt,curend);
		}

		// GGGG 23
		// GGGG 24
		for (a=0;a<20;a++)
		{
		FillGreen( curstrt, curend, driver );
		Increment(curstrt,curend);
		}

		// WWWW 25
		// WWWW 26
		for (a=0;a<4;a++)
		{
		FillWhite( curstrt, curend, driver );
		Increment(curstrt,curend);
		}

		// BBBB 27
		// BBBB 28
		for (a=0;a<8;a++)
		{
		FillBlue( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		}

		// BWWB 29
		// BWWB 30
		for (a=0;a<2;a++)
		{
		FillBlue( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		FillWhite( curstrt, curend, driver ); // 8-15
		Increment(curstrt,curend);
		FillWhite( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		FillBlue(curstrt,curend, driver); // 8-15
		Increment(curstrt,curend);
		}

		// BBBB 31
		// BBBB 32
		for (a=0;a<8;a++)
		{
		FillBlue( curstrt, curend, driver ); // 0-7
		Increment( curstrt, curend );
		}
}
