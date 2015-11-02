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
 * (-2 % y) = (y - 2), etc.
 */
inline int loopMod(const int numerator, const int denominator) {
  return ((numerator % denominator) + denominator) % denominator;
}
/*
 * returns 1 if "d" is greater than 0, else return 0
 */
inline int Bit(double d) {
	return (d > 0.0);
}

///*
// * convert anything to a string using a string stream
// */
//template<typename Type>
//inline string mkString(Type value) {
//	std::ostringstream oss;
//	oss << value;
//	return oss.str();
//}

/*
 * this is here so we can use to string and it will work even if we give it a string as input
 */
inline string to_string(string str){
	return(str);
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
