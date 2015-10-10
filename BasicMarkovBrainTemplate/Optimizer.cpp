//
//  Optimizer.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Optimizer.h"
#include "Random.h"
#include <math.h>
#include <stdlib.h>     /* atoi */

using namespace std;

int& Optimizer::elitism = Parameters::register_parameter("elitism", -1,
		"The highest scoring agent will be included in the next generation this many times (-1 = no elitism)?",
		"OPTIMIZER");

Optimizer::Optimizer() {
}

Optimizer::~Optimizer() {
}

vector<Genome*> Optimizer::makeNextGeneration(vector<Genome*> population, vector<double> W) {
	vector<Genome*> nextGeneration;
	//We make a copy and nothing else
	for (size_t i = 0; i < population.size(); i++) {
		nextGeneration.push_back(new Genome(population[i]));
	}
	return nextGeneration;
}

vector<Genome*> GA::makeNextGeneration(vector<Genome*> population, vector<double> W) {

	vector<Genome*> nextGeneration;
	int best = 0;
	//We need to know max fitness:
	maxFitness = W[0];
	for (size_t i = 1; i < W.size(); i++)
		if (W[i] > maxFitness) {
			maxFitness = W[i];
			best = i;
		}

	cout << "r: " << population[best]->data["food1"] << " b:" << population[best]->data["food2"] << " g:" << population[best]->data["food3"] << " s:"
			<< population[best]->data["switches"] << " t:" << population[best]->data["total"] << "\n   ";

	//now to roulette wheel selection:
	nextGeneration.clear();
	while (nextGeneration.size() < population.size()) {
		int who;
		if ((int) nextGeneration.size() < Optimizer::elitism) {
			who = best;
		} else {
			if (maxFitness > 0.0) {	// if anyone has fitness > 0
				do {
					//WAS who=rand()%(int)W.size();
					who = Random::getIndex(population.size()); //keep choosing a random genome from population until we get one that's good enough
					//WAS }while(pow(1.05,((double)rand()/(double)RAND_MAX))>pow(1.05,(W[who]/maxFitness)));
				} while (pow(1.05, Random::getDouble(1)) > pow(1.05, (W[who] / maxFitness)));
			} else {
				who = Random::getIndex(population.size()); // otherwise, just pick a random genome from population
			}
		}
		nextGeneration.push_back(population[who]->makeMutatedOffspring(Genome::pointMutationRate));
	}
	return nextGeneration;
}

vector<Genome*> Tournament::makeNextGeneration(vector<Genome*> population, vector<double> W) {
	vector<Genome*> nextGeneration;

	//We don't need to know max fitness,
	//but we do it anyways for good form
	maxFitness = W[0];
	int best = 0;
	for (size_t i = 1; i < W.size(); i++)
		if (W[i] > maxFitness) {
			maxFitness = W[i];
			best = i;
		}
	cout << "r: " << population[best]->data["food1"] << " b:" << population[best]->data["food2"] << " g:" << population[best]->data["food3"] << " s:"
			<< population[best]->data["switches"] << " t:" << population[best]->data["total"] << "\n   ";

	//tournament selection:
	// pick two organisms, the one with the higher fitness
	// makes and offspring into the next population
	nextGeneration.clear();
	while (nextGeneration.size() < population.size()) {
		int w1, w2;
		do {
			w1 = Random::getIndex(population.size());
			w2 = Random::getIndex(population.size());
		} while (w1 == w2);
		if (W[w1] > W[w2])
			nextGeneration.push_back(population[w1]->makeMutatedOffspring(Genome::pointMutationRate));
		else
			nextGeneration.push_back(population[w2]->makeMutatedOffspring(Genome::pointMutationRate));
	}
	return nextGeneration;
}

vector<Genome*> Runoff::makeNextGeneration(vector<Genome*> population, vector<double> W) {
	vector<Genome*> nextGeneration;

	//We don't need to know max fitness,
	//but we do it anyways for good form
	maxFitness = W[0];
	int best = 0;
	for (size_t i = 1; i < W.size(); i++)
		if (W[i] > maxFitness) {
			maxFitness = W[i];
			best = i;
		}
	cout << "r: " << population[best]->data["food1"] << " b:" << population[best]->data["food2"] << " g:" << population[best]->data["food3"] << " s:"
			<< population[best]->data["switches"] << " t:" << population[best]->data["total"] << "\n   ";

	//tournament selection:
	// pick two organisms, the one with the higher fitness
	// makes and offspring into the next population
	nextGeneration.clear();
	while (nextGeneration.size() < population.size()) {
		int winner, challanger;
		winner = Random::getIndex(population.size());
		for (int i = 0; i < strengthOfSelection; i++) {
			challanger = Random::getIndex(population.size());
			if (W[challanger] > W[winner]) {
				winner = challanger;
			}
		}
		nextGeneration.push_back(population[winner]->makeMutatedOffspring(Genome::pointMutationRate));
	}
	return nextGeneration;
}
