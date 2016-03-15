//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__MarkovBrain__
#define __BasicMarkovBrainTemplate__MarkovBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "Brain.h"
#include "../Gate/Gate_Builder.h"
#include "../GateListBuilder/GateListBuilder.h"
#include "../Genome/Genome.h"

#include "../Utilities/Parameters.h"

using namespace std;

class MarkovBrain : public AbstractBrain {
 protected:
	vector<shared_ptr<Gate>> gates;
	shared_ptr<Base_GateListBuilder> GLB;

 public:
	static int& defaultNrOfBrainStates;  // default value for number of states in all brains
	//int nrOfBrainStates;  // the number of states in THIS brain
	static bool& serialProcessing;  // write directly states (overwrite) - do not use nextStates

//	static bool& cacheResults;
//	static int& cacheResultsCount;

	static void initializeParameters();
	static vector<int> defaultNodeMap;

	/*
	 * Builds a look up table to convert genome site values into brain state addresses - this is only used when there is a fixed number of brain states
	 * if there is a variable number of brain states, then the node map must be rebuilt.
	 */
	static int makeNodeMap(vector<int> & nodeMap, int sizeInBits, int defaultNrOfBrainStates) {
		for (int i = 0; i < pow(2, (sizeInBits)); i++) {  // each site in the genome has 8 bits so we need to count though (  2 to the (8 * number of sites)  )
			nodeMap.push_back(i % defaultNrOfBrainStates);
		}

		return 1;
	}

	//Genome *genome;
	vector<double> states;
	vector<double> nextStates;

	MarkovBrain() = delete;

	MarkovBrain(shared_ptr<Base_GateListBuilder> _GLB, int _nrOfStates = defaultNrOfBrainStates);
	MarkovBrain(shared_ptr<Base_GateListBuilder> _GLB, shared_ptr<AbstractGenome> genome, int _nrOfBrainStates = defaultNrOfBrainStates);
	virtual ~MarkovBrain() = default;

	virtual void update() override;

	void inOutReMap();

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;

	virtual string description() override;
	virtual vector<string> getStats() override;

	virtual int IntFromState(vector<int> I);
	virtual int IntFromAllStates();
	virtual void resetBrain() override;
	virtual string gateList();
	virtual vector<vector<int>> getConnectivityMatrix();
	virtual int brainSize();
	//set<int> findCodingRegions(int mask);
	int numGates();

 public:
	inline void setState(const int& state, const double& value) override{
		if (state < (int) states.size()) {
			states[state] = value;
		} else {
			cout << "Writing to invalid brain state - this brain needs more states!\nExiting"<<endl;
			exit(1);
		}
	}
	inline double getState(const int& state) override{
		if (state < (int) states.size()) {
			return states[state];
		} else {
			cout << "Reading from invalid brain state - this brain needs more states!\nExiting"<<endl;
			exit(1);
		}
	}

	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);

};

#endif /* defined(__BasicMarkovBrainTemplate__MarkovBrain__) */
