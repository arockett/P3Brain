#include "Global.h"

//global variables that should be accessible to all
shared_ptr<ParameterLink<int>> Global::randomSeedPL = Parameters::register_parameter("GLOBAL-randomSeed", 101, "seed for random number generator, if -1 random number generator will be seeded randomly");
shared_ptr<ParameterLink<int>> Global::updatesPL = Parameters::register_parameter("GLOBAL-updates", 20, "how long the program will run");
shared_ptr<ParameterLink<int>> Global::popSizePL = Parameters::register_parameter("GLOBAL-popSize", 100, "number of genomes in the populaiton");
shared_ptr<ParameterLink<string>> Global::modePL = Parameters::register_parameter("GLOBAL-mode", (string) "run", "mode to run MABE in [run,test]");

shared_ptr<ParameterLink<string>> Global::outputDirectoryPL = Parameters::register_parameter("GLOBAL-outputDirectory", (string) "./", "where files will be written");

shared_ptr<ParameterLink<string>> Global::groupNameSpacesPL = Parameters::register_parameter("GLOBAL-groupNameSpaces", (string) "[]", "name spaces (also names) of groups to be created (in addition to the defualt no name space group.)");

shared_ptr<ParameterLink<int>> Global::bitsPerBrainAddressPL = Parameters::register_parameter("BRAIN-bitsPerBrainAddress", 8, "how many bits are evaluated to determine the brain addresses");
shared_ptr<ParameterLink<int>> Global::bitsPerCodonPL = Parameters::register_parameter("BRAIN-bitsPerCodon", 8, "how many bits are evaluated to determine the codon addresses");

int Global::update = 0;
