//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__TritGate__
#define __BasicMarkovBrainTemplate__TritGate__

#include "AbstractGate.h"

class TritDeterministicGate : public AbstractGate {
 public:
	vector<vector<int>> table;

	TritDeterministicGate() = delete;
	TritDeterministicGate(shared_ptr<ParametersTable> _PT = nullptr) :
		AbstractGate(_PT) {
		table = {};
	}
	TritDeterministicGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~TritDeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;

	//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	virtual string gateType() override{
		return "TritDeterministic";
	}

	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

	//double voidOutput;
};

#endif /* defined(__BasicMarkovBrainTemplate__TritGate__) */



