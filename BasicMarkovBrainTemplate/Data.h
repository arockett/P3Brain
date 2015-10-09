//
//  Data.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/31/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Data__
#define __BasicMarkovBrainTemplate__Data__

#include <stdlib.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <memory>
#include "Genome.h"

using namespace std;

class Genome;

class Data {
public:

	static int klyphCount;


	static int& repNumber;
	static bool& randomSeed;
	static int& updates;
	static int& popSize;
	static int& dataInterval;
	static int& genomeInterval;
	static int& pruneInterval;
	static string& DataFileName;
	static string& GenomeFileName;
	static int& bitsPerBrainAddress; // how many sites are evaluated to determine the brain addresses


public:
	static int update;  // this is a global variable - the current "time"
	static int lastSaveUpdate; // the last time data was written to file
	static set<int> inUseGateTypes; // this is global - a list of the start codeons for gates being used
	static map<int, map<string, string> > dataMap; // contains data about genomes (genome_id->(trait_name->trait_value))
												   // i.e. (genome #1234 -> ("score" -> 101))
												   // any trait name can be used. data is added with Data::Add() (see below)

												   // file output
	static bool initFiles; // false if we have not started the data file yet

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

	//this is the command line parameter part (replaced by parameters.h/.cpp)

	//static map<string,double*> defaultDoubles;
	//static map<string,int*> defaultInts;
	//static map<string,string*> defaultStrings;
	//static map<string,bool*> defaultBools;

	//static map<string,int*> parameterInt;
	//static map<string,double*> parameterDouble;
	//static map<string,string*> parameterString;
	//static map<string,bool*> parameterBool;
	//static void setDefaultParameter(string key,int *variable,int value);
	//static void setDefaultParameter(string key,double *variable,double value);
	//static void setDefaultParameter(string key,string *variable,string value);
	//static void setDefaultParameter(string key,bool *variable,bool value);

	//static void UseCommandLineParameters(int argc, const char * argv[]);

	//default on the fly parameters
	//static double getDefaultDouble(string S);
	//static int getDefaultInt(string S);
	//static string getDefaultString(string S);
	//static bool getDefaultBool(string S);
	//static double* makeDefaultDouble(string S);
	//static int* makeDefaultInt(string S);
	//static string* makeDefaultString(string S);
	//static bool* makeDefaultBool(string S);
	//static void setDefaultDouble(string S,double to);
	//static void setDefaultInt(string S,int to);
	//static void setDefaultString(string S,string to);
	//static void setDefaultBool(string S,bool to);

};
#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
