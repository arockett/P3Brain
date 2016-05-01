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

#include "../Genome/AbstractGenome.h"
#include "../Organism/Organism.h"

#include "../Utilities/Parameters.h"

using namespace std;

class AbstractOptimizer {
 public:
	static shared_ptr<string> Optimizer_MethodStr;

	static shared_ptr<int> elitism;
	static shared_ptr<int> tournamentSize;

 public:
	double maxFitness;
	AbstractOptimizer() {
		maxFitness = 0;
	}
	virtual ~AbstractOptimizer() = default;
	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) = 0;
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */
