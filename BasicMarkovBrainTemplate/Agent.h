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
#include <memory>
#include "Genome.h"
#include "Gate.h"
#include "Parameters.h"
#include <math.h>


using namespace std;

class Agent{
protected:
	vector<shared_ptr<Gate>> gates;

public:
	static int& defaultNrOfBrainStates; // default value for number of states in all brains
	int nrOfBrainStates; // the number of states in THIS brain
	static double& skipGate; // probablity to skip a gate when this brain is updating
	static bool& serialProcessing; // write directly states (overwrite) - do not use nextStates

	static void initializeParameters();
	
	static int makeDefaultNodeMap(vector <int> & nodeMap, int bitCount, int defaultNrOfBrainStates){
		//cout << "HERE:\n";
		for(int i=0;i<pow(2,bitCount);i++){
			//cout << i << "\n";
			//cout << defaultNrOfBrainStates << "\n";
			//cout << i%defaultNrOfBrainStates << "\n";
			nodeMap.push_back(i%defaultNrOfBrainStates);
		}
		//cout << "HERE2:\n";

		return 1;
	}
	static vector <int> defaultNodeMap;

	Genome *genome;
	vector <double> states;
	vector <double> nextStates;
	
	Agent();
	Agent(Genome *startGenome,int _nrOfStates);
	virtual ~Agent();
	
	void inOutReMap();

	inline int Bit(double d){ // returns 1 if "d" is greater than 0, else return 0
		return (d>0.0);
	}

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
