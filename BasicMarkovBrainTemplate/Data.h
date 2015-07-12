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
#include <vector>
#include <string>
#include "Genome.h"

using namespace std;

class Genome;

class Data{
public:
	//this is the LOD and general data recording part
	static int ID;
	static map<int,map<string,string> > dataMap;
	static map<string,double*> defaultDoubles;
	static map<string,int*> defaultInts;
	static map<string,string*> defaultStrings;
	static map<string,bool*> defaultBools;
	static void Add(int value,string key,Genome* to);
	static void Add(double value,string key,Genome* to);
	static string Get(string key,Genome *from);
	static vector<string> GetLODItem(string key,Genome *from);
	static void removeGenome(Genome* who);
	static int registerGenome(Genome* who);
	static void prune(int theID);
	static void prune(Genome* who);
	static void saveLOD(Genome *who,string filename);
	static void saveGEN(Genome *who,string filename,int intervall);
    static vector<Genome*> getLOD(Genome *from);
	static Genome* getMostRecentCommonAncestor(Genome *from);
	
	//this is the command line parameter part
	static map<string,int*> parameterInt;
	static map<string,double*> parameterDouble;
	static map<string,string*> parameterString;
	static map<string,bool*> parameterBool;
	static void setDefaultParameter(string key,int *variable,int value);
	static void setDefaultParameter(string key,double *variable,double value);
	static void setDefaultParameter(string key,string *variable,string value);
	static void setDefaultParameter(string key,bool *variable,bool value);
	
	static void UseCommandLineParameters(int argc, const char * argv[]);
	
	//default on the fly parameters
	static double getDefaultDouble(string S);
	static int getDefaultInt(string S);
	static string getDefaultString(string S);
	static bool getDefaultBool(string S);
	static double* makeDefaultDouble(string S);
	static int* makeDefaultInt(string S);
	static string* makeDefaultString(string S);
	static bool* makeDefaultBool(string S);
    static void setDefaultDouble(string S,double to);
    static void setDefaultInt(string S,int to);
    static void setDefaultString(string S,string to);
    static void setDefaultBool(string S,bool to);
	
	static void showAll();
};
#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
