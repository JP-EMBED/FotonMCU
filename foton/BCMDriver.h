/*
 * BCMDriver.hpp
 *
 *  Created on: Feb 23, 2015
 *      Author: Christopher
 */

#ifndef BCMDRIVER_HPP_
#define BCMDRIVER_HPP_

// Define PINS

class GPIOPin
{
	public:
	bool mIsON;
};

extern GPIOPin * RED0_PTR;
extern GPIOPin * GREEN0_PTR;
extern GPIOPin * BLUE0_PTR;

extern GPIOPin * RED1_PTR;
extern GPIOPin * GREEN1_PTR;
extern GPIOPin * BLUE1_PTR;

#define PIN_COUNT  6



extern void BCMDriverTask(void *);


#endif /* BCMDRIVER_HPP_ */
