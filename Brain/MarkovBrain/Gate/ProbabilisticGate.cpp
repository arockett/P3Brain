//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#include "ProbabilisticGate.h"

ProbabilisticGate::ProbabilisticGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> rawTable, int _ID, shared_ptr<ParametersTable> _PT) :
	AbstractGate(_PT) {
	ID = _ID;
	int i, j;
	inputs = addresses.first;
	outputs = addresses.second;

	int numInputs = inputs.size();
	int numOutputs = outputs.size();

	table.resize(1 << numInputs);
	//normalize each row
	for (i = 0; i < (1 << numInputs); i++) {  //for each row (each possible input bit string)
		table[i].resize(1 << numOutputs);
		// first sum the row
		double S = 0;
		for (j = 0; j < (1 << numOutputs); j++) {
			S += (double) rawTable[i][j];
		}
		// now normalize the row
		if (S == 0.0) {  //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
			for (j = 0; j < (1 << numOutputs); j++)
				table[i][j] = 1.0 / (double) (1 << numOutputs);
		} else {  //otherwise divide all values in a row by the sum of the row
			for (j = 0; j < (1 << numOutputs); j++)
				table[i][j] = (double) rawTable[i][j] / S;
		}
	}

}

void ProbabilisticGate::update(vector<double> & nodes, vector<double> & nextNodes) {  //this translates the input bits of the current states to the output bits of the next states
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)
	int outputColumn = 0;
	double r = Random::getDouble(1);  // r will determine with set of outputs will be chosen
	while (r > table[input][outputColumn]) {
		r -= table[input][outputColumn];  // this goes across the probability table in row for the given input and subtracts each
		// value in the table from r until r is less than a value it reaches
		outputColumn++;  // we have not found the correct output so move to the next output
	}
	for (size_t i = 0; i < outputs.size(); i++)  //for each output...
		nextNodes[outputs[i]] += 1.0 * ((outputColumn >> (outputs.size() - 1 - i)) & 1);  // convert output (the column number) to bits and pack into next states
																						   // but always put the last bit in the first input (to maintain consistancy)
}

shared_ptr<AbstractGate> ProbabilisticGate::makeCopy(shared_ptr<ParametersTable> _PT)
{
	if (_PT == nullptr) {
		_PT = PT;
	}
	auto newGate = make_shared<ProbabilisticGate>(_PT);
	newGate->table = table;
	newGate->ID = ID;
	newGate->inputs = inputs;
	newGate->outputs = outputs;
	return newGate;
}
