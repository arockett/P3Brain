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
	static const string& brainTypeStr;
	static const int& hiddenNodes;
	static const bool& serialProcessing;

	vector<string> aveFileColumns;

	bool recordActivity;
	string recordActivityFileName;

	int nrOfBrainNodes;  // the number of states in THIS brain
						 // this is temporary! new node/memory/dataCell will fix this.
	int nrInNodes;
	int nrOutNodes;
	int nrHiddenNodes;

	vector<double> nodes;
	vector<double> nextNodes;

	AbstractBrain() {
		nrInNodes = nrOutNodes = nrHiddenNodes = nrOfBrainNodes = 0;
		recordActivity = false;
		cout << "ERROR: attempting to construct brain with no arguments. Check brain type for required parameters... most likely at least #in, #out and #hidden are required!\n\nExiting.\n" << endl;
		exit(1);
	}

	AbstractBrain(int ins, int outs, int hidden) {
		nrInNodes = ins;
		nrOutNodes = outs;
		nrHiddenNodes = hidden;
		recordActivity = false;

		nrOfBrainNodes = nrInNodes + nrOutNodes + nrHiddenNodes;
		nodes.resize(nrOfBrainNodes);
		nextNodes.resize(nrOfBrainNodes);

	}

	virtual ~AbstractBrain() = default;
	virtual void update() = 0;

	virtual string description() = 0;  // returns a desription of this brain in it's current state
	virtual vector<string> getStats() =0;  // returns a vector of string pairs of any stats that can then be used for data tracking (etc.)

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) = 0;
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome) = 0;

	virtual void inline resetBrain() {
		for (int i = 0; i < nrOfBrainNodes; i++) {
			nodes[i] = 0.0;
		}
	}

	virtual void inline setRecordActivity(bool _recordActivity){
		recordActivity = _recordActivity;
	}

	virtual void inline setRecordFileName(string _recordActivityFileName){
		recordActivityFileName = _recordActivityFileName;
	}

	virtual void inline resetOutputs() {
		for (int i = nrInNodes; i < (nrInNodes + nrOutNodes); i++) {
			nodes[i] = 0.0;
		}
	}

	inline void setInput(const int& nodeAddress, const double& value) {
		if (nodeAddress < nrInNodes) {
			nodes[nodeAddress] = value;
		} else {
			cout << "in Brain::setInput() : Writing to invalid input node (" << nodeAddress << ") - this brain needs more input nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline void setNode(const int& nodeAddress, const double& value) {
		if (nodeAddress < nrInNodes + nrOutNodes + nrHiddenNodes) {
			nodes[nodeAddress] = value;
		} else {
			cout << "in Brain::setNode() :Writing to invalid input node (" << nodeAddress << ") - this brain needs more nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline double readInput(const int& nodeAddress) {
		if (nodeAddress < nrInNodes) {
			return nodes[nodeAddress];
		} else {
			cout << "in Brain::readInput() : Reading from invalid input node (" << nodeAddress << ") - this brain needs more input nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline double readOutput(const int& nodeAddress) {
		if (nodeAddress < nrOutNodes) {
			return nodes[nodeAddress + nrInNodes];
		} else {
			cout << "in Brain::readOutput() : Reading from invalid output node (" << nodeAddress << ") - this brain needs more output nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline double ReadNode(const int& nodeAddress) {
		if (nodeAddress < nrInNodes + nrOutNodes + nrHiddenNodes) {
			return nodes[nodeAddress];
		} else {
			cout << "in Brain::ReadNode() :Writing to invalid input node (" << nodeAddress << ") - this brain needs more nodes!\nExiting" << endl;
			exit(1);
		}
	}

	inline vector<double> ReadAllNodes(const int& nodeAddress) {
		return nodes;
	}

	// converts the value of each value in nodes[] to bit and converts the bits to an int
	// useful to generate values for lookups, useful for caching
	int allNodesToBitToInt() {
		int result = 0;
		for (int i = 0; i < nrOfBrainNodes; i++)
			result = (result) + Bit(nodes[i]);
		return result;

	}

};

#endif /* defined(__BasicMarkovBrainTemplate__Brain__) */
