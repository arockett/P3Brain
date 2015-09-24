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
#include "Parameters.h"

using namespace std;

class AgentSettings{
public:
	static int nrOfBrainStates;
	static double skipGate;
	static bool serialProcessing;
	static void initializeParameters();
};


class Agent{
protected:
	vector<Gate*> gates;
	
public:
	Genome *genome;
	double *states,*nextStates;
	int *nodeMap;
	int nrOfStates;
	
	Agent();
	Agent(Genome *startGenome,int _nrOfStates);
	virtual ~Agent();
	
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
