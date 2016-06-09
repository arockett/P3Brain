//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "../ConstantValuesBrain/ConstantValuesBrain.h"

shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMinPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMin", 0.0, "Minmum value that brain will deliver");
shared_ptr<ParameterLink<double>> ConstantValuesBrain::valueMaxPL = Parameters::register_parameter("BRAIN_CONSTANT-valueMax", 100.0, "Maximum value that brain will deliver");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::valueTypePL = Parameters::register_parameter("BRAIN_CONSTANT-valueType", 0, "0 = int, 1 = double");
shared_ptr<ParameterLink<int>> ConstantValuesBrain::samplesPerValuePL = Parameters::register_parameter("BRAIN_CONSTANT-samplesPerValue", 1, "for each brain value, this many samples will be taken from genome and averaged");

ConstantValuesBrain::ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes, _PT) {

	//WorldY = (PT == nullptr) ? WorldYPL->lookup() : PT->lookupInt("WORLD_BERRY-worldY");

	valueMin = (PT == nullptr) ? valueMinPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMin");
	valueMax = (PT == nullptr) ? valueMaxPL->lookup() : PT->lookupDouble("BRAIN_CONSTANT-valueMax");
	valueType = (PT == nullptr) ? valueTypePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-valueType");
	samplesPerValue = (PT == nullptr) ? samplesPerValuePL->lookup() : PT->lookupInt("BRAIN_CONSTANT-samplesPerValue");

// columns to be added to ave file
	aveFileColumns.clear();
}

shared_ptr<AbstractBrain> ConstantValuesBrain::makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
	shared_ptr<ConstantValuesBrain> newBrain = make_shared<ConstantValuesBrain>(nrInNodes, nrOutNodes, nrHiddenNodes);
	auto genomeHandler = _genome->newHandler(_genome, true);

	double tempValue;

	for (int i = 0; i < nrOutNodes; i++) {
		tempValue = 0;
		for (int j = 0; j < samplesPerValue; j++) {
			if (valueType == 0) {
				tempValue += genomeHandler->readInt(valueMin, valueMax);
			} else if (valueType == 1) {
				tempValue += genomeHandler->readDouble(valueMin, valueMax);
			} else {
				cout << "  ERROR! BRAIN_CONSTANT-valueType is invalid. current value: " << valueType << endl;
				exit(1);
			}
		}
		if (valueType == 0) {
			newBrain->nodes[outputNodesList[i]] = int(tempValue / samplesPerValue);
			newBrain->nextNodes[outputNodesList[i]] = int(tempValue / samplesPerValue);
		}
		if (valueType == 1) {
			newBrain->nodes[outputNodesList[i]] = tempValue / samplesPerValue;
			newBrain->nextNodes[outputNodesList[i]] = tempValue / samplesPerValue;
		}
	}
	return newBrain;
}

void ConstantValuesBrain::resetBrain() {
// do nothing! values never change!
}

void ConstantValuesBrain::update() {
// do nothing! output is already set!
}

void inline ConstantValuesBrain::resetOutputs() {
// do nothing! output is already set!
}

string ConstantValuesBrain::description() {
	string S = "Constant Values Brain\n";
	return S;
}

vector<string> ConstantValuesBrain::getStats() {
	vector<string> dataPairs;
	dataPairs.push_back("outputValues");
	string valuesList = "\"[";
	for (int i = 0; i < nrOutNodes; i++) {
		valuesList += to_string(nextNodes[outputNodesList[i]]) + ",";
	}
	valuesList.pop_back();
	valuesList += "]\"";

	dataPairs.push_back(to_string(valuesList));

	return (dataPairs);
}

void ConstantValuesBrain::initalizeGenome(shared_ptr<AbstractGenome> _genome) {
	_genome->fillRandom();
}

