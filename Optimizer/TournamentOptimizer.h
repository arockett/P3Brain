//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Tournament_Optimizer__
#define __BasicMarkovBrainTemplate__Tournament_Optimizer__

#include "Optimizer.h"

using namespace std;

class TournamentOptimizer : public BaseOptimizer {
 public:
	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population);
};

#endif /* defined(__BasicMarkovBrainTemplate__Tournament_Optimizer__) */
