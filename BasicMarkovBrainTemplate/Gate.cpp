//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Gate.h"
#include <math.h>
#include <iostream>
#include <time.h>
//#include "Random.h"

bool& Gate::usingProbGate = Parameters::register_parameter("probGate", false, "set to true to enable probabilistic gates", "GATE TYPES");
bool& Gate::usingDetGate = Parameters::register_parameter("detGate", false, "set to true to enable deterministic gates?", "GATE TYPES");
bool& Gate::usingFBGate = Parameters::register_parameter("fbGate", false, "set to true to enable feedback gates", "GATE TYPES");
bool& Gate::usingGPGate = Parameters::register_parameter("gpGate", false, "set to true to enable GP (what?) gates", "GATE TYPES");
bool& Gate::usingThGate = Parameters::register_parameter("thGate", false, "set to true to enable theta gates", "GATE TYPES");
bool& Gate::usingEpsiGate = Parameters::register_parameter("epsiGate", false, "set to true to enable epsilon gates", "GATE TYPES");

double& Gate::voidOutPut = Parameters::register_parameter("voidOutput", 0.0,
		"chance that an output from a determinstic gate will be set to 0", "GATES");
/*
 * setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
 * there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
 * after initalizing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
 */
void Gate::setupGates() {
	for (int i = 0; i < 256; i++) {
		Gate::AddGate(i, NULL);
	}
	if (usingProbGate) {
		AddGate(42, [](Genome* genome,int pos) {shared_ptr<Gate> newGate(new Gate(genome,pos)); return newGate;});
		Data::inUseGateTypes.insert(42);
	}
	if (usingDetGate) {
		AddGate(43,
				[](Genome* genome,int pos) {shared_ptr<DeterministicGate> newGate(new DeterministicGate(genome,pos)); return newGate;});
		Data::inUseGateTypes.insert(43);
	}
	if (usingFBGate) {
		AddGate(44,
				[](Genome* genome,int pos) {shared_ptr<FeedbackGate> newGate(new FeedbackGate(genome,pos)); return newGate;});
		Data::inUseGateTypes.insert(44);
	}
	if (usingGPGate) {
		AddGate(45,
				[](Genome* genome,int pos) {shared_ptr<GPGate> newGate(new GPGate(genome,pos)); return newGate;});
		Data::inUseGateTypes.insert(45);
	}
	if (usingThGate) {
		AddGate(46,
				[](Genome* genome,int pos) {shared_ptr<Thresholdgate> newGate(new Thresholdgate(genome,pos)); return newGate;});
		Data::inUseGateTypes.insert(46);
	}
	if (usingEpsiGate) {
		AddGate(47,
				[](Genome* genome,int pos) {shared_ptr<FixedEpsilonGate> newGate(new FixedEpsilonGate(genome,pos)); return newGate;});
		Data::inUseGateTypes.insert(47);
	}
}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
function<shared_ptr<Gate>(Genome*, int)> Gate::makeGate[256];
void Gate::AddGate(int ID, function<shared_ptr<Gate>(Genome*, int)> theFunction) {
	makeGate[ID] = theFunction;
}

/* *** Gate implementation *** */
Gate::Gate() {
}

Gate::Gate(Genome *genome, int startCodonAt) {
	int i, j, k;
	inputs.clear();
	outputs.clear();
	int _xDim, _yDim;
	codingRegions.clear();

	//find the first nucleotide that isn't part of the start codon
	codingRegions[startCodonAt] = 0; //maps start codon
	codingRegions[(startCodonAt + 1) % (int) genome->sites.size()] = 0;
	k = (startCodonAt + 2) % (int) genome->sites.size(); //this makes the genome loop back around. k is the position in genome of the third nucleotide

			//get the dimensions of the table
	codingRegions[k] = 1; //maps values of third and fourth nucleotide
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3); //the third nucleotide, "bit and" 3 +1, is the x dimension
	codingRegions[k] = 1;
	_yDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3); //the fourth nucleotide, "bit and" 3 +1, is the y dimension

	//prepare the containers for the inputsand outputs addresses
	inputs.resize(_yDim); //inputsis the input vector with length yDim
	outputs.resize(_xDim); //outputs is the output vector with length xDim

	for (i = 0; i < _yDim; i++) {
		inputs[i] = (((int) genome->sites[(k + (i * 2)) % genome->sites.size()]) << 8)
				| ((int) genome->sites[(k + (i * 2) + 1) % genome->sites.size()]); //fills each input with 2 bytes from genome
		//cout << inputs.size() << "   " << i << " :: " << inputs[i] << "\n";
		codingRegions[(k + (i * 2)) % genome->sites.size()] = 1;
		codingRegions[(k + (i * 2) + 1) % genome->sites.size()] = 1;
	}
	for (i = 0; i < _xDim; i++) {
		outputs[i] = (((int) genome->sites[(k + 8 + (i * 2)) % genome->sites.size()]) << 8)
				| ((int) genome->sites[(k + 8 + (i * 2) + 1) % genome->sites.size()]); //fills each input with 2 bytes from genome
		//cout << outputs.size() << "   " << i << " -- " << outputs[i] << "\n";
		codingRegions[(k + 8 + (i * 2)) % genome->sites.size()] = 1;
		codingRegions[(k + 8 + (i * 2) + 1) % genome->sites.size()] = 1;
	}

	k = k + 32; //move forward 32 spaces in genome	//get all the values into the table
	table.resize(1 << _yDim); //table is a vector of vectors of doubles. this resizes the first level vector to 2^(number of inputs).
	for (i = 0; i < (1 << _yDim); i++) { //for each input vector...
		table[i].resize(1 << _xDim); //resize the second level vectors to 2^(number of outputs)
		double S = 0.0;
		for (j = 0; j < (1 << _xDim); j++) {
			table[i][j] = (double) genome->sites[(k + j + ((1 << _xDim) * i)) % genome->sites.size()]; //fills each spot in the probability table with the value of the
			//corresponding nucleotide
			codingRegions[(k + j + ((1 << _xDim) * i)) % genome->sites.size()] = 2;
		}

		//normalize the row
		if (S == 0.0) { //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
			for (j = 0; j < (1 << _xDim); j++)
				table[i][j] = 1.0 / (double) (1 << _xDim);
		} else { //otherwise divide all values in a row by the sum of the row
			for (j = 0; j < (1 << _xDim); j++)
				table[i][j] /= S;
		}
	}

}

Gate::~Gate() { //this can delete gates
}

int Gate::Bit(double d) { //this returns 1 if you have inputted a number greater than 0
	if (d > 0.0)
		return 1;
	return 0;
}

void Gate::update(vector<double> & states, vector<double> & nextStates) { //this translates the input bits of the current states to the output bits of the next states
	int input = 0, output = 0;
	double r = (double) rand() / (double) RAND_MAX; //r is a random double between 0 and 1
	for (size_t i = 0; i < inputs.size(); i++) //for everything in vector inputs...
		input = (input << 1) + Bit(states[inputs[i]]); //stores the current states in bitstring input with bitshifting. it will now be a bitstring of length (inputs)
	while (r > table[input][output]) { //this goes across the probability table in row (input) and subtracts each value from r until r is less than a value it reaches
		r -= table[input][output];
		output++;
	}
	for (size_t i = 0; i < outputs.size(); i++) //for each number in vector output...
		nextStates[outputs[i]] += 1.0 * ((output >> i) & 1); //carries the output selected by r into the output of nextStates
}

string Gate::description() {
	string S = "Gate\n";
	S = S + "IN:";
	for (size_t i = 0; i < inputs.size(); i++)
		S = S + " " + to_string(inputs[i]);
	S = S + "\n";
	S = S + "OUT:";
	for (size_t i = 0; i < outputs.size(); i++)
		S = S + " " + to_string(outputs[i]);
	S = S + "\n";
	return S;
}

/*
 * converts values attained from genome for inputs and outputs to vaild brain state ids
 * uses nodeMap to accomplish the remaping
 */
void Gate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
	//cout << "in Gate::applyNodeMap\n";
	for (size_t i = 0; i < inputs.size(); i++) {
		//cout << inputs[i] << "%16  ->   ";
		//cout << nodeMap[inputs[i]] << " ";
		inputs[i] = nodeMap[inputs[i]] % maxNodes;
		//cout << inputs[i] << "\n";
	}
	for (size_t i = 0; i < outputs.size(); i++) {
		//cout << outputs[i] << "%16   ->   ";
		//cout << nodeMap[outputs[i]] << " ";
		outputs[i] = nodeMap[outputs[i]] % maxNodes;
		//cout << outputs[i] << "\n";

	}
	//cout << "leaving Gate::applyNodeMap\n";
}

void Gate::resetGate(void) {
	//nothing to reset here!
}

vector<int> Gate::getIns() {
	return inputs;
}

vector<int> Gate::getOuts() {
	return outputs;
}

/* *** Determistic Gate Implementation *** */
DeterministicGate::DeterministicGate() {
}

DeterministicGate::DeterministicGate(Genome *genome, int startCodonAt) { //all this looks exactly the same as

	//cout << "in DeterministicGate constructor\n";
	int i, j, k;
	inputs.clear();
	outputs.clear();
	int _xDim, _yDim;
	codingRegions.clear();

	codingRegions[startCodonAt] = 0;
	codingRegions[startCodonAt + 1] = 0;
	//find the first nucleotide that isn't part of the start codon
	k = (startCodonAt + 2) % (int) genome->sites.size();

	//get the dimensions of the table
	codingRegions[k] = 1;
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
	//cout << "The number of outputs is " << _xDim << "\n";
	codingRegions[k] = 1;
	_yDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
	//cout << "The number of inputs is " << _yDim << "\n";

	//prepare the containers for the inputsand outputs addresses
	inputs.resize(_yDim);
	outputs.resize(_xDim);

	for (i = 0; i < _yDim; i++) {
		inputs[i] = (((int) genome->sites[(k + (i * 2)) % genome->sites.size()]) << 8)
				| ((int) genome->sites[(k + (i * 2) + 1) % genome->sites.size()]); //fills each input with 2 bytes from genome
		//cout << inputs.size() << "   " << i << " :: " << inputs[i] << "\n";
		codingRegions[(k + (i * 2)) % genome->sites.size()] = 1;
		codingRegions[(k + (i * 2) + 1) % genome->sites.size()] = 1;
	}
	for (i = 0; i < _xDim; i++) {
		outputs[i] = (((int) genome->sites[(k + 8 + (i * 2)) % genome->sites.size()]) << 8)
				| ((int) genome->sites[(k + 8 + (i * 2) + 1) % genome->sites.size()]); //fills each input with 2 bytes from genome
		//cout << outputs.size() << "   " << i << " -- " << outputs[i] << "\n";
		codingRegions[(k + 8 + (i * 2)) % genome->sites.size()] = 1;
		codingRegions[(k + 8 + (i * 2) + 1) % genome->sites.size()] = 1;
	}

	k = k + 32; //move forward 32 spaces in genome
	//get all the values into the table
	table.resize(1 << _yDim); //the y dimension of the table is still a list of binary 2^(number of inputs)
	for (i = 0; i < (1 << _yDim); i++) {
		table[i].resize(_xDim);
		for (j = 0; j < _xDim; j++) { //the outputs of the table (corresponding to where in the new brainstate the output bits go) are just 0-(number of outputs)
			table[i][j] = 1.0 * (double) (genome->sites[(k + j + (_xDim * i)) % genome->sites.size()] & 1); //if the number of each space in the logic table is odd, a 1 goes there. if even, a 0
			codingRegions[(k + j + (_xDim * i)) % genome->sites.size()] = 2;
			//cout << codingRegions[(k+j+(_xDim*i))%genome->sites.size()].first;
		}
	}
	//cout << "leaving DeterministicGate constructor\n";

}

void DeterministicGate::setupForBits(int* Ins, int nrOfIns, int Out, int logic) {
	inputs.resize(nrOfIns);
	for (int i = 0; i < nrOfIns; i++)
		inputs[i] = Ins[i];
	outputs.resize(1);
	outputs[0] = Out;
	table.resize(1 << nrOfIns);
	for (int i = 0; i < (1 << nrOfIns); i++) {
		table[i].resize(1);
		table[i][0] = (logic >> i) & 1;
	}
}

#define VOIDOUTPUT 1

DeterministicGate::~DeterministicGate() {
}

void DeterministicGate::update(vector<double> & states, vector<double> & nextStates) {
	int input = 0;
	for (size_t i = 0; i < inputs.size(); i++) {
		input = (input << 1) + Bit(states[inputs[i]]); //stores current input states in bistring "input"
	}
	vector<double> outputRow = table[input];
#if VOIDOUTPUT==1
	//if(*Data::parameterDouble["voidOutput"]>0){
	if (((double) rand() / (double) RAND_MAX) <= Gate::voidOutPut) {
		int whichBit = rand() % outputs.size();
		outputRow[whichBit] = 0;
	}
	//}
#endif // VOIDOUTPUT
	for (size_t i = 0; i < outputs.size(); i++) {
		nextStates[outputs[i]] += outputRow[i];
	}
}

string DeterministicGate::description() {
	return "Deterministic " + Gate::description();
}

/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */

/* *** GP Gate implementation *** */
union intToFloatBitByBit {
	int I;
	float F;
};

GPGate::GPGate() {

}

GPGate::GPGate(Genome *genome, int startCodonAt) {
	int i, k;
	inputs.clear();
	outputs.clear();
	int _xDim, _yDim;

	//find the first nucleotide that isn't part of the start codon
	k = (startCodonAt + 2) % (int) genome->sites.size();

	//get the dimensions of the table
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
	_yDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);

	//prepare the containers for the inputsand outputs addresses
	inputs.resize(_yDim);
	outputs.resize(_xDim);

	for (i = 0; i < _yDim; i++)
		inputs[i] = genome->sites[(k + i) % genome->sites.size()];
	for (i = 0; i < _xDim; i++)
		outputs[i] = genome->sites[(k + 4 + i) % genome->sites.size()];
	k += 8;
	myGateType = genome->sites[(k++) % genome->sites.size()] % 8;
	myValues.clear();
	for (i = 0; i < _xDim; i++) {
		intToFloatBitByBit V;
		V.I = 0;
		for (int j = 0; j < 4; j++)
			V.I = (V.I << (int) 8) + (int) genome->sites[(k++) % genome->sites.size()];
		myValues.push_back(V.F);
	}
}

GPGate::~GPGate() {

}

void GPGate::update(vector<double> & states, vector<double> & nextStates) {
	double retValue = inputs[0];
	int Z = 0;
	size_t i, o;
	bool convReturn = true;
	switch (myGateType) {
	case 0: //constant
		convReturn = false;
		for (o = 0; o < outputs.size(); o++)
			nextStates[outputs[o]] = myValues[o];
		break;
	case 1: // add
		for (i = 1; i < inputs.size(); i++)
			retValue += states[inputs[i]];
		break;
	case 2: // sub
		for (i = 1; i < inputs.size(); i++)
			retValue -= states[inputs[i]];
		break;
	case 3: // mult
		for (i = 1; i < inputs.size(); i++)
			retValue *= states[inputs[i]];
		break;
	case 4: // div
		for (i = 1; i < inputs.size(); i++)
			retValue /= states[inputs[i]];
		break;
	case 5: // sin
		convReturn = false;
		for (o = 0; o < outputs.size(); o++) {
			nextStates[outputs[o]] += sin(states[inputs[Z]]);
			Z = (Z + 1) % (int) inputs.size();
		}
		break;
	case 6: // cos
		convReturn = false;
		for (o = 0; o < outputs.size(); o++) {
			nextStates[outputs[o]] += cos(states[inputs[Z]]);
			Z = (Z + 1) % (int) inputs.size();
		}
		break;
	case 7: // log
		convReturn = false;
		for (o = 0; o < outputs.size(); o++) {
			if (inputs[Z] > 0.0)
				nextStates[outputs[o]] += log(states[inputs[Z]]);
			Z = (Z + 1) % (int) inputs.size();
		}
		break;
	case 8: // exp
		convReturn = false;
		for (o = 0; o < outputs.size(); o++) {
			nextStates[outputs[o]] += exp(states[inputs[Z]]);
			Z = (Z + 1) % (int) inputs.size();
		}
		break;
	}
	if (convReturn) {
		for (size_t o = 0; o < outputs.size(); o++)
			nextStates[outputs[o]] += retValue;
	}
}

string GPGate::description() {
	string gateTypeName[8] = { "+", "-", "*", "/", "Sin", "Cos", "Log", "Exp" };
	return "GP " + gateTypeName[myGateType] + "\n" + Gate::description();
}

/* *** Feedback Gate *** */
bool FeedbackGate::feedbackON = true;

FeedbackGate::FeedbackGate() {

}

FeedbackGate::FeedbackGate(Genome *genome, int startCodonAt) {
	int i, j, k;
	inputs.clear();
	outputs.clear();
	int _xDim, _yDim;

	//find the first nucleotide that isn't part of the start codon
	k = (startCodonAt + 2) % (int) genome->sites.size();

	//get the dimensions of the table
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
	_yDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);

	//get the dimensions of the feedback buffer
	posFBNode = genome->sites[(k++) % genome->sites.size()];
	negFBNode = genome->sites[(k++) % genome->sites.size()];
	nrPos = genome->sites[(k++) % genome->sites.size()] & 3;
	nrNeg = genome->sites[(k++) % genome->sites.size()] & 3;

	//prepare the containers for the inputsand outputs addresses
	inputs.resize(_yDim);
	outputs.resize(_xDim);

	//prepare the containers for the feedback pipelines
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);

	//get the Ioutputs addresses
	for (i = 0; i < _yDim; i++)
		inputs[i] = genome->sites[(k + i) % genome->sites.size()];
	for (i = 0; i < _xDim; i++)
		outputs[i] = genome->sites[(k + 4 + i) % genome->sites.size()];

	//get the Feedback forces
	for (i = 0; i < nrPos; i++)
		posLevelOfFB[i] = ((double) (1 + genome->sites[(k + 8 + i) % genome->sites.size()])) / 256.0;
	for (i = 0; i < nrNeg; i++)
		negLevelOfFB[i] = ((double) (1 + genome->sites[(k + 12 + i) % genome->sites.size()])) / 256.0;

	k = k + 16;
	//get all the values into the table
	table.resize(1 << _yDim);
	originalTable.resize(1 << _yDim);
	for (i = 0; i < (1 << _yDim); i++) {
		table[i].resize(1 << _xDim);
		originalTable[i].resize(1 << _xDim);
		double S = 0.0;
		for (j = 0; j < (1 << _xDim); j++) {
			table[i][j] = (double) genome->sites[(k + j + ((1 << _xDim) * i)) % genome->sites.size()];
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
FeedbackGate::~FeedbackGate() {

}

void FeedbackGate::update(vector<double> & states, vector<double> & nextStates) {
	size_t i;
	double mod;

	//Apply the feedback
	if ((feedbackON) && (nrPos != 0) && (states[posFBNode] > 0.0)) {
		for (i = 0; i < chosenInPos.size(); i++) {
			mod = ((double) rand() / (double) RAND_MAX) * posLevelOfFB[i];
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
			mod = ((double) rand() / (double) RAND_MAX) * negLevelOfFB[i];
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
	double r = (double) rand() / (double) RAND_MAX;
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
	string S = "pos node:" + to_string(posFBNode) + "\n neg node:" + to_string(negFBNode);
	return "Feedback Gate\n " + S + "\n" + Gate::description();
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

/* *** Threshold Gate *** */
Thresholdgate::Thresholdgate() {

}

Thresholdgate::Thresholdgate(Genome *genome, int startCodonAt) {
	int i, k;
	inputs.clear();
	outputs.clear();
	int _xDim;

	//find the first nucleotide that isn't part of the start codon
	k = (startCodonAt + 2) % (int) genome->sites.size();

	//The dimensions are different here, the first inputsis the trigger
	//the next one are the current state Counter
	//the outputSize is the same, the first bit of the output is the ping
	//the others are the current state counter, they are the same as the inputs!
	//get the dimensions of the table
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 7);

	//prepare the containers for the inputsand outputs addresses
	inputs.resize(_xDim);
	outputs.resize(_xDim);

	inputs[0] = genome->sites[k % genome->sites.size()];
	for (i = 1; i < _xDim; i++) {
		inputs[i] = genome->sites[(k + i) % genome->sites.size()];
		outputs[i] = genome->sites[(k + i) % genome->sites.size()];
	}
	//keep calm, i is at the right value
	outputs[0] = genome->sites[(k + i) % genome->sites.size()];

	//yes the threshold can be higher than the maximal number countable to by this threshold counter
	threshold = genome->sites[(k + i + 1) % genome->sites.size()];
}

Thresholdgate::~Thresholdgate() {

}

void Thresholdgate::update(vector<double> & states, vector<double> & nextStates) {
	int theCount = 0;
	for (size_t i = 1; i < inputs.size(); i++)
		theCount = (theCount << 1) + Bit(states[inputs[i]]);
	theCount += Bit(states[inputs[0]]);
	if (theCount > threshold)
		states[outputs[0]] += 1.0;
	for (size_t i = 1; i < outputs.size(); i++)
		nextStates[outputs[i]] += 1.0 * ((theCount >> (i - 1)) & 1);
}

string Thresholdgate::description() {
	return "ThresholdgateGate: " + to_string(threshold) + "\n" + Gate::description();;
}

/* *** Fixed Epison Gate *** */
/* this gate behaves like a deterministic gate with a constant externally set error */

double FixedEpsilonGate::epsilon = 0.0;

FixedEpsilonGate::FixedEpsilonGate() {

}

FixedEpsilonGate::FixedEpsilonGate(Genome *genome, int startCodonAt) {
	int i, j, k;
	inputs.clear();
	outputs.clear();
	int _xDim, _yDim;
	codingRegions.clear();

	codingRegions[startCodonAt] = 0; //maps start codon
	codingRegions[(startCodonAt + 1) % genome->sites.size()] = 0;
	//find the first nucleotide that isn't part of the start codon
	k = (startCodonAt + 2) % (int) genome->sites.size();

	//get the dimensions of the table
	codingRegions[k] = 1; //maps values of third and fourth nucleotide
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
	//cout << "The number of outputs is " << _xDim << "\n";
	codingRegions[k] = 1;
	_yDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 3);
	//cout << "The number of inputs is " << _yDim << "\n";

	//prepare the containers for the inputs and outputs addresses
	inputs.resize(_yDim);
	outputs.resize(_xDim);

	for (i = 0; i < _yDim; i++) {
		codingRegions[(k + i) % genome->sites.size()] = 1; //maps these values
		inputs[i] = genome->sites[(k + i) % genome->sites.size()]; //fill vector I
		//cout << k+i << " is an index of an input\n";
	}
	for (i = 0; i < _xDim; i++) {
		codingRegions[(k + 4 + i) % genome->sites.size()] = 1; //maps these values
		outputs[i] = genome->sites[(k + 4 + i) % genome->sites.size()]; //fill vector O
		//cout << k+4+i << " is an index of an output\n";
	}

	k = k + 16;
	//get all the values into the table
	table.resize(1 << _yDim); //there are 2^number of inputs rows
	for (i = 0; i < (1 << _yDim); i++) {
		table[i].resize(_xDim); //there are (number of outputs) spaces per row
		for (j = 0; j < _xDim; j++) {
			table[i][j] = 1.0 * (double) (genome->sites[(k + j + (_xDim * i)) % genome->sites.size()] & 1);
			codingRegions[(k + j + (_xDim * i)) % genome->sites.size()] = 2;
			//cout << "The above index is part of the data table\n";
		}
	}
	//now to the specifics of this gate
	defaultOutput.clear(); //clear the vector<int>
	for (size_t i = 0; i < table.size(); i++) { //for every row of the table (which is 2^inputs)
		int D = 0;
		for (size_t j = 0; j < table[i].size(); j++) { //for every space in the row
			D |= (Bit(table[i][j]) << j); //translate the value in that space to a bit (0 or 1), then bitshift it [position in row] times, then take D, bit-or it with that number,
			//and make that the new D
		}
		defaultOutput.push_back(D); //add D (which is a bitstring of length [2^outputs]) to defaultOutput. defaultoutputs is now of length (2^number of inputs)
	}
}

FixedEpsilonGate::~FixedEpsilonGate() {

}

void FixedEpsilonGate::update(vector<double> & states, vector<double> & nextStates) {
	int input = 0; //input is the correct bitstring of outputs of the gate
	for (size_t i = 0; i < inputs.size(); i++) //for every index in the input (I) vector
		input = (input << 1) + Bit(states[inputs[i]]); //stores current states in bitstring input
	if (((double) rand() / (double) RAND_MAX) < epsilon) { //if there will be an error
		//do a random output excluding the one otherwise given
		int output = 0;
		do {
			output = rand() & (table.size() - 1); //pick a bitstring (random row in the table)
		} while (output == defaultOutput[input]); //until you pick the correct bitstring of outputs of the gate. If you don't, move on.
		for (size_t i = 0; i < outputs.size(); i++) {
			nextStates[outputs[i]] += 1.0 * (double) ((output >> i) & 1);
		}
	} else {
		//do the default output
		for (size_t i = 0; i < outputs.size(); i++) {
			nextStates[outputs[i]] += table[input][i];
		}
	}
}

string FixedEpsilonGate::description() {
	return "Fixed Epsilon " + to_string(epsilon) + " " + Gate::description();
}

