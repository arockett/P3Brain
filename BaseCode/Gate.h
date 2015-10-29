//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Gate__
#define __BasicMarkovBrainTemplate__Gate__

#include <deque>
#include <functional>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>

#include "Genome.h"
#include "Global.h"

#include "../Utilities/Parameters.h"

using namespace std;

#define START_CODE 0
#define IN_COUNT_CODE 10
#define IN_ADDRESS_CODE 11
#define OUT_COUNT_CODE 20
#define OUT_ADDRESS_CODE 21
#define DATA_CODE 30

class Gate {  //conventional probabilistic gate
public:
	static bool& usingProbGate;
	static bool& usingDetGate;
	static bool& usingFBGate;
	static bool& usingGPGate;
	static bool& usingThGate;
	static bool& usingEpsiGate;
	static double& voidOutPut;
	static double& FixedEpsilonGateP;

public:
	static void setupGates();
	static function<shared_ptr<Gate>(Genome*, int)> makeGate[256];
	vector<int> inputs;
	vector<int> outputs;
	vector<vector<double>> table;
	map<int, int> codingRegions; //indicates coding regions. maps genome index to usage of nucleotide in gate. 0=start codon. 1=wiring(defines # of inputs and outputs and what they are). 2=gate specific information

	Gate();
	Gate(Genome *genome, int startCodonAt);

	void movePastStartCodeon(int& genomeIndex, Genome* genome) {
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
	int getIOAddress(int& genomeIndex, Genome* genome) {
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
	void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, int& genome_index,
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
	void getInputsAndOutputs(const vector<int> insRange, vector<int> outsRange, int& genomeIndex, Genome* genome) { // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

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
	void getTableFromGenome(vector<int> range, vector<int> rangeMax, int& genomeIndex, Genome* genome) {
		int x,y;
		int Y = range[0];
		int X = range[1];
		int maxY = rangeMax[0];
		int maxX = rangeMax[1];

		//void getTableFromGenome({numInputs,NumOutputs},{4,4},genomeIndex);

		table.resize(Y); // set the number of rows in the table

		for ( y = 0; y < (Y); y++) {
			table[y].resize(X); // set the number of columns in this row
			for ( x = 0; x < X; x++) {
				table[y][x] = (double) (genome->sites[genomeIndex]);
				codingRegions[genomeIndex] = DATA_CODE;
				genome->advanceIndex(genomeIndex);
			}
			for (; x < maxX; x++) {
				genome->advanceIndex(genomeIndex); // advance genomeIndex to account for unused entries in the max sized table for this row
			}
		}
		for ( y = 0; y < (Y); y++) {
			for ( x = 0; x < maxY; x++) {
				genome->advanceIndex(genomeIndex); // advancegenomeIndexk to account for extra rows in the max sized table
			}
		}
	}



	virtual ~Gate();

	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
	virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);
	static void AddGate(int ID, function<shared_ptr<Gate>(Genome*, int)> theFunction);
	virtual void resetGate(void);
	virtual vector<int> getIns();
	virtual vector<int> getOuts();

	virtual string getCodingRegions(); // display the codingRegions

};

class DeterministicGate: public Gate {
public:
	DeterministicGate();
	DeterministicGate(Genome *genome, int startCodonAt);
	virtual ~DeterministicGate();
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();

	void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	//double voidOutput;
};

/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */
class FeedbackGate: public Gate {
private:
	vector<vector<double>> originalTable;
	unsigned char posFBNode, negFBNode;
	unsigned char nrPos, nrNeg;
	vector<double> posLevelOfFB, negLevelOfFB;
	deque<unsigned char> chosenInPos, chosenInNeg, chosenOutPos, chosenOutNeg;
public:
	static bool feedbackON;
	FeedbackGate();
	FeedbackGate(Genome *genome, int startCodonAt);
	~FeedbackGate();
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
	virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);
	virtual void resetGate(void);
	virtual vector<int> getIns();
};

class GPGate: public Gate {
private:
	int myGateType;											//<link> stores the kind of GP operation (Add, Sub, Mult...)
	vector<double> myValues;								//<link>
public:
	GPGate();
	GPGate(Genome *genome, int startCodonAt);
	virtual ~GPGate();
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

class Thresholdgate: public Gate {
private:
	int threshold;
public:
	Thresholdgate();
	Thresholdgate(Genome *genome, int startCodonAt);
	virtual ~Thresholdgate();
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

class FixedEpsilonGate: public DeterministicGate {
private:
public:
	vector<int> defaultOutput;
	double epsilon;
	FixedEpsilonGate();
	FixedEpsilonGate(Genome *genome, int startCodonAt);
	virtual ~FixedEpsilonGate();
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
