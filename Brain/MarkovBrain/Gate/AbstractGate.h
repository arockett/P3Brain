//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Gate__
#define __BasicMarkovBrainTemplate__Gate__

#include <deque>
#include <functional>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>
#include <iostream>

#include "../../../Genome/AbstractGenome.h"
#include "../../../Global.h"

#include "../../../Utilities/Parameters.h"
#include "../../../Utilities/Random.h"

using namespace std;

class AbstractGate {
public:
	int ID;
	const shared_ptr<ParametersTable> PT;

	static const int START_CODE = 0;
	static const int IN_COUNT_CODE = 10;
	static const int IN_ADDRESS_CODE = 11;
	static const int OUT_COUNT_CODE = 20;
	static const int OUT_ADDRESS_CODE = 21;
	static const int DATA_CODE = 30;
	AbstractGate() = delete;
	AbstractGate(shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {
		ID = 0;
	}
	virtual ~AbstractGate() = default;

	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr);
	vector<int> inputs;
	vector<int> outputs;

	virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);  // converts genome values into brain state value addresses
	virtual void resetGate(void);  // this is empty here. Some gates so not need to reset, they can use this method.
	virtual vector<int> getIns();  // returns a vector of int with the adress for this gates input brain state value addresses
	virtual vector<int> getOuts();  // returns a vector of int with the adress for this gates onput brain state value addresses
	virtual void update(vector<double> & states, vector<double> & nextStates) = 0;  // the function is empty, and must be provided in any derived gates
	// returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions
	virtual string descriptionIO();  // returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions
	virtual string description();  // returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions
	virtual string gateType() {
		return "undefined";
	}
	virtual pair<vector<int>,vector<int>> getConnectionsLists(){
		pair<vector<int>,vector<int>> connectionsLists;
		connectionsLists.first = inputs;
		connectionsLists.second = outputs;
		return connectionsLists;
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
