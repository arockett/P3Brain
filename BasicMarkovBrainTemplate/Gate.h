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
#include "Parameters.h"

using namespace std;

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

	/*
	 * gets an address with Agent::bitsPerBrainAddress bits
	 * reads the required number of sites and then trims the leading bits.
	 * if bitsPerBrainAddress = 10 and the next two sites were 14 (00001110) and 25(00011001)
	 * the result would be 0000111000011001 masked with 0000001111111111 = 0000001000011001 or 537
	 */
	int getIOAddress(int& genome_index, const Genome *genome) {
		int bitCount;
		int address = (int) genome->sites[genome_index]; // grab a byte from the genome
		genome_index = (genome_index + 1) % genome->sites.size(); // advance the genome_index

		for (bitCount = 8; Data::bitsPerBrainAddress > bitCount; bitCount += 8) { // now, one site (8 bits) at a time, add sites to the address, until we have enough bits
			address = (address << 8) | (int) genome->sites[genome_index]; // shift the current value 8 bits left and add the next site
			genome_index = (genome_index + 1) % genome->sites.size(); // advance the genome_index
		}
		int bitMask = 0;
		for (bitCount = 0; bitCount < Data::bitsPerBrainAddress; bitCount++) { // create the bit mask - this is used to trim off unwanted leading bits from the address
			bitMask = (bitMask << 1) | 1;
		}
		return address & bitMask; // trim the address with the bitMask
	}

	/*
	 * Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
	 */
	void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, int& genome_index,
			const Genome* genome, map<int, int>& codingRegions) {
		int lastLocation = genome_index; //make a note of where were are now so we can get codingRegions later
		for (auto i = 0; i < howMany; i++) { // for the number of addresses we need
			addresses[i] = getIOAddress(genome_index, genome); // get an address
		}
		while (lastLocation < genome_index) { // fill in codingRegions between lastLocation and currLocation
			codingRegions[lastLocation] = 1;  // with 1 (connections)
			lastLocation++;
		}
		genome_index += ((ceil(((double) Data::bitsPerBrainAddress) / 8.0)) * (howManyMax-howMany));
		// move the genome_index ahead to account not only the addresses we need, but the total number of possible addresses
	}

	/*
	 * a shortcut for getSomeBrainAddresses() when you need to inputs, outputs and want to set them to this Agents inputs and outputs
	 */
	void getInputsAndOutputs(const int& insCount, const int& outsCount, const int& insMaxCount, const int& outsMaxCount,
			int& genome_index, const Genome* genome, map<int, int>& codingRegions) { // (#in, #out, max#in, max#out,currentIndexInGenome,genome,codingRegions)
		getSomeBrainAddresses(insCount, insMaxCount, inputs, genome_index, genome, codingRegions);
		getSomeBrainAddresses(outsCount, outsMaxCount, outputs, genome_index, genome, codingRegions);
	}

	virtual ~Gate();

	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
	virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);
	static void AddGate(int ID, function<shared_ptr<Gate>(Genome*, int)> theFunction);
	virtual void resetGate(void);
	virtual vector<int> getIns();
	virtual vector<int> getOuts();
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
