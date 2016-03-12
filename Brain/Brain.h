//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Brain__
#define __BasicMarkovBrainTemplate__Brain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../Gate/Gate_Builder.h"
#include "../GateListBuilder/GateListBuilder.h"
#include "../Genome/Genome.h"

#include "../Utilities/Parameters.h"

using namespace std;
class AbstractBrain {
 public:
	int nrOfBrainStates;  // the number of states in THIS brain
	                      // this is temporary! new node/memory/dataCell will fix this.

	AbstractBrain() {
		nrOfBrainStates = 0;
	}
	virtual ~AbstractBrain() = default;
	virtual void update() = 0;
	virtual string description() = 0;  // returns a desription of this brain in it's current state
	virtual vector<string> getStats() =0;  // returns a vector of string pairs of any stats that can then be used for data tracking (etc.)

//	virtual string description();
//	virtual vector<string> getStats();
//
//	virtual int IntFromState(vector<int> I);
//	virtual int IntFromAllStates();
//	virtual string gateList();
//	virtual vector<vector<int>> getConnectivityMatrix();
//	virtual int brainSize();
//	//set<int> findCodingRegions(int mask);
//	int numGates();
	virtual void resetBrain() = 0;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) = 0;
	virtual void setState(const int& state, const double& value) = 0;
	virtual double getState(const int& state) = 0;
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome) = 0;

};

#endif /* defined(__BasicMarkovBrainTemplate__Brain__) */
