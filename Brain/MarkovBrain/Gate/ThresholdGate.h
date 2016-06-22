//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Threshold_Gate__
#define __BasicMarkovBrainTemplate__Threshold_Gate__

#include "AbstractGate.h"

using namespace std;

class Thresholdgate : public AbstractGate {
 private:
	int threshold;
 public:
	Thresholdgate() = delete;
	Thresholdgate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		threshold = 0; 
	}
	Thresholdgate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~Thresholdgate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;
};

#endif /* defined(__BasicMarkovBrainTemplate__Threshold_Gate__) */
