//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Feedback_Gate__
#define __BasicMarkovBrainTemplate__Feedback_Gate__

#include "AbstractGate.h"

using namespace std;

class FeedbackGate : public AbstractGate {
 private:
	vector<vector<double>> table;
	vector<vector<double>> originalTable;
	unsigned char posFBNode, negFBNode;
	unsigned char nrPos, nrNeg;
	vector<double> posLevelOfFB, negLevelOfFB;
	deque<unsigned char> chosenInPos, chosenInNeg, chosenOutPos, chosenOutNeg;
 public:

	static bool feedbackON;

	FeedbackGate() = delete;
	FeedbackGate(shared_ptr<ParametersTable> _PT) :
		AbstractGate(_PT) {
		table = {};
		originalTable = {};
		exit(1); 
	}
	FeedbackGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID, shared_ptr<ParametersTable> _PT = nullptr);
	virtual ~FeedbackGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
	virtual void applyNodeMap(vector<int> nodeMap, int maxNodes);
	virtual void resetGate(void) override;
	virtual vector<int> getIns() override;
	virtual string gateType() override{
		return "Feedback";
	}
	virtual shared_ptr<AbstractGate> makeCopy(shared_ptr<ParametersTable> _PT = nullptr) override;

};

#endif /* defined(__BasicMarkovBrainTemplate__Feedback_Gate__) */
