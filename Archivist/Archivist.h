//
//  Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__Archivist__
#define __BasicMarkovBrainTemplate__Archivist__

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "../Brain/ClassicBrain.h"
#include "../Genome/Genome.h"
#include "../Global.h"
#include "../Organism/Organism.h"

//#include "Utilities/Parameters.h"
#include "../Utilities/Data.h"
#include "../Utilities/Utilities.h"

using namespace std;

class Archivist {
 public:

	static string& Arch_outputMethodStr;  // string parameter for outputMethod;
	////static int outputMethod;  // 0 = LODwAP (Line of Decent with Aggressive Pruning), 1 = SSwD (SnapShot with Delay)

	static int& Arch_realtimeFilesInterval;  // how often to write out data
	static bool& Arch_writeAveFile;  // if true, ave file will be created
	static bool& Arch_writeDominantFile;  // if true, dominant file will be created
	static string& Arch_AveFileName;  // name of the Averages file (ave for all brains when file is written to)
	static string& Arch_DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)
	static string& Arch_DefaultAveFileColumnNames;  // data to be saved into average file (must be values that can generate an average)

	int realtimeFilesInterval;  // how often to write out data
	bool writeAveFile;  // if true, ave file will be created
	bool writeDominantFile;  // if true, dominant file will be created
	string AveFileName;  // name of the Averages file (ave for all brains when file is written to)
	string DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)

	map<string, vector<string>> files;  // list of files (NAME,LIST OF COLUMNS)
	vector<string> DefaultAveFileColumns;  // what columns will be written into the AveFile

	bool finished;  // if finished, then as far as the archivist is concerned, we can stop the run.

	Archivist();
	virtual ~Archivist() = default;

	//save dominant and average file data
	void writeRealTimeFiles(vector<shared_ptr<Organism>> &population);

	// save data and manage in memory data
	// return true if next save will be > updates + terminate after
	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0);

};

#endif /* defined(__BasicMarkovBrainTemplate__Archivist__) */
