//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__FixedEpsilonGate__
#define __BasicMarkovBrainTemplate__FixedEpsilonGate__

#include <deque>
#include <functional>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>

#include "DeterministicGate.h"

#include "../Genome/AbstractGenome.h"
#include "../Global.h"

#include "../Utilities/Parameters.h"

using namespace std;

class FixedEpsilonGate: public DeterministicGate {
public:
	static const double& FixedEpsilonGate_Probability;
	vector<int> defaultOutput;
	double epsilon;
	FixedEpsilonGate() = delete;
	FixedEpsilonGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~FixedEpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "FixedEpsilon";
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__FixedEpsilonGate__) */