//
//  GateListBuilder.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__GateListBuilder__
#define __BasicMarkovBrainTemplate__GateListBuilder__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../Gate/GateBuilder.h"
#include "../Genome/AbstractGenome.h"

#include "../Utilities/Parameters.h"

using namespace std;
class AbstractGateListBuilder {
 public:
	AbstractGateListBuilder() = default;
	virtual ~AbstractGateListBuilder() = default;
//	virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
//	                                               int maxValue, shared_ptr<vector<int>> genomeHeadValues, int genomeHeadValuesCount,
//	                                               shared_ptr<vector<vector<int>>> genomePerGateValues, int genomePerGateValuesCount) = 0;

	virtual vector<shared_ptr<AbstractGate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates){
		vector<int> temp1;
		vector<vector<int>> temp2;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, 0, temp1, 0, temp2, 0);
	}

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetHeadValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount){
		vector<vector<int>> temp;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, maxValue, genomeHeadValues, genomeHeadValuesCount, temp, 0);
	}

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetPerGateValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount){
		vector<int> temp;
		return buildGateListAndGetAllValues(genome, nrOfBrainStates, maxValue, temp, 0, genomePerGateValues, genomePerGateValuesCount);
	}

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetAllValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
            int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount,
            vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount) = 0;

};

class ClassicGateListBuilder : public AbstractGateListBuilder {
 public:

	ClassicGateListBuilder() = default;
	virtual ~ClassicGateListBuilder() = default;

	virtual vector<shared_ptr<AbstractGate>> buildGateListAndGetAllValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates,
	                                               int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount,
	                                               vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount);
};

#endif /* defined(__BasicMarkovBrainTemplate__GateListBuilder__) */