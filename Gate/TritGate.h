//
//  TritGate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//
//
#include <iostream>
#include <math.h>

#include "Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

class TritDeterministicGate : public Gate {
 public:
	vector<vector<int>> table;

	TritDeterministicGate() = delete;
	TritDeterministicGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _table, int _ID);

	virtual ~TritDeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;

	void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	virtual string description() override;

	//double voidOutput;
};




