//
//  World.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "AbstractWorld.h"

#include <math.h>

#include "../Utilities/Data.h"

const int& AbstractWorld::repeats = Parameters::register_parameter("repeats", 3, "Number of times to test each Genome per generation", "WORLD");
const bool& AbstractWorld::showOnUpdate = Parameters::register_parameter("showOnUpdate", false, "display world while running world updates (if world allows)", "WORLD");

void AbstractWorld::evaluateFitness(vector<shared_ptr<Organism>> population, bool analyse, bool show) {
	for (size_t i = 0; i < population.size(); i++) {
		double score;
		double scoreTotal = 0.0;
		for (int r = 0; r < AbstractWorld::repeats; r++) {
			score = testIndividual(population[i], analyse, show);
			scoreTotal += score;
			population[i]->dataMap.Append("allscore", score);
		}
		population[i]->score = (scoreTotal / (double) repeats);
		population[i]->dataMap.Set("update", Global::update);
	}
}
