//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Agent__
#define __BasicMarkovBrainTemplate__Agent__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "Gate.h"
#include "Genome.h"

#include "../Utilities/Parameters.h"

using namespace std;

class Agent {
protected:
	vector<shared_ptr<Gate>> gates;

public:
	static int& defaultNrOfBrainStates; // default value for number of states in all brains
	int nrOfBrainStates; // the number of states in THIS brain
	static double& skipGate; // probablity to skip a gate when this brain is updating
	static bool& serialProcessing; // write directly states (overwrite) - do not use nextStates

	static void initializeParameters();

	/*
	 * Builds a look up table to convert genome site values into brain state addresses - this is only used when there is a fixed number of brain states
	 * if there is a variable number of brain states, then the node map must be rebuilt.
	 */
	static int makeNodeMap(vector<int> & nodeMap, int sizeInBits, int defaultNrOfBrainStates) {
		for (int i = 0; i < pow(2, (sizeInBits)); i++) { // each site in the genome has 8 bits so we need to count though (  2 to the (8 * number of sites)  )
			nodeMap.push_back(i % defaultNrOfBrainStates);
		}

		return 1;
	}
	static vector<int> defaultNodeMap;

	Genome *genome;
	vector<double> states;
	vector<double> nextStates;

	Agent() = default;
	Agent(Genome *startGenome, int _nrOfStates);
	virtual ~Agent() = default;

	void inOutReMap();

	virtual int IntFromState(vector<int> I);
	virtual int IntFromAllStates();
	virtual void resetBrain();
	virtual void updateStates();
	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
	virtual int brainSize();
	set<int> findCodingRegions(int mask);
	int numGates();

public:
	inline void setState(const int& state, const double& value) {
		if (state < (int) states.size()) {
			states[state] = value;
		} else {
			cout << "Writing to invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}
	inline double getState(const int& state) {
		if (state < (int) states.size()) {
			return states[state];
		} else {
			cout << "Reading from invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__Agent__) */
