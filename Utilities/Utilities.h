/*
 * Utilities.h
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#ifndef __BasicMarkovBrainTemplate__Utilities__
#define __BasicMarkovBrainTemplate__Utilities__

#include <sstream>

using namespace std;

inline int loopMod(int x, int y) {
	x += (y * (abs(x/y)+1));
	return x % y;
}

inline int Bit(double d) { // returns 1 if "d" is greater than 0, else return 0
	return (d > 0.0);
}

template <typename Type>
inline string mkString(Type value) {
    std::ostringstream oss;
    oss << value;
	return oss.str();
}

inline int isInSeq(const string& rawSeq, int currentValue){
	if (currentValue % 100 == 0){
		return true;
	}
	return false;
}

inline int nextInSeq(const string& rawSeq, int currentValue){
	if (currentValue < 0){
		return 0;
	}
	return (((currentValue / 25)*25) + 25);
}

#endif // __BasicMarkovBrainTemplate__Utilities__
