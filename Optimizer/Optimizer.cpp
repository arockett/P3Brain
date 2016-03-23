//
//  Optimizer.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <algorithm>
#include <math.h>
#include <stdlib.h>     // for atoi

#include "../Optimizer/Optimizer.h"

#include "../Utilities/Random.h"

using namespace std;

const string& BaseOptimizer::Optimizer_MethodStr = Parameters::register_parameter("optimizer", (string) "GA", "optimizer to be used in evolution loop, [GA, Tournament, Tournament2]", "OPTIMIZER");  // string parameter for outputMethod;

const int& BaseOptimizer::elitism = Parameters::register_parameter("elitism", 0, "if the chosen optimizer allows for elitism, The highest scoring brain will be included in the next generation this many times (0 = no elitism)?", "OPTIMIZER");
const int& BaseOptimizer::tournamentSize = Parameters::register_parameter("tournamentSize", 5, "how many genomes to consider when doing Tournament selection? 1 will result in random selection.", "OPTIMIZER - TOURNAMENT");

/*
 * Optimizer::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * place holder function, copies population to make new population
 * no selection and no mutation
 */
void BaseOptimizer::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
	vector<shared_ptr<Organism>> nextPopulation;
	for (size_t i = 0; i < population.size(); i++) {
		shared_ptr<Organism> newOrg = make_shared<Organism>(population[i], population[i]->genome);
		nextPopulation.push_back(newOrg);
	}
	for (size_t i = 0; i < population.size(); i++) {
		population[i]->kill();  // set org.alive = 0 and delete the organism if it has no offspring
	}
	population = nextPopulation;
}
