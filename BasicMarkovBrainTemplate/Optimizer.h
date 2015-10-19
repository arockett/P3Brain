//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include <stdlib.h>
#include <iostream>
#include "Genome.h"
#include <vector>
#include "Parameters.h"

using namespace std;

class Optimizer{
public:
	static int& elitism;
	static int& tournamentSize;

public:
	double maxFitness;
	Optimizer();
	virtual ~Optimizer();
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
