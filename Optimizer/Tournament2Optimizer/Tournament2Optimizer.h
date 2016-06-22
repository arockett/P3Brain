//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Tournament2_Optimizer__
#define __BasicMarkovBrainTemplate__Tournament2_Optimizer__

#include "../AbstractOptimizer.h"

class Tournament2Optimizer : public AbstractOptimizer {
 public:

	shared_ptr<ParameterLink<int>> elitismLPL;
	shared_ptr<ParameterLink<int>> tournamentSizeLPL;

	Tournament2Optimizer(shared_ptr<ParametersTable> _PT = nullptr) : AbstractOptimizer(_PT) {
		elitismLPL = (PT == nullptr) ? AbstractOptimizer::elitismPL : Parameters::getIntLink("OPTIMIZER-elitism", PT);
		tournamentSizeLPL = (PT == nullptr) ? AbstractOptimizer::tournamentSizePL : Parameters::getIntLink("OPTIMIZER-tournamentSize", PT);
	}

	virtual void makeNextGeneration(vector<shared_ptr<Organism>> &population) override;
};

#endif /* defined(__BasicMarkovBrainTemplate__Tournament2_Optimizer__) */
