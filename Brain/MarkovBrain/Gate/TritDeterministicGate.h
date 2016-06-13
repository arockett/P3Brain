//
//  TritGate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//
//

#ifndef __BasicMarkovBrainTemplate__TritGate__
#define __BasicMarkovBrainTemplate__TritGate__

#include "AbstractGate.h"

class TritDeterministicGate : public AbstractGate {
 public:
	vector<vector<int>> table;

	TritDeterministicGate() = delete;
	TritDeterministicGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~TritDeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;

	void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	virtual string gateType() override{
		return "TritDeterministic";
	}

	//double voidOutput;
};

#endif /* defined(__BasicMarkovBrainTemplate__TritGate__) */



