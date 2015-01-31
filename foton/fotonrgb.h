/************************************************************************
* file: fotonrgb.h
* author: Kat Derby
* Revision: 1.0
* Last Revision Date: 1/28/2015
*
* Struct: FOTON_RGB
*		- containts 3 bytes of data
*			- each byte holds information for 1 color ( red, green, blue )
*			- this information is used in binary coded modulation
*
* Typedef: FOTON_COLOR
*		- unsigned char which equates to a byte of data
* 
*************************************************************************/

#ifndef FOTONRGB_H_
#define FOTONRGB_H_

typedef  unsigned char FOTON_COLOR;

typedef  struct FOTON_RGB
{
    FOTON_COLOR red;
    FOTON_COLOR green;
    FOTON_COLOR blue;
	
}FOTON_RGB;

#endif /* FOTONRGB_H_ */
