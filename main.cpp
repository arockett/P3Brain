//
//  main.cpp
//
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "BaseCode/Agent.h"
#include "BaseCode/Genome.h"
#include "BaseCode/Global.h"
#include "BaseCode/Optimizer.h"
#include "BaseCode/World.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

#include "Worlds/BerryWorld.h"

using namespace std;

void writeRealTimeFiles(vector<Genome*> population, vector<double> W) { // write Ave and Dominant files NOW!
// write out Ave
	double aveValue, temp;
	DataMap AveMap;
	for (auto key : Global::DefaultAveFileColumns) {
		aveValue = 0;
		for (auto genome : population) {
			stringstream ss(genome->dataMap.Get(key));
			ss >> temp;
			aveValue += temp;
		}
		aveValue /= population.size();
		AveMap.Set(key, aveValue);
	}
	AveMap.writeToFile(Global::AveFileName, Global::DefaultAveFileColumns);

	// write out Dominant
	int best = findGreatestInVector(W);
	population[best]->dataMap.writeToFile(Global::DominantFileName);
}

int main(int argc, const char * argv[]) {
	//setupParameters(argc, argv); // read command line and config file and set up parameters

	Parameters::initialize_parameters(argc, argv); // loads command line and configFile values into registered parameters
												   // also writes out a config file if requested

	//make a node map to handle genome value to brain state address look up.
	Agent::makeNodeMap(Agent::defaultNodeMap, Global::bitsPerBrainAddress, Agent::defaultNrOfBrainStates);

	Gate::setupGates(); // determines which gate types will be in use.

	if (Global::seedRandom) {
		random_device rd;
		Random::getCommonGenerator().seed(rd());
	} else {
		Random::getCommonGenerator().seed(Global::repNumber);
	}

	//Optimizer *optimizer = (Optimizer*) new GA();
	Optimizer *optimizer = (Optimizer*) new Tournament();

	World *world = (World*) new BerryWorld(); //new World();

	// Make the output directory for this rep
//	string makeDirCommand = "mkdir OUTPUT" + mkString(Global::repNumber);
//	const char * DirCommand = makeDirCommand.c_str();
//	system(DirCommand);

	// To add extra output files, add something like this:
	//   Global::files[file_name] = {list of elements to save from a DataMap};
	//   eg: Global::files["world.csv"] = {"update","score","food1","food2","switches"};

	// setup population
	vector<Genome*> population, newPopulation;
	vector<double> W;
	// a progenitor must exist - that is, one ancestor genome
	// this genome is evaluated to populate the dataMap
	Genome* progenitor = new Genome();
	progenitor->birthDate = -1;
	Genome::MRCA = progenitor;
	progenitor->sites.resize(1);
	Agent *progenitorAgent = new Agent(progenitor, Agent::defaultNrOfBrainStates);
	world->testIndividual(progenitorAgent, false);
	delete progenitorAgent;
	for (int i = 0; i < Global::popSize; i++) {
		Genome *G = new Genome();
		G->fillRandom();
		G->ancestor = progenitor;
		progenitor->referenceCounter++;
		population.push_back(G);
	}
	progenitor->kill();

	// evolution loop
	Global::update = 0;
	while ((Global::nextDataWrite <= Global::updates) && (Global::nextGenomeWrite <= Global::updates)
			&& (Global::update <= (Global::updates + Global::terminateAfter))) {
		// translate all genomes to agents
		vector<Agent*> agents;
		for (int i = 0; i < Global::popSize; i++) {
			agents.push_back(new Agent(population[i], Agent::defaultNrOfBrainStates));
		}

		// evaluate each organism in the population using a World
		W = world->evaluateFitness(agents, false);

		// delete all agents
		for (int i = 0; i < Global::popSize; i++) {
			delete agents[i];
		}

		// remove agent pointers (which are pointing to null, since the agents were deleted)
		agents.clear();

		// write data to file and prune LOD every pruneInterval
		if (Global::update % Global::pruneInterval == 0) {
			writeRealTimeFiles(population, W); // write to dominant and average files
			Genome::MRCA = population[0]->getMostRecentCommonAncestor();
			population[0]->saveDataOnLOD(); // write out data and genomes
			// data and genomes have now been written out up till the MRCA
			// so all data and genomes from before the MRCA can be deleted
			if (Genome::MRCA->ancestor != nullptr) { // if the MRCA is not the oldest in the LOD...
				Genome::MRCA->ancestor->kill(); // kill MRCAs parent (and as a result all ancestors)
				Genome::MRCA->ancestor = nullptr; // MRCA is now the oldest genome in LOD!
			}
			Global::lastPrune = Genome::MRCA->birthDate; // this will hold the time of the oldest genome in RAM
		}

		Global::update++;

		//make next generation using an optimizer
		newPopulation = optimizer->makeNextGeneration(population, W);
		cout << "update: " << Global::update - 1 << "   maxFitness: " << optimizer->maxFitness << "\n";
		for (size_t i = 0; i < population.size(); i++) {
			population[i]->kill(); // this deletes the genome if it has no offspring
			population[i] = newPopulation[i];
		}
		newPopulation.clear();
	}
	cout << "Finished Evolution Loop... force file writes\n";
	population[0]->flushDataOnLOD();

	Genome * MRCA = population[0]->getMostRecentCommonAncestor();

	Agent *A = new Agent(MRCA, Agent::defaultNrOfBrainStates);
	printf("MRCA - born on: %d\n", MRCA->birthDate);
	printf("%s\n", A->gateList().c_str());

	return 0;
}

