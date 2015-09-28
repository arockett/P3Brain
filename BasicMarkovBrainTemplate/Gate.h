//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Gate__
#define __BasicMarkovBrainTemplate__Gate__

#include <stdlib.h>
#include <vector>
#include <deque>
#include "Genome.h"
#include <functional>
#include "Parameters.h"

using namespace std;



class Gate{  //conventional probabilistic gate
public:
	static bool gateFlags[8];
	static double voidOutPut;
	static void initializeParameters();
	
public:
	static void setupGates();
	static function<Gate*(Genome*,int)> makeGate[256];
	vector<int> I;
	vector<int> O;
	vector<vector<double>> table;
	map<int,int> codingRegions; //indicates coding regions. maps genome index to usage of nucleotide in gate. 0=start codon. 1=wiring(defines # of inputs and outputs and what they are). 2=gate specific information
	
	Gate();
	Gate(Genome *genome,int startCodonAt);
	
	virtual ~Gate();
	
	virtual void update(vector <double> & states, vector <double> & nextStates);
	int Bit(double d);
	virtual string description();
	virtual void applyNodeMap(vector <int> nodeMap,int maxNodes);
	static void AddGate(int ID, function<Gate*(Genome*,int)> theFunction);
	virtual void resetGate(void);
	virtual vector<int> getIns();
	virtual vector<int> getOuts();
};


class DeterministicGate:public Gate{
public:
	DeterministicGate();
	DeterministicGate(Genome *genome,int startCodonAt);
	virtual ~DeterministicGate();
	virtual void update(vector <double> & states, vector <double> & nextStates);
	virtual string description();
	
	void setupForBits(int* Ins,int nrOfIns, int Out, int logic);
	
	double voidOutput;
};

/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */
class FeedbackGate:public Gate{
private:
	vector<vector<double>> originalTable;
	unsigned char posFBNode,negFBNode;
	unsigned char nrPos,nrNeg;
	vector<double> posLevelOfFB,negLevelOfFB;
	deque<unsigned char> chosenInPos,chosenInNeg,chosenOutPos,chosenOutNeg;
public:
	static bool feedbackON;
	FeedbackGate();
	FeedbackGate(Genome *genome,int startCodonAt);
	~FeedbackGate();
	virtual void update(vector <double> & states, vector <double> & nextStates);
	virtual string description();
	virtual void applyNodeMap(vector <int> nodeMap,int maxNodes);
	virtual void resetGate(void);
	virtual vector<int> getIns();
};

class GPGate:public Gate{
private:
	int myGateType;											//<link> stores the kind of GP operation (Add, Sub, Mult...)
	vector<double> myValues;								//<link>
public:
	GPGate();
	GPGate(Genome *genome,int startCodonAt);
	virtual ~GPGate();
	virtual void update(vector <double> & states, vector <double> & nextStates);
	virtual string description();
};

class Thresholdgate:public Gate{
private:
	int threshold;
public:
	Thresholdgate();
	Thresholdgate(Genome *genome,int startCodonAt);
	virtual ~Thresholdgate();
	virtual void update(vector <double> & states, vector <double> & nextStates);
	virtual string description();
};

class FixedEpsilonGate:public DeterministicGate{
private:
public:
	vector<int> defaultOutput;
	static double epsilon;
	FixedEpsilonGate();
	FixedEpsilonGate(Genome *genome,int startCodonAt);
	virtual ~FixedEpsilonGate();
	virtual void update(vector <double> & states, vector <double> & nextStates);
	virtual string description();
};



#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
