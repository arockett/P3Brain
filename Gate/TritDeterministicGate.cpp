//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

//#include <iostream>
//
#include "TritDeterministicGate.h"
//
//#include "../Utilities/Random.h"
//#include "../Utilities/Utilities.h"
//
TritDeterministicGate::TritDeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID) {
	ID = _ID;
	inputs = addresses.first;
	outputs = addresses.second;
	table = _table;
}

void TritDeterministicGate::update(vector<double> & nodes, vector<double> & nextNodes) {
	int input = vectorToTritToInt(nodes,inputs,true);  // converts the input values into an index
	for (size_t i = 0; i < outputs.size(); i++) {
		nextNodes[outputs[i]] += table[input][i];
	}
}
