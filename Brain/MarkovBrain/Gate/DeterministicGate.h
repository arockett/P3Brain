//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__DeterministicGate__
#define __BasicMarkovBrainTemplate__DeterministicGate__

#include "AbstractGate.h"

using namespace std;


class DeterministicGate: public AbstractGate {
public:
	vector<vector<int>> table;
	DeterministicGate() = delete;
	DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~DeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);
	virtual string gateType() override{
		return "Deterministic";
	}
};


#endif /* defined(__BasicMarkovBrainTemplate__DeterministicGate__) */
