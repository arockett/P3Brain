//
//  Global.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/31/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Global_
#define __BasicMarkovBrainTemplate__Global_

#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <string>
#include <vector>

using namespace std;

class Global {
public:
	// parameters
	static shared_ptr<int> randomSeed;  // seed for random number generator, if -1 random number generator will be seeded randomly
	static shared_ptr<int> updates;  // run until there is a MCRA at this time
	static shared_ptr<int> popSize;  // number of genomes in the population
	static shared_ptr<string> mode;  // number of genomes in the population

	static shared_ptr<string> outputDirectory;  // where files will be written

	static shared_ptr<int> bitsPerBrainAddress;  // how many bits are evaluated to determine the brain addresses.
	// i.e. ceil(bitsPerBrainAddress/8) = number of genome sites used
	static shared_ptr<int> bitsPerCodon;

	//////////
	// Below are non configurable values (not set directly by Parameters.h methods)
	//////////
	static int update;  // the current "time"

};

#endif /* defined(__BasicMarkovBrainTemplate__Global__) */
