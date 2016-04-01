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

void AbstractWorld::evaluateFitness(vector<shared_ptr<Organism>> population, bool analyse) {
	for (size_t i = 0; i < population.size(); i++) {
		double score;
		double scoreTotal = 0.0;
		for (int r = 0; r < AbstractWorld::repeats; r++) {
			score = testIndividual(population[i], analyse);
			scoreTotal += score;
			population[i]->dataMap.Append("allscore", score);
		}
		population[i]->score = (scoreTotal / (double) repeats);
		population[i]->dataMap.Set("update", Global::update);
	}
}
