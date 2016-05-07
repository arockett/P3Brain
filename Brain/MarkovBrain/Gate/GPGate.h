//
//  Gate.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__GP_Gate__
#define __BasicMarkovBrainTemplate__GP_Gate__

#include "AbstractGate.h"

using namespace std;

class GPGate : public AbstractGate {
 private:

	int operation; // <link> stores the kind of GP operation (Add, Sub, Mult...)
	vector<double> constValues; // list of constant values
 public:

	static shared_ptr<double> constValueMin;
	static shared_ptr<double> constValueMax;

	GPGate() = delete;
	//GPGate(shared_ptr<AbstractGenome> genome, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID);
	GPGate(pair<vector<int>, vector<int>> _addresses, int _operation, vector<double> _constValues, int gateID);

	virtual ~GPGate() = default;
	virtual void update(vector<double> & states, vector<double> & nextStates) override;
	virtual string description() override;
	virtual string gateType() override{
		return "GeneticPrograming";
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__GP_Gate__) */
