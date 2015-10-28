/*
 * random
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#include "Random.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif


using namespace std;


mt19937 Random::mainRandEng(10);
mt19937 Random::altRandEng(10);
int Random::RandSeed = getpid();

