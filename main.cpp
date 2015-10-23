//
//  main.cpp
//  Red Berry Blue Berry World
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

#include "Worlds/RedBlueBerryWorld.h"


using namespace std;

int main(int argc, const char * argv[]) {
	//setupParameters(argc, argv); // read command line and config file and set up parameters

	Parameters::initialize_parameters(argc, argv); // loads command line and configFile values into registered parameters
												   // also writes out a config file if requested

	//make a node map to handle genome value to brain state address look up.
	Agent::makeNodeMap(Agent::defaultNodeMap, Global::bitsPerBrainAddress, Agent::defaultNrOfBrainStates);

	Gate::setupGates(); // determines which gate types will be in use.

	if (Global::seedRandom) {
		Random::seed();
		Random::seedAlt();
	} else {
		Random::seed(Global::repNumber);
		Random::seedAlt(Global::repNumber+1);
	}


	//Optimizer *optimizer = (Optimizer*) new GA();
	Optimizer *optimizer = (Optimizer*) new Tournament();

	World *world = (World*) new RedBlueBerryWorld(); //new World();



	// Make the output directory for this rep
	string makeDirCommand = "mkdir OUTPUT" + to_string(Global::repNumber);
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
	world->testIndividual(progenitorAgent,false);
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
	while ((Global::savedUpTo(Global::updates)==false) && (Global::update <= (Global::updates + Global::terminateAfter))) {
		// translate all genomes to agents
		vector<Agent*> agents;
		for (int i = 0; i < Global::popSize; i++) {
			agents.push_back(new Agent(population[i], Agent::defaultNrOfBrainStates));
		}

		// evaluate each organism in the population using a World
		W = world->evaluateFitness(agents, false);

		// delete all agents
		for (int i = 0; i < Global::popSize; i++){
			delete agents[i];
		}

		// remove agent pointers (which are pointing to null, since the agents were deleted)
		agents.clear();

		// write data to file and prune LOD every pruneInterval
		if (Global::update % Global::pruneInterval == 0) {
			population[0]->saveDataOnLOD("data.dat",{"update","ID","score","food1","food2","switches","total"}); // write out data and genomes
			population[0]->saveDataOnLOD("history.dat",{"update","ID","birthDate"});
			// data and genomes have now been written out up till the MRCA
			// so all data and genomes from before the MRCA can be deleted
			Genome * MRCA = population[0]->getMostRecentCommonAncestor();
			if (MRCA->ancestor != NULL) {
				MRCA->ancestor->kill();
				MRCA->ancestor=NULL; // MRCA is now the oldest genome!
			}

		}

		Global::update++;

		//make next generation using an optimizer
		newPopulation = optimizer->makeNextGeneration(population, W);
		cout << "update: " << Global::update-1 << "   maxFitness: " << optimizer->maxFitness << "\n";
		for (size_t i = 0; i < population.size(); i++) {
			population[i]->kill(); // this deletes the genome if it has no offspring
			population[i] = newPopulation[i];
		}
		newPopulation.clear();
	}

	cout << "Finished Evolution Loop... force file writes\n";
	population[0]->saveDataOnLOD("data.dat",{"update","ID","score","food1","food2","switches","total"},0);
	population[0]->saveDataOnLOD("history.dat",{"update","ID","birthDate"},0);

	Genome * MRCA = population[0]->getMostRecentCommonAncestor();

	Agent *A = new Agent(MRCA, Agent::defaultNrOfBrainStates);
	printf("MRCA - born on: %d\n", MRCA->birthDate);
	printf("%s\n", A->gateList().c_str());

	return 0;
}

