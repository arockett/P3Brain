#include "Global.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Utilities.h"


//global variables that should be accessible to all
int Global::update = 0;
set<int> Global::inUseGateTypes;

int& Global::repNumber = Parameters::register_parameter("repNumber", 101,
		"Replicate ID and seed (if seedWithPID not set true)", "GLOBAL");
bool& Global::seedRandom = Parameters::register_parameter("seedRandom", false,
		"if seedRandom, random number generator will seeded randomly", "GLOBAL");
int& Global::updates = Parameters::register_parameter("updates", 200, "how long the program will run", "GLOBAL");
int& Global::terminateAfter = Parameters::register_parameter("terminateAfter", 200,
		"if evolution is still running this many updates after updates, stop running and use population[0] for LOD",
		"GLOBAL");
int& Global::popSize = Parameters::register_parameter("popSize", 100, "number of genomes in the populaiton", "GLOBAL");
int& Global::dataInterval = Parameters::register_parameter("dataInterval", 1, "How often to write to data file",
		"DATA");
int& Global::genomeInterval = Parameters::register_parameter("genomeInterval", 1, "How often to write genome file",
		"DATA");
int& Global::pruneInterval = Parameters::register_parameter("pruneInterval", dataInterval,
		"How often to attempt to prune LOD and actually write out to files", "DATA");
string& Global::DataFileName = Parameters::register_parameter("dataFileName", (string) "data.csv",
		"name of genome file (stores genomes for line of decent)", "DATA");
string& Global::GenomeFileName = Parameters::register_parameter("genomeFileName", (string) "genome.csv",
		"name of data file (stores data i.e. scores", "DATA");
int& Global::bitsPerBrainAddress = Parameters::register_parameter("bitsPerBrainAddress", 10,
		"how many bits are evaluated to determine the brain addresses", "AGENT");


map<string,int> FileTracker::filesNextUpdate; // for each filename, when is it's next update?
map<string,int> FileTracker::filesLastUpdate; // for each filename, when was it last pruned?
map<string,vector<string>> FileTracker::fileColumns; // names of columns for each file
map<string,int> FileTracker::fileUpdateIntervals; // how often does this file write?


// returns true if all files next save is > "testUpdate" i.e. have we saved all the data?
int FileTracker::savedUpTo(int testUpdate) {
	if (filesNextUpdate.size() == 0) { // there are no files being tracked
		cout << "  In FileTracker::savedUpTo() : Warning... No files have been Initialized yet (this may indicate a problem...)\n";
		return false;
	}
	int savedUpTo = testUpdate + 1;
	for (auto i : filesNextUpdate) {
		savedUpTo = (i.second < savedUpTo) ? i.second : savedUpTo;
	}
	return (savedUpTo == testUpdate + 1);
}

void FileTracker::initFile(string fileName, vector<string> keys, int interval) {
	if (exists(fileName) == false) { // if file has not be initialized yet
		cout << "  Initializing file : \"" << fileName << "\".\n";
		filesLastUpdate[fileName] = -1; // it's never output anything
		filesNextUpdate[fileName] = nextInStepList(interval, filesLastUpdate[fileName]); // returns 0 - here for future Seq object
		fileUpdateIntervals[fileName] = interval;
		fileColumns[fileName] = keys;
	} else {
		cout << "  In FileTracker::initFile() :: ERROR! Attempt to initialize file \"" << fileName << "\" more then once failed.\n  Exiting!\n";
		exit(1);
	}
}

bool FileTracker::exists(string fileName){
	if (filesNextUpdate.find(fileName) == filesNextUpdate.end()) {
		return false;
	}
	return true;
}
