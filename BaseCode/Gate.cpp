//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>
//#include <time.h>

#include "Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

#define VOIDOUTPUT 0

bool& Gate::usingProbGate = Parameters::register_parameter("probGate", false,
		"set to true to enable probabilistic gates", "GATE TYPES");
bool& Gate::usingDetGate = Parameters::register_parameter("detGate", false,
		"set to true to enable deterministic gates?", "GATE TYPES");
bool& Gate::usingFBGate = Parameters::register_parameter("fbGate", false, "set to true to enable feedback gates",
		"GATE TYPES");
bool& Gate::usingGPGate = Parameters::register_parameter("gpGate", false, "set to true to enable GP (what?) gates",
		"GATE TYPES");
bool& Gate::usingThGate = Parameters::register_parameter("thGate", false, "set to true to enable theta gates",
		"GATE TYPES");
bool& Gate::usingEpsiGate = Parameters::register_parameter("epsiGate", false, "set to true to enable epsilon gates",
		"GATE TYPES");

#if VOIDOUTPUT==1
double& Gate::voidOutPut = Parameters::register_parameter("voidOutput", 0.0,
		"chance that an output from a determinstic gate will be set to 0", "GATES");
#endif // VOIDOUTPUT

double& Gate::FixedEpsilonGateP = Parameters::register_parameter("FixedEpsilonGateProb", 0.05,
		"chance that an output from a FixedEpsilonGate gate will be randomized", "GATES");

/*
 * setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
 * there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
 * after initializing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
 */
void Gate::setupGates() {
	for (int i = 0; i < 256; i++) {
		Gate::AddGate(i, NULL);
	}
	if (usingProbGate) {
		AddGate(42, [](Genome* genome,int pos) {return make_shared<Gate>(genome,pos);});
		Global::inUseGateTypes.insert(42);
	}
	if (usingDetGate) {
		//AddGate(43, [](Genome* genome,int pos) {return make_shared<DeterministicGate>(genome,pos);});
		AddGate(43, [](Genome* genome,int pos) {return make_shared<FixedEpsilonGate>(genome,pos);});
		Global::inUseGateTypes.insert(43);
	}
	if (usingFBGate) {
		AddGate(44, [](Genome* genome,int pos) {return make_shared<FeedbackGate>(genome,pos);});
		Global::inUseGateTypes.insert(44);
	}
	if (usingGPGate) {
		AddGate(45, [](Genome* genome,int pos) {return make_shared<GPGate>(genome,pos);});
		Global::inUseGateTypes.insert(45);
	}
	if (usingThGate) {
		AddGate(46, [](Genome* genome,int pos) {return make_shared<Thresholdgate>(genome,pos);});
		Global::inUseGateTypes.insert(46);
	}
	if (usingEpsiGate) {
		AddGate(47, [](Genome* genome,int pos) {return make_shared<FixedEpsilonGate>(genome,pos);});
		Global::inUseGateTypes.insert(47);
	}
}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
function<shared_ptr<Gate>(Genome*, int)> Gate::makeGate[256];
void Gate::AddGate(int ID, function<shared_ptr<Gate>(Genome*, int)> theFunction) {
	makeGate[ID] = theFunction;
}

/* *** Gate implementation *** */

Gate::Gate(Genome *genome, int genomeIndex) {
	int i, j;
	inputs.clear();
	outputs.clear();
	int numOutputs, numInputs;
	codingRegions.clear();

	//move past the first to sites (start codeon)
	movePastStartCodeon(genomeIndex, genome);

	// get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
	getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome); // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
	numInputs = inputs.size();
	numOutputs = outputs.size();

	genome->advanceIndex(genomeIndex, 16);

	// get a table filled with values from the genome that has
	// rows = (the number of possible combinations of input values) and columns = (the number of possible combinations of output values)
	getTableFromGenome( { 1 << numInputs, 1 << numOutputs }, { 16, 16 }, genomeIndex, genome);

	//normalize each row
	for (i = 0; i < (1 << numInputs); i++) { //for each row (each possible input bit string)
		// first sum the row
		double S = 0;
		for (j = 0; j < (1 << numOutputs); j++) {
			S += table[i][j];
		}
		// now normalize the row
		if (S == 0.0) { //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
			for (j = 0; j < (1 << numOutputs); j++)
				table[i][j] = 1.0 / (double) (1 << numOutputs);
		} else { //otherwise divide all values in a row by the sum of the row
			for (j = 0; j < (1 << numOutputs); j++)
				table[i][j] /= S;
		}
	}

}

void Gate::movePastStartCodeon(int& genomeIndex, Genome* genome) {
	codingRegions[genomeIndex] = START_CODE;
	genome->advanceIndex(genomeIndex);
	codingRegions[genomeIndex] = START_CODE;
	genome->advanceIndex(genomeIndex);
}

/*
 * gets an address with Agent::bitsPerBrainAddress bits
 * reads the required number of sites and then trims the leading bits.
 * if bitsPerBrainAddress = 10 and the next two sites were 14 (00001110) and 25(00011001)
 * the result would be 0000111000011001 masked with 0000001111111111 = 0000001000011001 or 537
 */
int Gate::getIOAddress(int& genomeIndex, Genome* genome) {
	int bitCount;
	int address = (int) genome->sites[genomeIndex]; // grab a byte from the genome
	genome->advanceIndex(genomeIndex); // advance the genome_index

	for (bitCount = 8; Global::bitsPerBrainAddress > bitCount; bitCount += 8) { // now, one site (8 bits) at a time, add sites to the address, until we have enough bits
		address = (address << 8) | (int) genome->sites[genomeIndex]; // shift the current value 8 bits left and add the next site
		genome->advanceIndex(genomeIndex); // advance the genome_index
	}
	int bitMask = 0;
	for (bitCount = 0; bitCount < Global::bitsPerBrainAddress; bitCount++) { // create the bit mask - this is used to trim off unwanted leading bits from the address
		bitMask = (bitMask << 1) | 1;
	}
	return address & bitMask; // trim the address with the bitMask
}

/*
 * Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
 */
void Gate::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, int& genome_index,
		Genome* genome, int codeNumber) {
	int lastLocation = genome_index; //make a note of where were are now so we can get codingRegions later
	for (auto i = 0; i < howMany; i++) { // for the number of addresses we need
		addresses[i] = getIOAddress(genome_index, genome); // get an address
	}
	while (lastLocation < genome_index) { // fill in codingRegions between lastLocation and currLocation
		codingRegions[lastLocation] = codeNumber;  // with 10 (inputs) or 11 (outputs) (connections)
		lastLocation++;
	}
	genome_index += ((ceil(((double) Global::bitsPerBrainAddress) / 8.0)) * (howManyMax - howMany));
	// move the genome_index ahead to account not only the addresses we need, but the total number of possible addresses
}

/*
 * given a genome and a genomeIndex:
 * pull out the number a number of inputs, number of outputs and then that many inputs and outputs
 * if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
 * there can not be more then 255 inputs or outputs without additional support (to read more sites to generate these numbers)
 */
void Gate::getInputsAndOutputs(const vector<int> insRange, vector<int> outsRange, int& genomeIndex, Genome* genome) { // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

	codingRegions[genomeIndex] = IN_COUNT_CODE;
	int numInputs = insRange[0] + (genome->sites[genomeIndex] & (insRange[1] - 1));
	genome->advanceIndex(genomeIndex);
	codingRegions[genomeIndex] = OUT_COUNT_CODE;
	int numOutputs = outsRange[0] + (genome->sites[genomeIndex++] & (outsRange[1] - 1));
	genome->advanceIndex(genomeIndex);

	inputs.resize(numInputs);
	outputs.resize(numOutputs);
	if (insRange[1] > 0) {
		getSomeBrainAddresses(numInputs, insRange[1], inputs, genomeIndex, genome, 11);
	}
	if (outsRange[1] > 0) {
		getSomeBrainAddresses(numOutputs, outsRange[1], outputs, genomeIndex, genome, 21);
	}
}

/*
 * build a table of size range[0],range[1] which is the upper left subset of a table rangeMax[0],rangeMax[1]
 * the table rangeMax[0],rangeMax[0] would be filled with values from the genome (filling each row before going to the next column
 * This table is assigned to the gates table field.
 * set codingRegions for each used genome site value = DATA_CODE; (may need to add more support for this later!)
 */
void Gate::getTableFromGenome(vector<int> range, vector<int> rangeMax, int& genomeIndex, Genome* genome) {
	int x, y;
	int Y = range[0];
	int X = range[1];
	int maxY = rangeMax[0];
	int maxX = rangeMax[1];

	//void getTableFromGenome({numInputs,NumOutputs},{4,4},genomeIndex);

	table.resize(Y); // set the number of rows in the table

	for (y = 0; y < (Y); y++) {
		table[y].resize(X); // set the number of columns in this row
		for (x = 0; x < X; x++) {
			table[y][x] = (double) (genome->sites[genomeIndex]);
			codingRegions[genomeIndex] = DATA_CODE;
			genome->advanceIndex(genomeIndex);
		}
		for (; x < maxX; x++) {
			genome->advanceIndex(genomeIndex); // advance genomeIndex to account for unused entries in the max sized table for this row
		}
	}
	for (y = 0; y < (Y); y++) {
		for (x = 0; x < maxY; x++) {
			genome->advanceIndex(genomeIndex); // advancegenomeIndexk to account for extra rows in the max sized table
		}
	}
}

void Gate::update(vector<double> & states, vector<double> & nextStates) { //this translates the input bits of the current states to the output bits of the next states
	int input = 0, output = 0;
	double r = Random::getDouble(1); //r is a random double between 0 and 1
	for (int i = (int) inputs.size() - 1; i >= 0; i--) { //for everything in vector inputs...
		input = (input << 1) + Bit(states[inputs[i]]); //stores the current states in bitstring input with bitshifting. it will now be a bitstring of length (inputs)
	}
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
		S = S + " " + mkString(inputs[i]);
	S = S + "\n";
	S = S + "OUT:";
	for (size_t i = 0; i < outputs.size(); i++)
		S = S + " " + mkString(outputs[i]);
	S = S + "\n";
	return S;
}

/*
 * converts values attained from genome for inputs and outputs to vaild brain state ids
 * uses nodeMap to accomplish the remaping
 */
void Gate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
	for (size_t i = 0; i < inputs.size(); i++) {
		inputs[i] = nodeMap[inputs[i]] % maxNodes;
	}
	for (size_t i = 0; i < outputs.size(); i++) {
		outputs[i] = nodeMap[outputs[i]] % maxNodes;
	}
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

// display the getCodingRegions for a gate as (Index1,Type1,Index2,Type2...)
string Gate::getCodingRegions() {
	string S = "";
	for (auto site : codingRegions) {
		S = S + mkString(site.first) + ":" + mkString(site.second) + "  ";
	}
	S += "\n";
	return S;
}

/* *** Determistic Gate Implementation *** */

DeterministicGate::DeterministicGate(Genome *genome, int genomeIndex) {
	inputs.clear();
	outputs.clear();
	int numOutputs, numInputs;
	codingRegions.clear();

	//move past the first to sites (start codeon)
	movePastStartCodeon(genomeIndex, genome);

	// get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
	getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome); // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
	numInputs = inputs.size();
	numOutputs = outputs.size();

	genome->advanceIndex(genomeIndex, 16); //move forward 16 spaces in genome

	// get a table filled with values from the genome that has
	// rows = (the number of possible combinations of input values) and columns = (the number of output values)
	getTableFromGenome( { (1 << numInputs), numOutputs }, { 4, 4 }, genomeIndex, genome);

	// convert the table contents to bits
	for (int i = 0; i < (1 << numInputs); i++) {
		for (int o = 0; o < numOutputs; o++) {
			table[i][0] = (int) table[i][0] & 1; // convert even to 0 and odd to 1
			//if (table[i][0] == 0) cout << table[i][0] << " "; // uncomment for light show! //
			//else cout << "  ";								// uncomment for light show! //

		}
	}
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

void DeterministicGate::update(vector<double> & states, vector<double> & nextStates) {
	int input = 0;
	for (int i = (int) inputs.size() - 1; i >= 0; i--) {
		input = (input << 1) + Bit(states[inputs[i]]); //stores current input states in bit string "input" with last input having greatest signifigence
	}
	vector<double> outputRow = table[input];
#if VOIDOUTPUT==1
	//if(*Data::parameterDouble["voidOutput"]>0){
	if (Random::P(Gate::voidOutPut)) {
		outputRow[Random::getIndex(outputs.size())] = 0; // pick one output randomly and set it to 0
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

GPGate::GPGate(Genome *genome, int genomeIndex) {
	int i;
	inputs.clear();
	outputs.clear();
	int numOutputs;
	codingRegions.clear();
	//move past the first to sites (start codeon)
	movePastStartCodeon(genomeIndex, genome);

	// get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
	getInputsAndOutputs( { 0, 0 }, { 1, 4 }, genomeIndex, genome); // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
	//numInputs = inputs.size();
	numOutputs = outputs.size();

	genome->advanceIndex(genomeIndex, 16);

	codingRegions[genomeIndex] = DATA_CODE;
	myGateType = genome->sites[(genomeIndex++) % genome->sites.size()] % 8;
	myValues.clear();
	for (i = 0; i < numOutputs; i++) {
		intToFloatBitByBit V;
		V.I = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		for (int j = 0; j < 4; j++) { // what is this 4??? number of inputs?
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			codingRegions[genomeIndex] = DATA_CODE;
			V.I = (V.I << (int) 8) + (int) genome->sites[(genomeIndex++) % genome->sites.size()];
		}
		myValues.push_back(V.F);
	}
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
	return "Genetic Programing " + gateTypeName[myGateType] + "\n" + Gate::description();
}

/* *** Feedback Gate *** */
bool FeedbackGate::feedbackON = true;

FeedbackGate::FeedbackGate(Genome *genome, int startCodonAt) {

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// need to move to new Get Address
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
	string S = "pos node:" + mkString((int) posFBNode) + "\n neg node:" + mkString((int) negFBNode);
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

Thresholdgate::Thresholdgate(Genome *genome, int startCodonAt) {
	int i, k;
	inputs.clear();
	outputs.clear();
	int _xDim;

	codingRegions.clear();

	codingRegions[startCodonAt] = START_CODE;
	codingRegions[(startCodonAt + 1) % genome->sites.size()] = START_CODE;
	//find the first nucleotide that isn't part of the start codon
	k = (startCodonAt + 2) % (int) genome->sites.size();

	//The dimensions are different here, the first inputs is the trigger
	//the next one are the current state Counter
	//the outputSize is the same, the first bit of the output is the ping
	//the others are the current state counter, they are the same as the inputs!
	//get the dimensions of the table
	codingRegions[k] = IN_COUNT_CODE; //maps values of third and fourth nucleotide
	_xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 7);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	codingRegions[k] = OUT_COUNT_CODE; ///////////////////////////////////////// IS THIS REALLY AN OUTPUT COUNT????
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//prepare the containers for the inputs and outputs addresses
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
	return "ThresholdgateGate: " + mkString(threshold) + "\n" + Gate::description();;
}

/* *** Fixed Epison Gate *** */
/* this gate behaves like a deterministic gate with a constant externally set error */

FixedEpsilonGate::FixedEpsilonGate(Genome *genome, int genomeIndex) :
					DeterministicGate(genome, genomeIndex){ // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
	epsilon = FixedEpsilonGateP; // in case you want to have different epsilon for different gates (who am I to judge?)

	// now to the specifics of this gate - we convert the table to a list of numbers (i.e. bitstrings) so we can do fast comparisons in the update
	defaultOutput.clear(); // clear the defaultOutput vector... the list of bitstrings
	for (size_t i = 0; i < table.size(); i++) { //for every row of the table (which is 2^inputs)
		int D = 0;
		for (size_t j = 0; j < table[i].size(); j++) { //for every space in the row
			D |= (Bit(table[i][j]) << j); //translate the value in that space to a bit (0 or 1), then bitshift it [position in row] times, then take D, bit-or it with that number,
			//and make that the new D
		}
		defaultOutput.push_back(D); //add D (which is a bitstring of length [2^outputs]) to defaultOutput. defaultoutputs is now of length (2^number of inputs)
	}
}

void FixedEpsilonGate::update(vector<double> & states, vector<double> & nextStates) {
	int input = 0; //input is the correct bitstring of outputs of the gate
	for (size_t i = 0; i < inputs.size(); i++) { // for every index in the input (I) vector
		input = (input << 1) + Bit(states[inputs[i]]); // stores current states in bitstring input
	}
	if (Random::P(epsilon)) { //if there will be an error
		//do a random output excluding the one otherwise given
		int output = 0;
		do {
			// WAS output = rand() & (table.size() - 1);
			output = Random::getIndex(table.size()); // pick a bitstring (with the correct number of bits) i.e. one bit for each output
		} while (output == defaultOutput[input]); // if you happen to pick the a bit string which happens to match the correct output, pick again
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
	return "Fixed Epsilon " + mkString(epsilon) + "\n " + Gate::description();
}

