/*
 * Random.h
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#ifndef __BasicMarkovBrainTemplate__Random__
#define __BasicMarkovBrainTemplate__Random__

#include <chrono> // including to generate random seeds
#include <random>
#include <iostream>

#include "Data.h" // DELETE ME : HERE FOR klyphcount

using namespace std;

/*
 * the Random class is a static class used to generate randomness
 * All functions in Random have 2 versions, the Main version and the Alt version
 * The Alt version exists in case a random number is needed without changing the Main output
 *
 * seed() & seedAlt()
 * set a new seed, if no argument is given, a random value is generated for the seed
 *
 * getDouble(...) & getDoubleAlt(...)
 * getInt(...) & getIntAlt(...)
 * with 2 parameters  : return a random number in the range of "first parameter" to "second parameter"
 * with 1 parameter   : or 0 to "first parameter"
 *
 * getIndex(...)&getIndexAlt(...)
 * Like getInt(...) but does not include the upper bound
 *
 * getBinomial(samples, probability) & getBinomialAlt(samples, probability)
 * returns number of successes on "sample" trials with "probability"
 *
 * P(prob) & PAlt(probability)
 * generates a value between 0 and 1 and returns true if it is < "probability"
 */
class Random {
	static mt19937 mainRandEng;
	static mt19937 altRandEng;
	static int RandSeed;

public:

	inline static void seed(const int& new_seed = RandSeed++) {
		mainRandEng.seed(new_seed);
	}
	inline static void seedAlt(const int& new_seed = RandSeed++) {
		altRandEng.seed(new_seed);
	}

//////////////////////////////////// getDOUBLE ////////////////////////////////////

	/*
	 * getDouble() gets a uniform distributed random double in range "val1" to "val2"
	 */
	inline static double getDouble(const double& val1, const double& val2) {
		uniform_real_distribution<double> dist(val1, val2);
		return dist(mainRandEng);
	}
	inline static double getDouble(const double& val2) {
		return getDouble(0, val2);
	}

	inline static double getDoubleAlt(const double& val1, const double& val2) {
		uniform_real_distribution<double> dist(val1, val2);
		return dist(altRandEng);
	}
	inline static double getDoubleAlt(const double& val2) {
		return getDoubleAlt(0, val2);
	}

//////////////////////////////////// getInt ////////////////////////////////////

	/*
	 * getDouble() gets a uniform distributed random double in range "val1" to "val2"
	 */
	inline static int getInt(const int& val1, const int& val2) {
		uniform_int_distribution<int> dist(val1, val2);
		return dist(mainRandEng);
	}
	inline static int getInt(const int& val2) {
		return getInt(0, val2);
	}

	inline static int getIntAlt(const double& val1, const double& val2) {
		uniform_int_distribution<int> dist(val1, val2);
		return dist(altRandEng);
	}
	inline static int getIntAlt(const double& val2) {
		return getIntAlt(0, val2);
	}

//////////////////////////////////// getIndex ////////////////////////////////////

	inline static int getIndex(const int& val1, const int& val2) {
		return getInt(val1, val2 - 1);
	}
	inline static int getIndex(const int& val2) {
		return getInt(0, val2 - 1);
	}

	inline static int getIndexAlt(const int& val1, const int& val2) {
		return getIntAlt(val1, val2 - 1);
	}
	inline static int getIndexAlt(const int& val2) {
		return getIntAlt(0, val2 - 1);
	}

//////////////////////////////////// binomial distribution ////////////////////////////////////

	inline static int getBinomial(const int& tests, const double& prob) {
		binomial_distribution<> dist(tests, prob);
		return dist(mainRandEng);
	}
	inline static int getBinomialAlt(const int& tests, const double& prob) {
		binomial_distribution<> dist(tests, prob);
		return dist(altRandEng);
	}

//////////////////////////////////// P ////////////////////////////////////

	inline static bool P(const double& testVal) {
		return (testVal > getDouble(1));
	}
	inline static bool PAlt(const double& testVal) {
		return (testVal > getDoubleAlt(1));
	}

};

#endif // __BasicMarkovBrainTemplate__Random__
