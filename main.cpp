//
//  main.cpp
//
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "BaseCode/Brain.h"
#include "BaseCode/DataHandler.h"
#include "BaseCode/Genome.h"
#include "BaseCode/Global.h"
#include "BaseCode/Optimizer.h"
#include "BaseCode/Organism.h"
#include "BaseCode/World.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

#include "Worlds/BerryWorld.h"

using namespace std;

int main(int argc, const char * argv[]) {

    Parameters::initialize_parameters(argc, argv); // loads command line and configFile values into registered parameters
                                                   // also writes out a config file if requested

    //make a node map to handle genome value to brain state address look up.
    Brain::makeNodeMap(Brain::defaultNodeMap, Global::bitsPerBrainAddress, Brain::defaultNrOfBrainStates);

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

    // setup population
    vector<Organism*> population;

    DataHandler dataHandler;

    // a progenitor must exist - that is, one ancestor genome
    // this genome is evaluated to populate the dataMap

    Global::update = -1; // before there was time, there was a progenitor

    Organism* progenitor = new Organism(new Genome(),new Brain()); // make a organism with a genome and brain (if you need to change the types here is where you do it)
    Organism::MRCA = progenitor; // the progenitor is everyones ancestor

    Global::update = 0; // the begining of time - now we construct the first population

    for (int i = 0; i < Global::popSize; i++) {
        Genome* genome = new Genome();
        genome->fillRandom();
        population.push_back(new Organism(progenitor, genome)); // add a new org to population using progenitors template and a new random genome
        population[population.size()-1]->gender = Random::getInt(0,1); // assign a random gender to the new org
    }
    progenitor->kill(); // the progenitor has served it's purpose.

    //////////////////
    // evolution loop
    //////////////////

    int outputMethod = 0; // if SSwD is being used then we want to run until intervalDelay for the last file has passed. This is assured at Global::intervalDelay + Global::updates
    int realTerminateAfter = (outputMethod == 0) ? (Global::terminateAfter) : Global::intervalDelay; // if the output method is SSwD override terminateAfter

    while (((Global::nextDataWrite <= Global::updates) || (Global::nextGenomeWrite <= Global::updates))
            && (Global::update <= (Global::updates + realTerminateAfter))) {

        // evaluate each organism in the population using a World
        world->evaluateFitness(population, false);

        dataHandler.saveData(population,outputMethod);

        // Data populations have been updated and data has been saved. It's time to make some new organisms.
        Global::update++;

        //make next generation using an optimizer
        optimizer->makeNextGeneration(population);
        cout << "update: " << Global::update - 1 << "   maxFitness: " << optimizer->maxFitness << "\n";
    }
    if (outputMethod == 0) {
        cout << "Finished Evolution Loop... force file writes\n";
        population[0]->flushDataOnLOD();
    Organism * FinalMRCA = population[0]->getMostRecentCommonAncestor();

    printf("MRCA - born on: %d\n", FinalMRCA->birthDate);
    printf("%s\n", FinalMRCA->brain->gateList().c_str());
    }

    return 0;
}

