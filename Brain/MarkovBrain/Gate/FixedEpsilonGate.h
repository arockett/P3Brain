//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__FixedEpsilonGate__
#define __BasicMarkovBrainTemplate__FixedEpsilonGate__

#include "DeterministicGate.h"

using namespace std;

class FixedEpsilonGate: public DeterministicGate {
public:
	static shared_ptr<ParameterLink<double>> EpsilonSourcePL;

	vector<int> defaultOutput;
	double epsilon;
	FixedEpsilonGate() = delete;
	FixedEpsilonGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, double _epsilon, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~FixedEpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "FixedEpsilon";
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__FixedEpsilonGate__) */
