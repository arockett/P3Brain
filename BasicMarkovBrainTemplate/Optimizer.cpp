//
//  Optimizer.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Optimizer.h"
#include <math.h>

int Optimizer::elitism=-1;

Optimizer::Optimizer(){
}

Optimizer::~Optimizer(){
}


vector<Genome*> Optimizer::makeNextGeneration(vector<Genome*> population,vector<double> W){
	vector<Genome*> nextGeneration;
	//We make a copy and nothing else
	for(int i=0;i<population.size();i++){
		nextGeneration.push_back(new Genome(population[i]));
	}
	return nextGeneration;
}

vector<Genome*> GA::makeNextGeneration(vector<Genome*> population,vector<double> W){
	vector<Genome*> nextGeneration;
	int best=0;
	//We need to know max fitness:
	maxFitness=W[0];
	for(int i=1;i<W.size();i++)
		if(W[i]>maxFitness){
			maxFitness=W[i];
			best=i;
		}

	//now to roulette wheel selection:
	nextGeneration.clear();
	while(nextGeneration.size()<population.size()){
		int who;
		if(maxFitness>0.0)
			do{
				who=rand()%(int)W.size();
			}while(pow(1.05,((double)rand()/(double)RAND_MAX))>pow(1.05,(W[who]/maxFitness)));
		else
			who=rand()%(int)W.size();
		if((int)nextGeneration.size()<=elitism)
			who=best;
		nextGeneration.push_back((Genome*)population[who]->makeMutatedOffspring(Data::getDefaultDouble("pointMutationRate")));
	}
	return nextGeneration;
}

vector<Genome*> Tournament::makeNextGeneration(vector<Genome*> population,vector<double> W){
	vector<Genome*> nextGeneration;
	
	//We don't need to know max fitness,
	//but we do it anyways for good form
	maxFitness=W[0];
	for(int i=1;i<W.size();i++)
		if(W[i]>maxFitness){
			maxFitness=W[i];
		}

	//tournament selection:
	// pick two organisms, the one with the higher fitness
	// makes and offspring into the next population
	nextGeneration.clear();
	while(nextGeneration.size()<population.size()){
		int w1,w2;
		do{
			w1=rand()%population.size();
			w2=rand()%population.size();
		}while(w1==w2);
		if(W[w1]>W[w2])
			nextGeneration.push_back((Genome*)population[w1]->makeMutatedOffspring(Data::getDefaultDouble("pointMutationRate")));
		else
			nextGeneration.push_back((Genome*)population[w2]->makeMutatedOffspring(Data::getDefaultDouble("pointMutationRate")));
	}
	return nextGeneration;
}
