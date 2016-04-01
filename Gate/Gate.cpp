//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

const double& FixedEpsilonGate::FixedEpsilonGate_Probability = Parameters::register_parameter("fixedEpsilonGate_Probability", 0.05, "chance that an output from a FixedEpsilonGate gate will be randomized", "GATES - FIXED EPSILON");
const double& VoidGate::voidGate_Probability = Parameters::register_parameter("voidGate_Probability", 0.05, "chance that an output from a void gate will be set to 0", "GATES - VOID");

// *** General tools for All Gates ***

// converts values attained from genome for inputs and outputs to vaild brain state ids
// uses nodeMap to accomplish the remaping
void Gate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
	for (size_t i = 0; i < inputs.size(); i++) {
		inputs[i] = nodeMap[inputs[i]] % maxNodes;
	}
	for (size_t i = 0; i < outputs.size(); i++) {
		outputs[i] = nodeMap[outputs[i]] % maxNodes;
	}
}

void Gate::resetGate() {
	//nothing to reset here!
}

vector<int> Gate::getIns() {
	return inputs;
}

vector<int> Gate::getOuts() {
	return outputs;
}

string Gate::descriptionIO() {
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

string Gate::description() {
	return "Gate " + to_string(ID) + " is a " + gateType() + "Gate\n" + descriptionIO();
}

/* *** ProbilisticGate implementation *** */

ProbabilisticGate::ProbabilisticGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> rawTable, int _ID) {
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

/* *** Determistic Gate Implementation *** */

DeterministicGate::DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID) {
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

/* *** Fixed Epison Gate *** */
/* this gate behaves like a deterministic gate with a constant externally set error which may the gate to deliver the wrong set of output values */

FixedEpsilonGate::FixedEpsilonGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID) :
		DeterministicGate(addresses, _table, _ID) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
	epsilon = FixedEpsilonGate_Probability;  // in case you want to have different epsilon for different gates (who am I to judge?)

	// now to the specifics of this gate - we convert the table to a list of numbers (i.e. bitstrings) so we can do fast comparisons in the update
	defaultOutput.clear();  // clear the defaultOutput vector... the list of bitstrings
	for (size_t i = 0; i < table.size(); i++) {  //for every row of the table (which is 2^inputs)
		int D = 0;
		for (size_t j = 0; j < table[i].size(); j++) {  //for every space in the row
			D |= (Bit(table[i][j]) << j);  //translate the value in that space to a bit (0 or 1), then bitshift it [position in row] times, then take D, bit-or it with that number,
			//and make that the new D
		}
		defaultOutput.push_back(D);  //add D (which is a bitstring of length [2^outputs]) to defaultOutput. defaultoutputs is now of length (2^number of inputs)
	}
}

void FixedEpsilonGate::update(vector<double> & nodes, vector<double> & nextNodes) {
	int input = vectorToBitToInt(nodes,inputs,true); // converts the input values into an index (true indicates to reverse order)

	if (Random::P(epsilon)) {  //if chance... then return a different set of output values
		int output = 0;
		do {
			output = Random::getIndex(table.size());
		} while (output == defaultOutput[input]);  // keep picking randomly until you pick a different output
		for (size_t i = 0; i < outputs.size(); i++) {
			nextNodes[outputs[i]] += 1.0 * (double) ((output >> i) & 1);
		}
	} else {
		//do the default output
		for (size_t i = 0; i < outputs.size(); i++) {
			nextNodes[outputs[i]] += table[input][i];
		}
	}
}

/* *** VoidGate *** */
/* this gate behaves like a deterministic gate with a constant externally set error which may set a single output to 0 */

VoidGate::VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID) :
		DeterministicGate(addresses, _table, _ID) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
	epsilon = voidGate_Probability;  // in case you want to have different epsilon for different gates (who am I to judge?)
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

