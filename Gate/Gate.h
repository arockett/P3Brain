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

	//static void setupGates();

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

	// functions which must be provided with each gate
	virtual void update(vector<double> & states, vector<double> & nextStates) = 0;  // the function is empty, and must be provided in any derived gates
	virtual string description() = 0;  // returns a description of the gate. This method can be used in derived gates description method to return ins and outs and coding regions

};

class ProbabilisticGate : public Gate {  //conventional probabilistic gate
 public:

	vector<vector<double>> table;

	ProbabilisticGate() = delete;
	//ProbabilisticGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID);
	ProbabilisticGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _rawTable, int _ID);


	virtual ~ProbabilisticGate() = default;

	virtual void update(vector<double> & states, vector<double> & nextStates) override;

	virtual string description() override;

};

class DeterministicGate : public Gate {
 public:
	vector<vector<int>> table;

	DeterministicGate() = delete;
	//DeterministicGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID);
	DeterministicGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _table, int _ID);

	virtual ~DeterministicGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;

	void setupForBits(int* Ins, int nrOfIns, int Out, int logic);

	virtual string description() override;

	//double voidOutput;
};

class FixedEpsilonGate : public DeterministicGate {
 private:
 public:
	static const double& FixedEpsilonGate_Probability;

	vector<int> defaultOutput;
	double epsilon;
	FixedEpsilonGate() = delete;
	FixedEpsilonGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~FixedEpsilonGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
};

class VoidGate : public DeterministicGate {
	static const double& voidGate_Probability;

 private:
 public:
	vector<int> defaultOutput;
	double epsilon;
	VoidGate() = delete;
	VoidGate(pair<vector<int>,vector<int>> addresses, vector<vector<int>> _table, int _ID);
	virtual ~VoidGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
