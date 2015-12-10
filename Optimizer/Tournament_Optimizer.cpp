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

#include "Tournament_Optimizer.h"
#include "../Utilities/Random.h"

using namespace std;

/*
 * Tournament::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time
 * for each next population genome, randomly select (with replacement) n genomes (where n = Optimizer::tournamentSize)
 * copy to the next generation and mutate the copy.
 */
void Tournament::makeNextGeneration(vector<shared_ptr<Organism>> &population) {
  vector<shared_ptr<Organism>> nextPopulation;

  vector<double> Scores;
  for (auto org : population) {
    Scores.push_back(org->score);
  }

  int best = findGreatestInVector(Scores);
  maxFitness = Scores[best];

  while (nextPopulation.size() < population.size()) {
    int winner, challanger;
    if ((int) nextPopulation.size() < BaseOptimizer::elitism) {
      winner = best;
    } else {
      winner = Random::getIndex(population.size());
      for (int i = 0; i < BaseOptimizer::tournamentSize - 1; i++) {
        challanger = Random::getIndex(population.size());
        if (Scores[challanger] > Scores[winner]) {
          winner = challanger;
        }
      }
    }
    nextPopulation.push_back(population[winner]->makeMutatedOffspring(population[winner]));
  }
  for (size_t i = 0; i < population.size(); i++) {
    population[i]->kill();  // set org.alive = 0 and delete the organism if it has no offspring
  }
  population = nextPopulation;

}
