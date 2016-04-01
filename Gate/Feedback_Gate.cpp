//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"

#include "Feedback_Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

bool FeedbackGate::feedbackON = true;

FeedbackGate::FeedbackGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {

	ID = gateID;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// need to move to new Get Address
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int i, j;
	inputs.clear();
	outputs.clear();
	int _xDim, _yDim;

	//get the dimensions of the table
	_xDim = genomeHandler->readInt(1, 4, Gate::IN_COUNT_CODE, gateID);
	_yDim = genomeHandler->readInt(1, 4, Gate::OUT_COUNT_CODE, gateID);

	//get the dimensions of the feedback buffer
	//was :
	//posFBNode = genome->sites[(k++) % genome->sites.size()];
	//negFBNode = genome->sites[(k++) % genome->sites.size()];
	//nrPos = genome->sites[(k++) % genome->sites.size()] & 3;
	//nrNeg = genome->sites[(k++) % genome->sites.size()] & 3;
	posFBNode = genomeHandler->readInt(0, 255, Gate::DATA_CODE, gateID);
	negFBNode = genomeHandler->readInt(0, 255, Gate::DATA_CODE, gateID);
	nrPos = genomeHandler->readInt(0, 3, Gate::DATA_CODE, gateID);
	nrNeg = genomeHandler->readInt(0, 3, Gate::DATA_CODE, gateID);

	//prepare the containers for the inputsand outputs addresses
	inputs.resize(_yDim);
	outputs.resize(_xDim);

	//prepare the containers for the feedback pipelines
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);

	//get the Ioutputs addresses

	for (i = 0; i < _yDim; i++) {
		inputs[i] = genomeHandler->readInt( 0, 255, Gate::IN_ADDRESS_CODE, gateID);
	}
	for (; i < 4; i++) {
		genomeHandler->readInt(0, 255 );
	}
	for (i = 0; i < _xDim; i++) {
		outputs[i] = genomeHandler->readInt(0, 255, Gate::OUT_ADDRESS_CODE, gateID);
	}
	for (; i < 4; i++) {
		genomeHandler->readInt(0, 255);
	}

	//get the Feedback forces
	for (i = 0; i < nrPos; i++) {
		posLevelOfFB[i] = ((double) (1 + genomeHandler->readInt(0, 255, Gate::DATA_CODE, gateID))) / 256.0;
	}
	for (; i < 4; i++) {
		genomeHandler->readInt( 0, 255 );
	}
	for (i = 0; i < nrNeg; i++) {
		negLevelOfFB[i] = ((double) (1 + genomeHandler->readInt(0, 255, Gate::DATA_CODE, gateID))) / 256.0;
	}
	for (; i < 4; i++) {
		genomeHandler->readInt(0, 255 );
	}

	//get all the values into the table
	table.resize(1 << _yDim);
	originalTable.resize(1 << _yDim);
	for (i = 0; i < (1 << _yDim); i++) {
		table[i].resize(1 << _xDim);
		originalTable[i].resize(1 << _xDim);
		double S = 0.0;
		for (j = 0; j < (1 << _xDim); j++) {
			table[i][j] = (double) genomeHandler->readInt(0, 255, Gate::DATA_CODE, gateID);
			S += table[i][j];
		}
		//normalize the row
		if (S == 0.0) {
			for (j = 0; j < (1 << _xDim); j++)
				table[i][j] = 1.0 / (double) (1 << _xDim);
		} else {
			for (j = 0; j < (1 << _xDim); j++)
				table[i][j] /= S;
		}
		for (j = 0; j < (1 << _xDim); j++)
			originalTable[i][j] = table[i][j];
	}

	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
}

void FeedbackGate::update(vector<double> & states, vector<double> & nextStates) {
	size_t i;
	double mod;

	//Apply the feedback
	if ((feedbackON) && (nrPos != 0) && (states[posFBNode] > 0.0)) {
		for (i = 0; i < chosenInPos.size(); i++) {
			mod = Random::getDouble(1) * posLevelOfFB[i];
			table[chosenInPos[i]][chosenOutPos[i]] += mod;
			double s = 0.0;
			for (size_t k = 0; k < table[chosenInPos[i]].size(); k++)
				s += table[chosenInPos[i]][k];
			for (size_t k = 0; k < table[chosenInPos[i]].size(); k++)
				table[chosenInPos[i]][k] /= s;
		}
	}
	if ((feedbackON) && (nrNeg != 0) && (states[negFBNode] > 0.0)) {
		for (i = 0; i < chosenInNeg.size(); i++) {
			mod = Random::getDouble(1) * negLevelOfFB[i];
			table[chosenInNeg[i]][chosenOutNeg[i]] -= mod;
			if (table[chosenInNeg[i]][chosenOutNeg[i]] < 0.001)
				table[chosenInNeg[i]][chosenOutNeg[i]] = 0.001;
			double s = 0.0;
			for (size_t k = 0; k < table[chosenInNeg[i]].size(); k++)
				s += table[chosenInNeg[i]][k];
			for (size_t k = 0; k < table[chosenInNeg[i]].size(); k++)
				table[chosenInNeg[i]][k] /= s;
		}
	}

	//do the logic of the gate
	int input = 0;
	int output = 0;
	double r = Random::getDouble(1);
	for (size_t i = 0; i < inputs.size(); i++)
		input = (input << 1) + Bit(states[inputs[i]]);
	while (r > table[input][output]) {
		r -= table[input][output];
		output++;
	}
	for (size_t i = 0; i < outputs.size(); i++)
		nextStates[outputs[i]] += 1.0 * ((output >> i) & 1);

	//remember the last actions for future feedback
	if (feedbackON) {
		chosenInPos.push_back(input);
		chosenInNeg.push_back(input);
		chosenOutPos.push_back(output);
		chosenOutNeg.push_back(output);
		while (chosenInPos.size() > nrPos) {
			chosenInPos.pop_front();
		}
		while (chosenOutPos.size() > nrPos) {
			chosenOutPos.pop_front();
		}
		while (chosenInNeg.size() > nrNeg) {
			chosenInNeg.pop_front();
		}
		while (chosenOutNeg.size() > nrNeg) {
			chosenOutNeg.pop_front();
		}
	}
}

string FeedbackGate::description() {
	string S = "pos node:" + to_string((int) posFBNode) + "\n neg node:" + to_string((int) negFBNode) + "\n";
	return Gate::description() + S;
}

void FeedbackGate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
	Gate::applyNodeMap(nodeMap, maxNodes);
	posFBNode = nodeMap[posFBNode % maxNodes];
	negFBNode = nodeMap[negFBNode % maxNodes];
}

void FeedbackGate::resetGate() {
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
	for (size_t i = 0; i < table.size(); i++)
		for (size_t j = 0; j < table[i].size(); j++)
			table[i][j] = originalTable[i][j];
}

vector<int> FeedbackGate::getIns() {
	vector<int> R;
	R.insert(R.begin(), inputs.begin(), inputs.end());
	R.push_back(posFBNode);
	R.push_back(negFBNode);
	return R;
}
