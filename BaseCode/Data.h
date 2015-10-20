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

	static map<int, map<string, string> > dataMap; // contains data about genomes (genome_id->(trait_name->trait_value))
												   // i.e. (genome #1234 -> ("score" -> 101))
												   // any trait name can be used. data is added with Data::Add() (see below)

												   // file output

	static void saveDataOnLOD(Genome* who); // for genome, save to file data for this genome and it's LOD

	// LOD tracking
	static vector<Genome*> getLOD(Genome* from); // return a list of genomes containing genome and all genomes ancestors ordered oldest first
	static Genome* getMostRecentCommonAncestor(Genome* from);
	static void showAll(); // show all data in dataMap (for testing)

	//data recording

	static void Add(int value, const string& key, map <string,string> & dataMap); // adds an int value to dataMap with keys "key" and Genome_id
	static void Add(double value, const string& key, map <string,string> & dataMap); // adds an double value to dataMap with keys "key" and Genome_id
	static string Get(string key, map <string,string> dataMap); //retrive the sting in dataMap with keys "key" and Genome_id
	static vector<string> GetLODItem(string key, Genome* from); // returns a list of strings for "key" for all ancestors of Genome
};
#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
