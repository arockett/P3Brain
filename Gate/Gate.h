//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Gate__
#define __BasicMarkovBrainTemplate__Gate__

#include <deque>
#include <functional>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <vector>

#include "../Genome/AbstractGenome.h"
#include "../Global.h"

#include "../Utilities/Parameters.h"

using namespace std;

class Gate {
public:
	int ID;

	static const int START_CODE = 0;
	static const int IN_COUNT_CODE = 10;
	static const int IN_ADDRESS_CODE = 11;
	static const int OUT_COUNT_CODE = 20;
	static const int OUT_ADDRESS_CODE = 21;
	static const int DATA_CODE = 30;

	Gate() {
		ID = 0;
	}
	virtual ~Gate() = default;

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

class ProbabilisticGate: public Gate {  //conventional probabilistic gate
public:
	vector<vector<double>> table;
	ProbabilisticGate() = delete;
	ProbabilisticGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _rawTable, int _ID);
	virtual ~ProbabilisticGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "Probabilistic";
	}


};

class DeterministicGate: public Gate {
public:
	vector<vector<int>> table;
	DeterministicGate() = delete;
	DeterministicGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~DeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	//void setupForBits(int* Ins, int nrOfIns, int Out, int logic);
	virtual string gateType() override{
		return "Deterministic";
	}
};

class FixedEpsilonGate: public DeterministicGate {
public:
	static const double& FixedEpsilonGate_Probability;
	vector<int> defaultOutput;
	double epsilon;
	FixedEpsilonGate() = delete;
	FixedEpsilonGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~FixedEpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
		return "FixedEpsilon";
	}

};

class VoidGate: public DeterministicGate {
	static const double& voidGate_Probability;
public:
	vector<int> defaultOutput;
	double epsilon;
	VoidGate() = delete;
	VoidGate(pair<vector<int>, vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~VoidGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string gateType() override{
			return "VoidGate";
		}

};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
