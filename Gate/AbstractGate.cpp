//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "AbstractGate.h"

#include <iostream>

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

// *** General tools for All Gates ***

// converts values attained from genome for inputs and outputs to vaild brain state ids
// uses nodeMap to accomplish the remaping
void AbstractGate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
	for (size_t i = 0; i < inputs.size(); i++) {
		inputs[i] = nodeMap[inputs[i]] % maxNodes;
	}
	for (size_t i = 0; i < outputs.size(); i++) {
		outputs[i] = nodeMap[outputs[i]] % maxNodes;
	}
}

void AbstractGate::resetGate() {
	//nothing to reset here!
}

vector<int> AbstractGate::getIns() {
	return inputs;
}

vector<int> AbstractGate::getOuts() {
	return outputs;
}

string AbstractGate::descriptionIO() {
	string S = "IN:";
	for (size_t i = 0; i < inputs.size(); i++)
		S = S + " " + to_string(inputs[i]);
	S = S + "\n";
	S = S + "OUT:";
	for (size_t i = 0; i < outputs.size(); i++)
		S = S + " " + to_string(outputs[i]);
	S = S + "\n";
	//S = S + getCodingRegions();
	return S;
}

string AbstractGate::description() {
	return "Gate " + to_string(ID) + " is a " + gateType() + "Gate\n" + descriptionIO();
}
