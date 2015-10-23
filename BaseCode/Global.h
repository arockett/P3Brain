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

//#include "Genome.h"

using namespace std;

class Global {
public:
	// parameters
	static int& repNumber; // number used for output directory names and random seed
	static bool& seedRandom; // if this is true, seed with a random value
	static int& updates; // run until there is a MCRA at this time
	static int& terminateAfter; // if evolution is still running this many updates after updates, stop running. Use population[0] for LOD
	static int& popSize; // number of genomes in the population
	static int& dataInterval; // how often to write out data
	static int& genomeInterval; // how often to write out genomes
	static int& pruneInterval; // how often to attempt to prune the LOD
	static string& DataFileName; // name of the Data file
	static string& GenomeFileName; // name of the Genome file (genomes on LOD)
	static int& bitsPerBrainAddress; // how many bits are evaluated to determine the brain addresses.
									 // i.e. ceil(bitsPerBrainAddress/8) = number of genome sites used

	// non configurable values (not set directly by config methods)
	static int update;  // the current "time"
	static set<int> inUseGateTypes; // this is global - a list of the start codeons for gates being used
	static map<string,int> filesNextUpdate; // for each filename, when was it last updated?
	static map<string,int> filesLastUpdate; // for each filename, when was it last updated?

	static int savedUpTo(int testUpdate); // returns true if all files next save is > "testUpdate" i.e. have we saved all the data?
};

#endif /* defined(__BasicMarkovBrainTemplate__Global__) */
