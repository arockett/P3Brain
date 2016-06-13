//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "DeterministicGate.h"

DeterministicGate::DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	inputs = addresses.first;
	outputs = addresses.second;
	table = _table;
}

//void DeterministicGate::setupForBits(int* Ins, int nrOfIns, int Out, int logic) {
//	inputs.resize(nrOfIns);
//	for (int i = 0; i < nrOfIns; i++)
//		inputs[i] = Ins[i];
//	outputs.resize(1);
//	outputs[0] = Out;
//	table.resize(1 << nrOfIns);
//	for (int i = 0; i < (1 << nrOfIns); i++) {
//		table[i].resize(1);
//		table[i][0] = (logic >> i) & 1;
//	}
//}

void DeterministicGate::update(vector<double> & nodes, vector<double> & nextNodes) {
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)
	for (size_t i = 0; i < outputs.size(); i++) {
		nextNodes[outputs[i]] += table[input][i];
	}
}

