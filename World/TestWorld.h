//
//  World.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__WorldTest__
#define __BasicMarkovBrainTemplate__WorldTest__

#include "AbstractWorld.h"

#include <stdlib.h>
#include <thread>
#include <vector>

using namespace std;

class TestWorld : public AbstractWorld {

public:
	TestWorld();
	virtual ~TestWorld() = default;
	virtual double testIndividual(shared_ptr<Organism> org, bool analyse, bool show = 0) override;
	virtual int requiredInputs() override;
	virtual int requiredOutputs() override;

};

#endif /* defined(__BasicMarkovBrainTemplate__WorldTest__) */
