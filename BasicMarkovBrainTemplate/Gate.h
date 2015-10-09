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
	 */
	int getIOAddress(int& geomeLocation, const Genome *genome) {
		int bitCount;
		int address = (int) genome->sites[geomeLocation];
		geomeLocation = (geomeLocation + 1) % genome->sites.size();

		for (bitCount = 8; Data::bitsPerBrainAddress > bitCount; bitCount += 8) {
			address = (address << 8) | (int) genome->sites[geomeLocation];
			geomeLocation = (geomeLocation + 1) % genome->sites.size();
		}
		int bitMask = 0;
		for (bitCount = 0; bitCount < Data::bitsPerBrainAddress; bitCount++) {
			bitMask = (bitMask << 1) | 1;
		}
		return address & bitMask;
	}

	void getInputsAndOutputs(const int& insCount, const int& outsCount, const int& insMaxCount,
			const int& outsMaxCount, int& genome_index, const Genome* genome, map<int, int>& codingRegions) { // (#in #out max#in max#out,currentIndexInGenome,genome,codingRegions)
		int currLocation = genome_index;
		for (auto i = 0; i < insCount; i++) {
			int lastLocation = currLocation;
			inputs[i] = getIOAddress(currLocation, genome);
			while (lastLocation < currLocation) {
				codingRegions[lastLocation++] = 1;
			}
		}
		genome_index += ((ceil((double) Data::bitsPerBrainAddress / 8.0)) * 4);

		currLocation = genome_index;
		for (auto i = 0; i < outsCount; i++) {

			int lastLocation = currLocation;
			outputs[i] = getIOAddress(currLocation, genome);
			while (lastLocation < currLocation) {
				codingRegions[lastLocation++] = 1;
			}
		}
		genome_index += ((ceil((double) Data::bitsPerBrainAddress / 8.0)) * 4);
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

	double voidOutput;
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
	static double epsilon;
	FixedEpsilonGate();
	FixedEpsilonGate(Genome *genome, int startCodonAt);
	virtual ~FixedEpsilonGate();
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
