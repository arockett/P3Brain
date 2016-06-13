//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Threshold_Gate__
#define __BasicMarkovBrainTemplate__Threshold_Gate__

#include "AbstractGate.h"

using namespace std;

class Thresholdgate : public AbstractGate {
 private:
	int threshold;
 public:
	Thresholdgate() = delete;
	Thresholdgate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~Thresholdgate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
};

#endif /* defined(__BasicMarkovBrainTemplate__Threshold_Gate__) */
