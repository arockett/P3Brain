#include "Global.h"
#include "Utilities/Parameters.h"

//global variables that should be accessible to all
int& Global::randomSeed = Parameters::register_parameter("randomSeed", 101, "seed for random number generator, if -1 random number generator will be seeded randomly", "GLOBAL");
int& Global::updates = Parameters::register_parameter("updates", 2000, "how long the program will run", "GLOBAL");
int& Global::popSize = Parameters::register_parameter("popSize", 150, "number of genomes in the populaiton", "GLOBAL");

string& Global::outputDirectory = Parameters::register_parameter("outputDirectory", (string) "./", "where files will be written", "GLOBAL");

int& Global::bitsPerBrainAddress = Parameters::register_parameter("bitsPerBrainAddress", 8, "how many bits are evaluated to determine the brain addresses", "BRAIN");

int Global::update = 0;
set<int> Global::inUseGateTypes;
