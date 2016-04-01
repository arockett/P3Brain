//
//  Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__DefaultArchivist__
#define __BasicMarkovBrainTemplate__DefaultArchivist__

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "../Global.h"
#include "../Organism/Organism.h"

using namespace std;

class DefaultArchivist {
 public:

	static const string& Arch_outputMethodStr;  // string parameter for outputMethod;

	static const int& Arch_realtimeFilesInterval;  // how often to write out data
	static const bool& Arch_writeAveFile;  // if true, ave file will be created
	static const bool& Arch_writeDominantFile;  // if true, dominant file will be created
	static const string& Arch_AveFileName;  // name of the Averages file (ave for all brains when file is written to)
	static const string& Arch_DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)
	static const string& Arch_DefaultAveFileColumnNames;  // data to be saved into average file (must be values that can generate an average)
	static const bool& Arch_DominantFileShowAllLists;

	int realtimeFilesInterval;  // how often to write out data
	bool writeAveFile;  // if true, ave file will be created
	bool writeDominantFile;  // if true, dominant file will be created
	string AveFileName;  // name of the Averages file (ave for all brains when file is written to)
	string DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)
	map<string, vector<string>> files;  // list of files (NAME,LIST OF COLUMNS)
	vector<string> DefaultAveFileColumns;  // what columns will be written into the AveFile

	bool finished;  // if finished, then as far as the archivist is concerned, we can stop the run.

	DefaultArchivist();
	DefaultArchivist(vector<string> aveFileColumns);
	virtual ~DefaultArchivist() = default;

	//save dominant and average file data
	void writeRealTimeFiles(vector<shared_ptr<Organism>> &population);

	// save data and manage in memory data
	// return true if next save will be > updates + terminate after
	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0);

};

#endif /* defined(__BasicMarkovBrainTemplate__DefaultArchivist__) */
