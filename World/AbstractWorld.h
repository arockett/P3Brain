//
//  World.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__World__
#define __BasicMarkovBrainTemplate__World__

#include <stdlib.h>
#include <thread>
#include <vector>

#include "../Group/Group.h"

using namespace std;

class AbstractWorld {
public:
	const static int& repeats;
	const static bool& showOnUpdate;

	vector<string> aveFileColumns;

	AbstractWorld() = default;
	virtual ~AbstractWorld() = default;
	virtual void evaluateFitness(vector<shared_ptr<Organism>> population, bool analyse, bool show = 0);
	virtual double testIndividual(shared_ptr<Organism> org, bool analyse, bool show = 0) = 0;
	virtual int requiredInputs() = 0;
	virtual int requiredOutputs() = 0;

};

#endif /* defined(__BasicMarkovBrainTemplate__World__) */
