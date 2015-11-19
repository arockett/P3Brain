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

#include "Optimizer.h"

#include "../Utilities/Random.h"

using namespace std;

int& Optimizer::elitism = Parameters::register_parameter("elitism", 0, "The highest scoring brain will be included in the next generation this many times (0 = no elitism)?", "OPTIMIZER");
int& Optimizer::tournamentSize = Parameters::register_parameter("tournamentSize", 2, "how many genomes to consider when doing Tournament selection? 1 will result in random selection.", "OPTIMIZER");

/*
 * Optimizer::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * place holder function, copies population to make new population
 * no selection and no mutation
 */
void Optimizer::makeNextGeneration(vector<Organism*> &population) {
    vector<Organism*> nextPopulation;

    for (size_t i = 0; i < population.size(); i++) {
        Organism* newOrg = new Organism(population[i], population[i]->genome);
        nextPopulation.push_back(newOrg);
    }
    for (size_t i = 0; i < population.size(); i++) {
        population[i]->kill(); // set org.alive = 0 and delete the organism if it has no offspring
    }
    population = nextPopulation;
}

/*
 * GA::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time for each next population genome, select a random number
 * then select (with replacement) a random genome. If the selected genomes fitness beats (there is some
 * normalizing of genomes fitness to max fitness and math, but that's really it) the random value, copy
 * to the next generation and mutate the copy. If it is too low, keep drawing genomes till you get one
 * which is good enough.
 */
void GA::makeNextGeneration(vector<Organism*> &population) {
    vector<Organism*> nextPopulation;

    vector<double> W;
    for (auto org : population) {
        W.push_back(org->score);
    }

    int best = findGreatestInVector(W);
    maxFitness = W[best];

    //now to roulette wheel selection:
    while (nextPopulation.size() < population.size()) {
        int who;
        if ((int) nextPopulation.size() < Optimizer::elitism) {
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
        nextPopulation.push_back(population[who]->makeMutatedOffspring(Genome::pointMutationRate));
    }
    for (size_t i = 0; i < population.size(); i++) {
        population[i]->kill(); // set org.alive = 0 and delete the organism if it has no offspring
    }
    population = nextPopulation;
}

/*
 * Tournament::makeNextGeneration(vector<Genome*> population, vector<double> W)
 * create a new generation one genome at a time
 * for each next population genome, randomly select (with replacement) n genomes (where n = Optimizer::tournamentSize)
 * copy to the next generation and mutate the copy.
 */
void Tournament::makeNextGeneration(vector<Organism*> &population) {
    vector<Organism*> nextPopulation;

    vector<double> Scores;
    for (auto org : population) {
        Scores.push_back(org->score);
    }

    int best = findGreatestInVector(Scores);
    maxFitness = Scores[best];

    while (nextPopulation.size() < population.size()) {
        int winner, challanger;
        if ((int) nextPopulation.size() < Optimizer::elitism) {
            winner = best;
        } else {
            winner = Random::getIndex(population.size());
            for (int i = 0; i < Optimizer::tournamentSize - 1; i++) {
                challanger = Random::getIndex(population.size());
                if (Scores[challanger] > Scores[winner]) {
                    winner = challanger;
                }
            }
        }
        nextPopulation.push_back(population[winner]->makeMutatedOffspring(Genome::pointMutationRate));
    }
    for (size_t i = 0; i < population.size(); i++) {
        population[i]->kill(); // set org.alive = 0 and delete the organism if it has no offspring
    }
    population = nextPopulation;
}

void Tournament2::makeNextGeneration(vector<Organism*> &population) {
}

//
//****************************************************************************
//* code below this line is for testing - it will be deleted at some point.
//****************************************************************************
//
//
//void Tournament2::makeNextGeneration(vector<Organism*> &population) {
//    vector<Organism*> nextPopulation;
//    int p1, p2; // parent1 and 2
//    int challanger; // used when picking best of
//    double surviveChance = 1;
//    bool orgSurvived = 0;
//
//    vector<double> Scores;
//    for (auto org : population) {
//        Scores.push_back(org->score);
//    }
//
//    int best = findGreatestInVector(Scores);
//    maxFitness = Scores[best];
//
//    while (nextPopulation.size() < population.size()) {
//        cout << "                         " << nextPopulation.size() << "\n";
//        // chance for each pick that this org survives to the next population
//
//        if ((int) nextPopulation.size() < Optimizer::elitism) { // if next population has less members then elitism, then p1 is best.
//            p1 = best;
//        } else { // otherwise, p1 is the best of tournamentSize random picks
//            p1 = Random::getIndex(population.size());
//            for (int i = 0; i < Optimizer::tournamentSize - 1; i++) {
//                challanger = Random::getIndex(population.size());
//                if (Scores[challanger] > Scores[p1]) {
//                    p1 = challanger;
//                }
//            }
//        }
//
//        if (Random::P(surviveChance)) { // if this org survives
//            if (find(nextPopulation.begin(), nextPopulation.end(), population[p1]) == nextPopulation.end()) { // if they have not already survived
//                nextPopulation.push_back(population[p1]); // push them to the next population
//                population[p1]->addFollow(); // add a follow since they are in a new population
//                orgSurvived = 1;
//                cout << "rc: " << population[p1]-> referenceCounter-1 << " -> " << population[p1]-> referenceCounter << "  org ID: " << population[p1]->ID << " SURVIVED!!\n";
//            }
//        }
//
//        if (!orgSurvived) {
//            p2 = p1; // make these the same to prime the while loop
//            while ((p1 == p2) || (population[p1]->gender == population[p2]->gender)) { // keep picking until you have 2 diffrent parents with 2 diffrent genders
//                p2 = Random::getIndex(population.size());
//                for (int i = 0; i < Optimizer::tournamentSize - 1; i++) {
//                    challanger = Random::getIndex(population.size());
//                    if (Scores[challanger] > Scores[p2]) {
//                        p2 = challanger;
//                    }
//                }
//            }
//            nextPopulation.push_back(population[p1]->makeMutatedOffspring(Genome::pointMutationRate, population[p2]));
//        }
//        orgSurvived = 0;
//
//    }
//    for (auto org : population) {
//        cout << "unfollowing: " << org->ID << " with " << org->referenceCounter << " refs \n";
//                org->unFollow();
//    }
//    population = nextPopulation;
//
//    ////////////
//    for (auto org : population) {
//        cout << "new pop: " << org->ID << " with " << org->referenceCounter << " refs \n";
//    }
//    ///////////////
//}
///*
// * creates new populations which demonstrate a speciation effet and has organisms that live for multiple generations.
// */
//vector<Organism*> Tournament2::makeNextGeneration(vector<Organism*> population) {
//    vector<Organism*> nextGeneration;
//
//    nextGeneration.push_back(population[0]);
//    population[0]->addFollow();
//
//    nextGeneration.push_back(population[4]);
//    population[4]->addFollow();
//
//    while (nextGeneration.size() < population.size()) {
//
//
//        int winner, winner2;
//        winner = (nextGeneration.size() < 5) ? Random::getInt(0, 4): Random::getInt(5, 9);
//
//        int start = 25;
//        int end = 50;
//
//        winner2 = winner;
//        while (winner2 == winner) {
//            winner2 = Random::getInt(0, 9);
//            double P = 1;
//            if (Global::update > start && Global::update < end) {
//                P = ((double)Global::update - (double)start) / (double)(end-start);
//                if (Random::P(P)) {
//                    if (winner <= 4) {
//                        winner2 = Random::getInt(0, 4);
//                    } else {
//                        winner2 = Random::getInt(5, 9);
//                    }
//                }
//            }
//
//            if (Global::update > end) {
//                if (winner <= 4) {
//                    winner2 = Random::getInt(0, 4);
//                } else {
//                    winner2 = Random::getInt(5, 9);
//                }
//            }
//        }
//
//        nextGeneration.push_back(population[winner]->makeMutatedOffspring(Genome::pointMutationRate, population[winner2]));
//    }
//    return nextGeneration;
//}
