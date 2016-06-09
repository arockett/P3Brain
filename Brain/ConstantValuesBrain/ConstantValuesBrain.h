//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__ConstantValuesBrain__
#define __BasicMarkovBrainTemplate__ConstantValuesBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"


using namespace std;

class ConstantValuesBrain: public AbstractBrain {
public:

	static shared_ptr<ParameterLink<double>> valueMinPL;
	static shared_ptr<ParameterLink<double>> valueMaxPL;
	static shared_ptr<ParameterLink<int>> valueTypePL;
	static shared_ptr<ParameterLink<int>> samplesPerValuePL;

	double valueMin;
	double valueMax;
	double valueType;
	int samplesPerValue;

	ConstantValuesBrain() = delete;

	ConstantValuesBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes, shared_ptr<ParametersTable> _PT = nullptr);

	virtual ~ConstantValuesBrain() = default;

	static shared_ptr<AbstractBrain> brainFactory(int ins, int outs, int hidden, shared_ptr<ParametersTable> PT) {
		return make_shared<ConstantValuesBrain>(ins, outs, hidden, PT);
	}

	virtual void update() override;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;

	virtual string description() override;
	virtual vector<string> getStats() override;

	virtual void resetBrain() override;
	virtual void resetOutputs() override;

	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome);

};

#endif /* defined(__BasicMarkovBrainTemplate__ConstantValuesBrain__) */
