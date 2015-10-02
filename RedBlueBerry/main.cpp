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
#include <memory>

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

///*
// * setupParameters reads command line arguments and a config file and then creates and tracks parameters that are requested from various moduals
// * parameters that are local to a module are defined in that modual and setup by a call to "Parameters::setupParameter()"
// * also from that module. Parameters used in main are directly setup in setupParameters.
// */
//void setupParameters(int argc, const char** argv) {
//	cout << "-----\n"
//			<< "  to see available command line/config file arguments set \"displayParameterDescriptions\" to true.\n"
//			<< "  to write out a new config file set \"makeConfigFile\" to true.\n" << "-----\n";
//
//	// load arguments from command line
//	Parameters::loadArguments(argc, argv);
//
//	// set the config file name (it may have been set on the command line)
//	Parameters::setupParameter("configFileName", Parameters::configFileName, "settings.cfg", "name of the config file");
//
//	// load arguments from config file
//	Parameters::loadConfigFile();
//
//	// first see if user wants us to be verbose when registering parameters
//	Parameters::paramVerbose = false; // must be set in order to call getParameterValue
//	Parameters::setupParameter("paramVerbose", Parameters::paramVerbose, false,
//			"if true: program reports info when registering parameters");
//
//	// initializeParameters - sets up variables with their values
//	// Critical initializeParameters DO NOT CHANGE THESE!
//	Data::initializeParameters();
//	Optimizer::initializeParameters();
//	Genome::initializeParameters();
//	Gate::initializeParameters();
//	Agent::initializeParameters();
//	World::initializeParameters();
//
//	// Add addtional initializeParameters below
//	RedBlueBerryWorld::initializeParameters();
//
//
//
//	// debugging/display methods
//	int printParameterDetail;
//	Parameters::setupParameter("printParameterDetail", printParameterDetail, 1,
//			"used in main to determine mow much information printParameterInfo will provide");
//	bool displayParameterDescriptions;
//	Parameters::setupParameter("displayParameterDescriptions", displayParameterDescriptions, false,
//			"used in main to switch on showing params");
//	bool makeConfigFile;
//	Parameters::setupParameter("makeConfigFile", makeConfigFile, false, "write out config file to \"configFileName\"");
//
//	if (printParameterDetail) {
//		Parameters::printParameterInfo(printParameterDetail);
//	}
//	if (displayParameterDescriptions) {
//		Parameters::printParameterDescriptions();
//	}
//	if (makeConfigFile) {
//		Parameters::makeConfigFile(Parameters::configFileName, 1);
//	}
//}

int main(int argc, const char * argv[]) {
	//setupParameters(argc, argv); // read command line and config file and set up parameters

	Parameters::initialize_parameters(argc, argv);

	//make a node map to handle genome value to brain state address look up.
	Agent::makeNodeMap(Agent::defaultNodeMap, Agent::sitesPerBrainAddress, Agent::defaultNrOfBrainStates);

	Gate::setupGates(); // determines which gate types will be in use.

	if (Data::seedWithPID) { // need to change this out with proper engine!
		srand(getpid()); // need different includes for windows XPLATFORM
	} else {
		srand(Data::repNumber);
	}


	Optimizer *optimizer = (Optimizer*) new GA();

	World *world = (World*) new RedBlueBerryWorld(); //new World();

	// Make the output directory for this rep
	string makeDirCommand = "mkdir " + to_string(Data::repNumber);
	const char * DirCommand = makeDirCommand.c_str();
	system(DirCommand);


	// setup population
	vector<Genome*> population, newPopulation;
	vector<double> W;
	// a progenitor must exist - that is, one ancestor genome
	// this genome is evaluated to populate the dataMap
	Genome* progenitor = new Genome();
	progenitor->sites.resize(1);
	Agent *progenitorAgent = new Agent(progenitor,16);
	world->testIndividual(progenitorAgent,true);
	delete progenitorAgent;
	for (int i = 0; i < Data::popSize; i++) {
		Genome *G = new Genome();
		G->fillRandom();
		G->ancestor = progenitor;
		progenitor->referenceCounter++;
		population.push_back(G);
	}
	progenitor->kill();


	// evolution loop
	for (Data::update = 0; Data::lastSaveUpdate < Data::updates; Data::update++) {

		// translate all genomes to agents
		vector<Agent*> agents;
		for (int i = 0; i < Data::popSize; i++) {
			agents.push_back(new Agent(population[i], Agent::defaultNrOfBrainStates));
		}

		// evaluate each organism in the population using a World
		W = world->evaluateFitness(agents, false);

		// delete all agents
		for (int i = 0; i < Data::popSize; i++)
			delete agents[i];

		// remove agent pointers (which are pointing to null, since the agents were deleted)
		agents.clear();

		// write data to file and prune LOD every pruneInterval
		if (Data::update % Data::pruneInterval == 0) {
			Data::saveDataOnLOD(population[0]); // write out data and genomes
			// data and genomes have now been written out up till the MRCA
			// so all data and genomes from before the MRCA can be deleted
			Genome * MRCA = Data::getMostRecentCommonAncestor(population[0]);
			if (MRCA->ancestor != NULL) {
				MRCA->ancestor->kill();
				MRCA->ancestor=NULL; // MRCA is now the oldest genome!
			}
		}

		//make next generation using an optimizer
		newPopulation = optimizer->makeNextGeneration(population, W);
		printf("update: %i maxFitness:%f %f\n", Data::update, optimizer->maxFitness, Genome::pointMutationRate);
		for (size_t i = 0; i < population.size(); i++) {
			population[i]->kill(); // this deletes the genome if it has no offspring
			population[i] = newPopulation[i];
		}
		newPopulation.clear();
	}

	Agent *A = new Agent(population[0]->ancestor->ancestor, Agent::defaultNrOfBrainStates);
	printf("%s\n", A->gateList().c_str());

	return 0;
}

