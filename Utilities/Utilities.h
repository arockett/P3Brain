/*
 * Utilities.h
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#ifndef __BasicMarkovBrainTemplate__Utilities__
#define __BasicMarkovBrainTemplate__Utilities__

inline int loopMod(int x, int y) {
	x += (y * (abs(x/y)+1));
	return x % y;
}

inline int Bit(double d) { // returns 1 if "d" is greater than 0, else return 0
	return (d > 0.0);
}


#endif // __BasicMarkovBrainTemplate__Utilities__
