//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__VoidGate__
#define __BasicMarkovBrainTemplate__VoidGate__

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

class VoidGate: public DeterministicGate {
	static const double& voidGate_Probability;
public:
	vector<int> defaultOutput;
	double epsilon;
	VoidGate() = delete;
	VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~VoidGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
			return "Void";
		}

};

#endif /* defined(__BasicMarkovBrainTemplate__VoidGate__) */
