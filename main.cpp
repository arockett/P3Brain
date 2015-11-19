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

#include "BaseCode/Archivist.h"
#include "BaseCode/Brain.h"
#include "BaseCode/Genome.h"
#include "BaseCode/Global.h"
#include "BaseCode/Group.h"
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
    // Optimizer *optimizer = (Optimizer*) new Tournament();

    World *world = (World*) new BerryWorld(); //new World();

    // setup population
    vector<Organism*> population;

    Archivist archivist;

    // a progenitor must exist - that is, one ancestor genome
    // this genome is evaluated to populate the dataMap

    Global::update = -1; // before there was time, there was a progenitor

    Organism* progenitor = new Organism(new Genome(), new Brain()); // make a organism with a genome and brain (if you need to change the types here is where you do it)
    Organism::MRCA = progenitor; // the progenitor is everyones ancestor

    Global::update = 0; // the begining of time - now we construct the first population

    for (int i = 0; i < Global::popSize; i++) {
        Genome* genome = new Genome();
        genome->fillRandom();
        population.push_back(new Organism(progenitor, genome)); // add a new org to population using progenitors template and a new random genome
        population[population.size() - 1]->gender = Random::getInt(0, 1); // assign a random gender to the new org
    }
    progenitor->kill(); // the progenitor has served it's purpose.

    Group* group = new Group(population, new Tournament());

    //////////////////
    // evolution loop
    //////////////////

    int outputMethod = 0; // if SSwD is being used then we want to run until intervalDelay for the last file has passed. This is assured at Global::intervalDelay + Global::updates
    int realTerminateAfter = (outputMethod == 0) ? (Global::terminateAfter) : Archivist::intervalDelay; // if the output method is SSwD override terminateAfter

    while (((Archivist::nextDataWrite <= Global::updates) || (Archivist::nextGenomeWrite <= Global::updates)) && (Global::update <= (Global::updates + realTerminateAfter))) {

        world->evaluateFitness(group->population, false); // evaluate each organism in the population using a World
        group->archive(); // save data, update memory and delete any unneeded data;
        Global::update++;
        group->optimize(); // update the population (reproduction and death)

        cout << "update: " << Global::update - 1 << "   maxFitness: " << group->optimizer->maxFitness << "\n";
    }

    group->archive(1); // flush any data that has not been output yet

    if (Archivist::outputMethod == 0) { // if using LODwAP, write out some info about MRCA
        Organism* FinalMRCA = group->population[0]->getMostRecentCommonAncestor();
        cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << "\n" << FinalMRCA->brain->description();
    }

    return 0;
}

