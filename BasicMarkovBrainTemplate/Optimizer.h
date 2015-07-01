//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include "Genome.h"
#include <vector>

using namespace std;

class Optimizer{
public:
	static int elitism;
	double maxFitness;
	Optimizer();
	~Optimizer();
	virtual vector<Genome*> makeNextGeneration(vector<Genome*> population,vector<double> W);
};

class GA:Optimizer{
public:
	virtual vector<Genome*> makeNextGeneration(vector<Genome*> population,vector<double> W);
};

class P3:Optimizer{
	
};

class Tournament:Optimizer{
public:
	virtual vector<Genome*> makeNextGeneration(vector<Genome*> population,vector<double> W);
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */
