#include "Global.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Utilities.h"


//global variables that should be accessible to all
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

int Global::update = 0;
set<int> Global::inUseGateTypes;

int Global::lastPrune = 0; // last time Genome was Pruned

int Global::nextDataWrite = 0; // next time data files will be written to disk
int Global::nextGenomeWrite = 0; // next time a genome file will be written to genome.csv
map<string,vector<string>> Global::files; // list of files in use with their meta data
