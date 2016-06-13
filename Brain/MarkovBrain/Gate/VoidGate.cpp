//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "VoidGate.h"

/* this gate behaves like a deterministic gate with a constant externally set error which may set a single output to 0 */

shared_ptr<ParameterLink<double>> VoidGate::voidGate_ProbabilityPL = Parameters::register_parameter("BRAIN_MARKOV_GATES_VOID-voidGate_Probability", 0.05, "chance that an output from a void gate will be set to 0");

VoidGate::VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID, shared_ptr<ParametersTable> _PT) :
		DeterministicGate(addresses, _table, _ID, _PT) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
	epsilon = voidGate_ProbabilityPL->lookup();  // in case you want to have different epsilon for different gates (who am I to judge?)
}

void VoidGate::update(vector<double> & nodes, vector<double> & nextNodes) {
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)

	vector<int> outputRow = table[input]; // pick the output row

	if (Random::P(epsilon)) { // if chance...
		outputRow[Random::getIndex(outputs.size())] = 0;  // pick one output randomly and set it to 0
	}
	for (size_t i = 0; i < outputs.size(); i++) { // add output row to nextNodes
		nextNodes[outputs[i]] += outputRow[i];
	}
}

