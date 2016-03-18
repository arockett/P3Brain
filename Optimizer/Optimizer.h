//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "../Genome/Genome.h"
#include "../Organism/Organism.h"

#include "../Utilities/Parameters.h"

using namespace std;

class BaseOptimizer {
 public:
	static string& Optimizer_MethodStr;

	static int& elitism;
	static int& tournamentSize;

 public:
	double maxFitness;
	BaseOptimizer() {
		maxFitness = 0;
	}
	virtual ~BaseOptimizer() = default;
	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population);
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */
