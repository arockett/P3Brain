//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "GPGate.h"

#include <iostream>

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"


const double& GPGate::constValueMin = Parameters::register_parameter("gpGate_constValueMin", -1, "for the const values, min value the genome can generate", "GATES - GP");
const double& GPGate::constValueMax = Parameters::register_parameter("gpGate_constValueMax", 1, "for the const values, max value the genome can generate", "GATES - GP");

/* *** GP Gate implementation *** */

//GPGate::GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
GPGate::GPGate(pair<vector<int>, vector<int>> _addresses, int _operation, vector<double> _constValues, int gateID) {

	ID = gateID;

	inputs = _addresses.first;
	outputs = _addresses.second;

	operation = _operation;
	constValues = _constValues;
}

void GPGate::update(vector<double> & states, vector<double> & nextStates) {
	double retValue = inputs[0];
	int index = 0;
	size_t i, o;
	bool writeValueAtEnd = true;
	switch (operation) {
		case 0:  //constant
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++)
				nextStates[outputs[o]] = constValues[o];
			break;
		case 1:  // add
			for (i = 1; i < inputs.size(); i++)
				retValue += states[inputs[i]];
			break;
		case 2:  // sub
			for (i = 1; i < inputs.size(); i++)
				retValue -= states[inputs[i]];
			break;
		case 3:  // mult
			for (i = 1; i < inputs.size(); i++)
				retValue *= states[inputs[i]];
			break;
		case 4:  // div
			for (i = 1; i < inputs.size(); i++)
				retValue /= states[inputs[i]];
			break;
		case 5:  // sin
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				nextStates[outputs[o]] += sin(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
		case 6:  // cos
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				nextStates[outputs[o]] += cos(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
		case 7:  // log
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				if (inputs[index] > 0.0)
					nextStates[outputs[o]] += log(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
		case 8:  // exp
			writeValueAtEnd = false;
			for (o = 0; o < outputs.size(); o++) {
				nextStates[outputs[o]] += exp(states[inputs[index]]);
				index = (index + 1) % (int) inputs.size();
			}
			break;
	}
	if (writeValueAtEnd) {
		for (size_t o = 0; o < outputs.size(); o++)
			nextStates[outputs[o]] += retValue;
	}
}

string GPGate::description() {
	string gateTypeName[9] = { "fixed constants", "+", "-", "*", "/", "Sin", "Cos", "Log", "Exp" };
	string constString = " constants: " + to_string(constValues[0]) + " " + to_string(constValues[1]) + " "+ to_string(constValues[2]) + " " + to_string(constValues[3]) + "\n";
	return "Gate " + to_string(ID) + " is a (" + gateTypeName[operation] + ") " + gateType() + "Gate\n" + AbstractGate::descriptionIO() + constString;
}

