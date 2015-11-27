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

  Parameters::initialize_parameters(argc, argv);  // loads command line and configFile values into registered parameters
                                                  // also writes out a config file if requested

  //make a node map to handle genome value to brain state address look up.
  Brain::makeNodeMap(Brain::defaultNodeMap, Global::bitsPerBrainAddress, Brain::defaultNrOfBrainStates);

  Gate::setupGates();  // determines which gate types will be in use.

  if (Global::seedRandom) {
    random_device rd;
    Random::getCommonGenerator().seed(rd());
  } else {
    Random::getCommonGenerator().seed(Global::repNumber);
  }

  //Optimizer *optimizer = (Optimizer*) new GA();
  // Optimizer *optimizer = (Optimizer*) new Tournament();

  World *world = (World*) new BerryWorld();  //new World();

  // define population

  // a progenitor must exist - that is, one ancestor genome
  // this genome is evaluated to populate the dataMap

  Global::update = -1;  // before there was time, there was a progenitor
  shared_ptr<Group> group;

  {
    vector<shared_ptr<Organism>> population;

    //shared_ptr<Genome> _genome(new Genome());
    //shared_ptr<Brain> _brain(new Brain());

    //shared_ptr<Organism> progenitor = make_shared<Organism>(_genome, _brain); // make a organism with a genome and brain (if you need to change the types here is where you do it)
    shared_ptr<Organism> progenitor = make_shared<Organism>(make_shared<Genome>(), make_shared<Brain>());  // make a organism with a genome and brain (if you need to change the types here is where you do it)

    Global::update = 0;  // the begining of time - now we construct the first population
    for (int i = 0; i < Global::popSize; i++) {
      shared_ptr<Genome> genome(new Genome());
      genome->fillRandom();
      shared_ptr<Organism> org(new Organism(progenitor, genome));
      population.push_back(org);  // add a new org to population using progenitors template and a new random genome
      population[population.size() - 1]->gender = Random::getInt(0, 1);  // assign a random gender to the new org
    }
    progenitor->kill();  // the progenitor has served it's purpose.

    group = make_shared<Group>(population, make_shared<Tournament>());
  }

  //////////////////
  // evolution loop
  //////////////////

  if (Archivist::outputMethod == -1) {  // this is the first time archive is called. get the output method
    if (Archivist::outputMethodStr == "LODwAP") {
      Archivist::outputMethod = 0;
    } else if (Archivist::outputMethodStr == "SSwD") {
      Archivist::outputMethod = 1;
    } else {
      cout << "unrecognized archive method \"" << Archivist::outputMethodStr << "\". Should be either \"LODwAP\" or \"SSwD\"\nExiting.\n";
      exit(1);
    }
  }

  int realTerminateAfter = (Archivist::outputMethod == 0) ? (Global::terminateAfter) : Archivist::intervalDelay;  // if the output method is SSwD override terminateAfter

  while (((Archivist::nextDataWrite <= Global::updates) || (Archivist::nextGenomeWrite <= Global::updates)) && (Global::update <= (Global::updates + realTerminateAfter))) {
    world->evaluateFitness(group->population, false);  // evaluate each organism in the population using a World

    group->archive();  // save data, update memory and delete any unneeded data;

    Global::update++;

    group->optimize();  // update the population (reproduction and death)

    cout << "update: " << Global::update - 1 << "   maxFitness: " << group->optimizer->maxFitness << "\n";
  }

  group->archive(1);  // flush any data that has not been output yet

  if (Archivist::outputMethod == 0) {  // if using LODwAP, write out some info about MRCA
    shared_ptr<Organism> FinalMRCA = group->population[0]->getMostRecentCommonAncestor(group->population[0]);
    cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << "\n" << FinalMRCA->brain->description();
  }
  return 0;
}

