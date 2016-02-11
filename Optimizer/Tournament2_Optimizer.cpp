//
//  Optimizer.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Tournament2_Optimizer.h"
#include "../Utilities/Random.h"

using namespace std;

void Tournament2::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
	vector<shared_ptr<Organism>> nextPopulation;
	set<shared_ptr<Organism>> survivors;
	int p1, p2;  // parent1 and 2
	int challanger;  // used when picking best of
	double surviveChance = 0;
	bool orgSurvived = 0;

	vector<double> Scores;
	for (auto org : population) {
		Scores.push_back(org->score);
	}
	int best = findGreatestInVector(Scores);
	maxFitness = Scores[best];

	{  // check to make sure there are at least 2 genders.
		bool same = true;
		for (int index = 0; index < (int) population.size() - 2; index++) {
			if (population[index]->gender != population[index + 1]->gender) {
				same = false;
			}
		}
		if (same == true) {  // if there are not atleast 2 genders, make population[0] have a diffrent gender
			(population[0]->gender == 1) ? population[0]->gender = 0 : population[0]->gender = 1;
		}
	}

	while (nextPopulation.size() < population.size()) {  // while we have not filled up the next generation
		// chance for each pick that this org survives to the next population

		if ((int) nextPopulation.size() < BaseOptimizer::elitism) {  // if next population has less members then elitism, then p1 is best.
			p1 = best;
		} else {  // otherwise, p1 is the best of tournamentSize random picks
			p1 = Random::getIndex(population.size());
			for (int i = 0; i < BaseOptimizer::tournamentSize - 1; i++) {
				challanger = Random::getIndex(population.size());
				if (Scores[challanger] > Scores[p1]) {
					p1 = challanger;
				}
			}
		}

		orgSurvived = 0;  // clear orgSurvived. if population[p1] survives this will become 1 and we will not pick a mate
		if (Random::P(surviveChance)) {  // if this org survives
			if (find(nextPopulation.begin(), nextPopulation.end(), population[p1]) == nextPopulation.end()) {  // if they have not already survived
				nextPopulation.push_back(population[p1]);  // push them to the next population
				orgSurvived = 1;
				survivors.insert(population[p1]);
			}
		}
		if (!orgSurvived) {
			p2 = p1;  // make these the same to prime the while loop
			while ((p1 == p2) || (population[p1]->gender == population[p2]->gender)) {  // keep picking until you have 2 diffrent parents with 2 diffrent genders
				p2 = Random::getIndex(population.size());
				for (int i = 0; i < BaseOptimizer::tournamentSize - 1; i++) {
					challanger = Random::getIndex(population.size());
					if (Scores[challanger] > Scores[p2]) {
						p2 = challanger;
					}
				}
			}
			cout << "*" << flush;
			nextPopulation.push_back(population[p1]->makeMutatedOffspring( { population[p1], population[p2] }));
			cout << "." << flush;
		}

	}
	for (auto org : population) {
		if (!survivors.count(org)) {  // if this org did not survive to the next generation, it must be killed (sad.)
			org->kill();
		}
	}
	cout << endl;
	population = nextPopulation;
}

