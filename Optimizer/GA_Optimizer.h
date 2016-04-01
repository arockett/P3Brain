//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__GA_Optimizer__
#define __BasicMarkovBrainTemplate__GA_Optimizer__

#include "AbstractOptimizer.h"

using namespace std;

class GA_Optimizer : public AbstractOptimizer {
 public:
	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) override;
};

#endif /* defined(__BasicMarkovBrainTemplate__GA_Optimizer__) */
