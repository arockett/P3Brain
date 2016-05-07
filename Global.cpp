#include "Global.h"
#include "Utilities/Parameters.h"

//global variables that should be accessible to all
shared_ptr<int> Global::randomSeed = Parameters::register_parameter("GLOBAL-randomSeed", 101, "seed for random number generator, if -1 random number generator will be seeded randomly");
shared_ptr<int> Global::updates = Parameters::register_parameter("GLOBAL-updates", 20, "how long the program will run");
shared_ptr<int> Global::popSize = Parameters::register_parameter("GLOBAL-popSize", 100, "number of genomes in the populaiton");
shared_ptr<string> Global::mode = Parameters::register_parameter("GLOBAL-mode", (string) "run", "mode to run MABE in [run,test]");

shared_ptr<string> Global::outputDirectory = Parameters::register_parameter("GLOBAL-outputDirectory", (string) "./", "where files will be written");

shared_ptr<int> Global::bitsPerBrainAddress = Parameters::register_parameter("BRAIN-bitsPerBrainAddress", 8, "how many bits are evaluated to determine the brain addresses");
shared_ptr<int> Global::bitsPerCodon = Parameters::register_parameter("BRAIN-bitsPerCodon", 8, "how many bits are evaluated to determine the codon addresses");

int Global::update = 0;
