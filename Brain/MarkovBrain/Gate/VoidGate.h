//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__VoidGate__
#define __BasicMarkovBrainTemplate__VoidGate__

#include "DeterministicGate.h"

using namespace std;

class VoidGate: public DeterministicGate {
public:
	static shared_ptr<ParameterLink<double>> voidGate_ProbabilityPL;
	vector<int> defaultOutput;
	double epsilon;
	VoidGate() = delete;
	VoidGate(shared_ptr<ParametersTable> _PT = nullptr) :
		DeterministicGate(_PT) {
		epsilon = 0; 
	}
	VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, double _epsilon, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~VoidGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
			return "Void";
		}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};

#endif /* defined(__BasicMarkovBrainTemplate__VoidGate__) */
