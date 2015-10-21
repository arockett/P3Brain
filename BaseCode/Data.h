//
//  Data.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/31/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Data__
#define __BasicMarkovBrainTemplate__Data__

#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <string>
#include <vector>

#include "Genome.h"

using namespace std;

class Genome;

class Data {
public:
	static int lastSaveUpdate; // the last time data was written to file
	static bool initFiles; // false if we have not started the data file yet

	static void saveDataOnLOD(Genome* who,int requireMRCA); // for genome, save to file data for this genome and it's LOD

	static void Add(int value, const string& key, map <string,string> & dataMap); // adds an int value to dataMap with keys "key" and Genome_id
	static void Add(double value, const string& key, map <string,string> & dataMap); // adds an double value to dataMap with keys "key" and Genome_id
	static string Get(string key, map <string,string> dataMap); //retrive the string in dataMap with keys "key" and Genome_id
};
#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
