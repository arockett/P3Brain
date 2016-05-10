//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Tournament2_Optimizer__
#define __BasicMarkovBrainTemplate__Tournament2_Optimizer__

#include "AbstractOptimizer.h"

class Tournament2Optimizer : public AbstractOptimizer {
 public:

	shared_ptr<ParameterLink<int>> elitismPL;
	shared_ptr<ParameterLink<int>> tournamentSizePL;

	Tournament2Optimizer() {
		elitismPL = AbstractOptimizer::elitismPL;
		tournamentSizePL = AbstractOptimizer::tournamentSizePL;
	}

	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) override;
};

#endif /* defined(__BasicMarkovBrainTemplate__Tournament2_Optimizer__) */
