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
	static bool& usingVoidGate;

	static double& voidOutPut;
	static double& FixedEpsilonGateP;

public:
	static void setupGates();
	static function<shared_ptr<Gate>(Genome*, int)> makeGate[256];
	vector<int> inputs;
	vector<int> outputs;
	vector<vector<double>> table;
	map<int, int> codingRegions; //indicates coding regions. maps genome index to usage of nucleotide in gate. 0=start codon. 1=wiring(defines # of inputs and outputs and what they are). 2=gate specific information

	Gate() = default;
	Gate(Genome *genome, int startCodonAt);

	void movePastStartCodeon(int& genomeIndex, Genome* genome);
	int getIOAddress(int& genomeIndex, Genome* genome);
	void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, int& genome_index,
			Genome* genome, int codeNumber);
	void getInputsAndOutputs(const vector<int> insRange, vector<int> outsRange, int& genomeIndex, Genome* genome);
	void getTableFromGenome(vector<int> range, vector<int> rangeMax, int& genomeIndex, Genome* genome);
	int getIndexFromInputs(vector<double> & states);
	virtual ~Gate() = default;

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
	DeterministicGate() = default;
	DeterministicGate(Genome *genome, int startCodonAt);
	virtual ~DeterministicGate() = default;
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
	FeedbackGate() = default;
	FeedbackGate(Genome *genome, int startCodonAt);
	virtual ~FeedbackGate() = default;
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
	GPGate() = default;
	GPGate(Genome *genome, int startCodonAt);
	virtual ~GPGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

class Thresholdgate: public Gate {
private:
	int threshold;
public:
	Thresholdgate() = default;
	Thresholdgate(Genome *genome, int startCodonAt);
	virtual ~Thresholdgate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

class FixedEpsilonGate: public DeterministicGate {
private:
public:
	vector<int> defaultOutput;
	double epsilon;
	FixedEpsilonGate() = default;
	FixedEpsilonGate(Genome *genome, int startCodonAt);
	virtual ~FixedEpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

class VoidGate: public DeterministicGate {
private:
public:
	vector<int> defaultOutput;
	double epsilon;
	VoidGate() = default;
	VoidGate(Genome *genome, int startCodonAt);
	virtual ~VoidGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates);
	virtual string description();
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
