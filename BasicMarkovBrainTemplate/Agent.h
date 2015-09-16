//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Agent__
#define __BasicMarkovBrainTemplate__Agent__

#include <vector>
#include <set>
#include <iostream>
#include "Genome.h"
#include "Gate.h"

using namespace std;


class Agent{
protected:
	vector<Gate*> gates;
	
public:
	//static int nodeMap[256];
	double skipGate;
	Genome *genome;
	double *states,*nextStates;
	int *nodeMap;
	int nrOfStates;
	static bool serialProcessing;
	
	Agent();
	Agent(Genome *startGenome,int _nrOfStates);
	~Agent();
	
	//	static void initializeGates(vector<bool> isGateOn);
	int Bit(double d);
	virtual int IntFromState(vector<int> I);
	virtual int IntFromAllStates();
	virtual void resetBrain();
	virtual void updateStates();
	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
	virtual int brainSize();
	set<int> findCodingRegions(int mask);
	int numGates();

};

#endif /* defined(__BasicMarkovBrainTemplate__Agent__) */
