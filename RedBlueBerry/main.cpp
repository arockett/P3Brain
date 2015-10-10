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
#include "Random.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

int main(int argc, const char * argv[]) {
	//setupParameters(argc, argv); // read command line and config file and set up parameters

	Parameters::initialize_parameters(argc, argv); // loads command line and configFile values into registered parameters
												   // also writes out a config file if requested

	//make a node map to handle genome value to brain state address look up.
	Agent::makeNodeMap(Agent::defaultNodeMap, Data::bitsPerBrainAddress, Agent::defaultNrOfBrainStates);

	Gate::setupGates(); // determines which gate types will be in use.

	if (Data::randomSeed) {
		Random::seed();
		Random::seedAlt();
	} else {
		Random::seed(Data::repNumber);
		Random::seedAlt(Data::repNumber+1);
	}


	//Optimizer *optimizer = (Optimizer*) new GA();
	Optimizer *optimizer = (Optimizer*) new Runoff();

	World *world = (World*) new RedBlueBerryWorld(); //new World();

	// Make the output directory for this rep
	string makeDirCommand = "mkdir OUTPUT" + to_string(Data::repNumber);
	const char * DirCommand = makeDirCommand.c_str();
	system(DirCommand);


	// setup population
	vector<Genome*> population, newPopulation;
	vector<double> W;
	// a progenitor must exist - that is, one ancestor genome
	// this genome is evaluated to populate the dataMap
	Genome* progenitor = new Genome();
	progenitor->sites.resize(1);
	Agent *progenitorAgent = new Agent(progenitor,Agent::defaultNrOfBrainStates);
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
		for (int i = 0; i < Data::popSize; i++){
			delete agents[i];
		}

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
		cout << "update: " << Data::update << "   maxFitness: " << optimizer->maxFitness << "\n";
		for (size_t i = 0; i < population.size(); i++) {
			population[i]->kill(); // this deletes the genome if it has no offspring
			population[i] = newPopulation[i];
		}
		newPopulation.clear();
	}
	cout << "In main(): " << Data::klyphCount <<"\n";

	Genome * MRCA = Data::getMostRecentCommonAncestor(population[0]);

	Agent *A = new Agent(MRCA, Agent::defaultNrOfBrainStates);
	printf("%s\n", A->gateList().c_str());

	return 0;
}

