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
#include "Genome.h"
#include "Gate.h"

using namespace std;


class Agent{
private:
	vector<Gate*> gates;
	
public:
	ClassicMBGenome *genome;
	double *states,*nextStates;
	int *nodeMap;
	int nrOfStates;
	static bool serialProcessing;
	Agent();
	Agent(ClassicMBGenome *startGenome,int _nrOfStates);
	~Agent();
	
	static void initializeGates(vector<bool> isGateOn);
	int Bit(double d);
	virtual int IntFromState(vector<int> I);
	virtual int IntFromAllStates();
	virtual void resetBrain();
	virtual void updateStates();
	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
};

#endif /* defined(__BasicMarkovBrainTemplate__Agent__) */
