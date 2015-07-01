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

using namespace std;

class Gate{  //conventional probabilistic gate

public:
	static function<Gate*(ClassicMBGenome*,int)> makeGate[256];
	vector<int> I;
	vector<int> O;
	vector<vector<double>> table;
	
	Gate();
	Gate(ClassicMBGenome *genome,int startCodonAt);
	
	virtual ~Gate();
	
 	virtual void update(double *states,double *nextStates);
	int Bit(double d);
	virtual string description();
	virtual void applyNodeMap(int *nodeMap,int maxNodes);
	static void AddGate(int ID, function<Gate*(ClassicMBGenome*,int)> theFunction);
};


class DeterministicGate:public Gate{
public:
	DeterministicGate();
	DeterministicGate(ClassicMBGenome *genome,int startCodonAt);
	virtual ~DeterministicGate();
	virtual void update(double *states,double *nextStates);
	virtual string description();
};

/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */
class FeedbackGate:public Gate{
private:
	unsigned char posFBNode,negFBNode;
	unsigned char nrPos,nrNeg;
	vector<double> posLevelOfFB,negLevelOfFB;
	deque<unsigned char> chosenInPos,chosenInNeg,chosenOutPos,chosenOutNeg;
public:
	FeedbackGate();
	FeedbackGate(ClassicMBGenome *genome,int startCodonAt);
	~FeedbackGate();
	virtual void update(double *states,double *nextStates);
	virtual string description();
	virtual void applyNodeMap(int *nodeMap,int maxNodes);
};

class GPGate:public Gate{
private:
	int myGateType;											//<link> stores the kind of GP operation (Add, Sub, Mult...)
	vector<double> myValues;								//<link>
public:
	GPGate();
	GPGate(ClassicMBGenome *genome,int startCodonAt);
	virtual ~GPGate();
	virtual void update(double *states,double *nextStates);
	virtual string description();
};

class Thresholdgate:public Gate{
private:
	int threshold;
public:
	Thresholdgate();
	Thresholdgate(ClassicMBGenome *genome,int startCodonAt);
	virtual ~Thresholdgate();
	virtual void update(double *states,double *nextStates);
	virtual string description();
};


#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
