/************************************************************************
* file: GenerateImage.c
* author: Kat Derby
* Revision: 2.0
* Last Revision Date: 3/1/2015
*
* Defined Functions:
*		- Defined functions that allow for generation of images
*
*		void ledSetColor(unsigned char red,unsigned char green, unsigned char blue, DisplayDriver * driver)
*			sets the color of the GlobalColor
*		void ledSet(unsigned char row, unsigned char col, DisplayDriver * driver)
*			sets an led in the array being currently displayed to the GlobalColor
*		void ledFill(int start, int end, DisplayDriver * driver)
*			sets a line of colors in the array to the GlobalColor
*			this line goes from start to end-1
*
*		FillColor (unsigned char red,unsigned char green, unsigned char blue,
*						int start, int end, DisplayDriver * driver )
*			fill with specified color from start pixel to end pixel
*
*
*************************************************************************/
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
	pixel = row *32 + col;
	(*driver).CURRENT_DISP_IMAGE[pixel]=(*driver).GLOBAL_COLOR;
}

// Sets a line of colors in the array to the GlobalColor
// This line goes from start to end-1
void ledFill(int start, int end, DisplayDriver * driver)
{
	int a;
	for (a=start; a<end; a++)
	{
		(*driver).CURRENT_DISP_IMAGE[a].red = (*driver).GLOBAL_COLOR.red;
		(*driver).CURRENT_DISP_IMAGE[a].green = (*driver).GLOBAL_COLOR.green;
		(*driver).CURRENT_DISP_IMAGE[a].blue = (*driver).GLOBAL_COLOR.blue;
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
