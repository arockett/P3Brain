/*
 * Utilities.h
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#ifndef __BasicMarkovBrainTemplate__Utilities__
#define __BasicMarkovBrainTemplate__Utilities__

#include <algorithm>
#include <sstream>

using namespace std;

/*
 * return x % y were (-1 % y) = (y - 1)
 */
inline int loopMod(int x, int y) {
	x += (y * (abs(x / y) + 1)); // this adds a number of 'y's to 'x' so as to insure that 'x' is greater then 0. the numer of 'y's is not important.
	return x % y;
}

/*
 * returns 1 if "d" is greater than 0, else return 0
 */
inline int Bit(double d) {
	return (d > 0.0);
}

/*
 * convert anything to a string using a string stream
 */
template<typename Type>
inline string mkString(Type value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

/*
 * getBestInVector(vector<T> vec)
 * given W (a list of scores), return the index of the highest score
 * if more then one genome has max score, return the first one.
 */
template<typename Type>
inline int findGreatestInVector(vector<Type> vec) {
		return distance(vec.begin(), max_element (vec.begin(),vec.end()));
}

#endif // __BasicMarkovBrainTemplate__Utilities__
