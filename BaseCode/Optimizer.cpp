//
//  Optimizer.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <math.h>
#include <stdlib.h>     // for atoi

#include "Optimizer.h"

#include "../Utilities/Random.h"

using namespace std;

int& Optimizer::elitism = Parameters::register_parameter("elitism", 0,
		"The highest scoring agent will be included in the next generation this many times (0 = no elitism)?",
		"OPTIMIZER");
int& Optimizer::tournamentSize = Parameters::register_parameter("tournamentSize", 2,
		"how many genomes to consider when doing Tournament selection? 1 will result in random selection.",
		"OPTIMIZER");

/*
 * Optimizer::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * place holder function, copies population to make new population
 * no selection and no mutation
 */
vector<Organism*> Optimizer::makeNextGeneration(vector<Organism*> population) {
	vector<Organism*> nextGeneration;

	for (size_t i = 0; i < population.size(); i++) {
		Organism* newOrg = new Organism(population[i],population[i]->genome);
		nextGeneration.push_back(newOrg);
	}
	return nextGeneration;
}

/*
 * GA::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time for each next population genome, select a random number
 * then select (with replacement) a random genome. If the selected genomes fitness beats (there is some
 * normalizing of genomes fitness to max fitness and math, but that's really it) the random value, copy
 * to the next generation and mutate the copy. If it is too low, keep drawing genomes till you get one
 * which is good enough.
 */
vector<Organism*> GA::makeNextGeneration(vector<Organism*> population) {
	vector<Organism*> nextGeneration;

    vector<double> W;
	for (auto org : population){
    	W.push_back(org->score);
    }

	int best = findGreatestInVector(W);
	maxFitness = W[best];

	//now to roulette wheel selection:
	while (nextGeneration.size() < population.size()) {
		int who;
		if ((int) nextGeneration.size() < Optimizer::elitism) {
			who = best;
		} else {
			if (maxFitness > 0.0) {	// if anyone has fitness > 0
				do {
					who = Random::getIndex(population.size()); //keep choosing a random genome from population until we get one that's good enough
				} while (pow(1.05, Random::getDouble(1)) > pow(1.05, (W[who] / maxFitness)));
			} else {
				who = Random::getIndex(population.size()); // otherwise, just pick a random genome from population
			}
		}
		nextGeneration.push_back(population[who]->makeMutatedOffspring(Genome::pointMutationRate));
	}
	return nextGeneration;
}

/*
 * Tournament::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time
 * for each next population genome, randomly select (with replacement) n genomes (where n = Optimizer::tournamentSize)
 * copy to the next generation and mutate the copy.
 */
vector<Organism*> Tournament::makeNextGeneration(vector<Organism*> population) {
	vector<Organism*> nextGeneration;

    vector<double> Scores;
	for (auto org : population){
    	Scores.push_back(org->score);
    }

	int best = findGreatestInVector(Scores);
	maxFitness = Scores[best];

	for (int i = 0; i < Optimizer::elitism; i++) { // first, if elitism > 0, add this many copies of best to the next generation
		if (nextGeneration.size() < population.size()) {
			nextGeneration.push_back(population[best]->makeMutatedOffspring(Genome::pointMutationRate));
		}
	}
	while (nextGeneration.size() < population.size()) {
		int winner, challanger;
		if ((int) nextGeneration.size() < Optimizer::elitism) {
			winner = best;
		} else {
			winner = Random::getIndex(population.size());
			for (int i = 0; i < Optimizer::tournamentSize - 1; i++) {
				challanger = Random::getIndex(population.size());
				if (Scores[challanger] > Scores[winner]) {
					winner = challanger;
				}
			}
			nextGeneration.push_back(population[best]->makeMutatedOffspring(Genome::pointMutationRate));
		}
	}
	return nextGeneration;
}

