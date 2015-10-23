#include "Global.h"
#include "../Utilities/Parameters.h"

//global variables that should be accessible to all
int Global::update = 0;
set<int> Global::inUseGateTypes;
map<string, int> Global::filesNextUpdate; // for each filename, when was it will next updat? NOTE: related to Data::dataFilesCreated which tracks existence of files
map<string, int> Global::filesLastUpdate; // for each filename, when was it last updated? NOTE: related to Data::dataFilesCreated which tracks existence of files

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

// returns true if all files next save is > "testUpdate" i.e. have we saved all the data?
int Global::savedUpTo(int testUpdate) {
	if (filesNextUpdate.size() == 0){
		return false;
		cout << "AAA\n";
	}
	int savedUpTo = testUpdate+1;
	for (auto i : filesNextUpdate) {
		savedUpTo = (i.second < savedUpTo) ? i.second : savedUpTo;
		cout << "AAA    " << i.first << " " << i.second << "\n";
	}
	cout << savedUpTo << " == " << testUpdate+1 << "\n";
	return (savedUpTo == testUpdate+1);
}
