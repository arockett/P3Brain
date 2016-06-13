//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__ProbabilisticGate__
#define __BasicMarkovBrainTemplate__ProbabilisticGate__

#include "AbstractGate.h"

using namespace std;


class ProbabilisticGate: public AbstractGate {  //conventional probabilistic gate
public:
	vector<vector<double>> table;
	ProbabilisticGate() = delete;
	ProbabilisticGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _rawTable, int _ID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~ProbabilisticGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "Probabilistic";
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__ProbabilisticGate__) */
