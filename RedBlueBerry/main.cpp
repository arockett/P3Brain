//
//  main.cpp
//  Red Berry Blue Berry World
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "Genome.h"
#include "Optimizer.h"
#include "World.h"
#include "Data.h"
#include "Agent.h"
#include "Parameters.h"
#include "RedBlueBerryWorld.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

class MainSettings {
public:
	static bool seedWithPID;
	static int updates;
	static int popSize;
};

bool MainSettings::seedWithPID;
int MainSettings::updates;
int MainSettings::popSize;

/*
 * setupParameters reads command line arguments and a config file and then creates and tracks parameters that are requested from various moduals
 * the working paradigme is that parameters that are local to a module are defined in that modual and setup by a call to "Parameters::setupParameter()"
 * also from that module. Parameters used in main are directly setup in setupParameters.
 */
void setupParameters(int argc, const char** argv) {
	cout << "-----\n"
			<< "  to see available command line/config file arguments set \"displayParameterDescriptions\" to true.\n"
			<< "  to write out a new config file set \"makeConfigFile\" to true.\n" << "-----\n";

	// load arguments from command line
	Parameters::loadArguments(argc, argv);
	// set the config file name (it may have been set on the command line)
	Parameters::setupParameter("configFileName", Parameters::configFileName, "settings.cfg", "name of the config file");
	// load arguments from config file
	Parameters::loadConfigFile();

	// first see if user wants us to be verbose when registering parameters
	Parameters::paramVerbose = false; // must be set in order to call getParameterValue
	Parameters::setupParameter("paramVerbose", Parameters::paramVerbose, false,
			"if true: program reports info when registering parameters");

	// call local initializeParameters for each module
	DataSettings::initializeParameters();
	OptimizerSettings::initializeParameters();
	GenomeSettings::initializeParameters();
	GateSettings::initializeParameters();
	AgentSettings::initializeParameters();
	WorldSettings::initializeParameters();
	RedBlueBerryWorldSettings::initializeParameters();

	//default command line parameters used in main
	Parameters::setupParameter("seedWithPID", MainSettings::seedWithPID, false,
			"if seedWithPID, random number generator will see with process ID");
	Parameters::setupParameter("updates", MainSettings::updates, 200, "how long the program will run");
	Parameters::setupParameter("popSize", MainSettings::popSize, 100, "number of brains in the populaiton");
	// end default command line parameters used in main

	// debugging/display methods
	int printParameterDetail;
	Parameters::setupParameter("printParameterDetail", printParameterDetail, 1,
			"used in main to determine mow much information printParameterInfo will provide");
	bool displayParameterDescriptions;
	Parameters::setupParameter("displayParameterDescriptions", displayParameterDescriptions, false,
			"used in main to switch on showing params");
	bool makeConfigFile;
	Parameters::setupParameter("makeConfigFile", makeConfigFile, false, "write out config file to \"configFileName\"");

	if (printParameterDetail) {
		Parameters::printParameterInfo(printParameterDetail);
	}
	if (displayParameterDescriptions) {
		Parameters::printParameterDescriptions();
	}
	if (makeConfigFile) {
		Parameters::makeConfigFile(Parameters::configFileName, 1);
	}
}

int main(int argc, const char * argv[]) {
	setupParameters(argc, argv); // read command line and config file and set up parameters
	setupGates(); // determines which gate types will be in use.

	if (MainSettings::seedWithPID) { // need to change this out with proper engine!
		srand(getpid()); // need different includes for windows XPLATFORM
	} else {
		srand(DataSettings::repNumber);
	}

	vector<Genome*> population, newPopulation;
	vector<double> W;
	Optimizer *optimizer = (Optimizer*) new GA();
	//	Optimizer *optimizer=(Optimizer*)new Tournament();
	World *world = (World*) new RedBlueBerryWorld(); //new World();

	// Make the output directory for this rep
	string makeDirCommand = "mkdir " + to_string(DataSettings::repNumber);
	const char * DirCommand = makeDirCommand.c_str();
	system(DirCommand);

	Genome *progenitor = new Genome();
	progenitor->ID = -2;
	progenitor->referenceCounter = MainSettings::popSize;
	//setup population
	for (int i = 0; i < MainSettings::popSize; i++) {
		Genome *G = new Genome();
		G->fillRandom();
		G->ancestor = progenitor;
		progenitor->kill();
		population.push_back(G);

	}
	//evolution loop
	for (Data::update = 0; Data::update < MainSettings::updates; Data::update++) {

		//translate all genomes to agents
		vector<Agent*> agents;
		for (int i = 0; i < MainSettings::popSize; i++) {
			agents.push_back(new Agent(population[i], AgentSettings::nrOfBrainStates));
		}

		//evaluate each organism in the population using a World
		W = world->evaluateFitness(agents, true);

		//delete all agents
		for (int i = 0; i < MainSettings::popSize; i++)
			delete agents[i];

		// remove agent pointers (which are pointing to null, since the agents were deleted)
		agents.clear();

		// white data to file and prune LOD
		if (Data::update % DataSettings::dataInterval == 0) {
			// write out data and genomes
			Data::saveDataOnLOD(population[0]);
			cout << "TEST 1\n";
			Genome * MRCA = Data::getMostRecentCommonAncestor(population[0]);
			cout << "TEST 2\n";
			if (MRCA->ancestor != NULL) {
				MRCA->ancestor->kill();
			}
		}

		//make next generation using an optimizer
		newPopulation = optimizer->makeNextGeneration(population, W);
		printf("update: %i maxFitness:%f %f\n", Data::update, optimizer->maxFitness, GenomeSettings::pointMutationRate);
		for (size_t i = 0; i < population.size(); i++) {
			cout << i << " killing old population\n" << progenitor->referenceCounter << "\n";

			population[i]->kill(); // this deletes the genome if it has no offspring
			population[i] = newPopulation[i];
		}
		newPopulation.clear();
	}
	//save data
	//Data::saveDataOnLOD(population[0]->ancestor, DataSettings::DataFileName);
	//Data::saveGenomesOnLOD(population[0]->ancestor, DataSettings::GenomeFileName, DataSettings::dataInterval);
	Agent *A = new Agent((Genome*) population[0]->ancestor->ancestor, AgentSettings::nrOfBrainStates);
	printf("%s\n", A->gateList().c_str());
	return 0;
}

// FROM KLYPH VERSION //////////////////////////////////////////////////
/*
 //pruning and output

 if((Data::update>0)&&(Data::update%pruneInterval==0)){
 for(int popID=0;popID<population.size();popID++){
 for(int backtrack=0;backtrack<pruneInterval-1; backtrack++){
 if(population[popID]->ancestor!=NULL){
 Genome* tempAncestor = population[popID]->ancestor;
 population[popID]->ancestor=tempAncestor->ancestor;
 population[popID]->ancestor->referenceCounter++;
 tempAncestor->kill();
 }
 }
 }
 auto check = find(evalsList,evalsList+numEvals,(Data::update-pruneInterval));
 if(check != evalsList+numEvals){ //if we are going to write out
 vector<Genome*> ancestors;
 vector<Genome*> workingPop=population;
 int lineageDepth=0;
 while(workingPop.size()!=1){
 ancestors.push_back(workingPop[0]->ancestor);
 for(int i=1;i<workingPop.size();i++){
 bool newAncestor=0;
 for(int j=0;j<ancestors.size();j++){
 if(ancestors[j]==workingPop[i]->ancestor){
 newAncestor=1;
 }
 }
 if(newAncestor==0){
 ancestors.push_back(workingPop[i]->ancestor);
 }
 }
 workingPop=ancestors;
 ancestors.clear();
 lineageDepth++;
 }
 */

//Add additional Data through analyze mode
//not needed in the basic example here
/*
 //get the LOD
 vector<Genome*> LOD=Data::getLOD(population[0]->ancestor);
 //iterate over the LOD and have analyze on
 //world will add all parameters you typically need to Data
 for(size_t i=0;i<LOD.size();i++){
 vector<Agent*> agents;
 Agent *A=new Agent((Genome*)LOD[i],MainSettings::nrOfBrainStates);
 agents.push_back(A);
 world->evaluateFitness(agents, true);
 delete A;
 }
 */

