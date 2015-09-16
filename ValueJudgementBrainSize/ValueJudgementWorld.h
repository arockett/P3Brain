//
//  ValueJudgementWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__ValueJudgementWorld__
#define __BasicMarkovBrainTemplate__ValueJudgementWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

class ValueJudgementWorld: public World{
public:
	static int thinkTime;
	static double alpha;
	ValueJudgementWorld();
	double testIndividual(Agent *agent,bool analyse);
};

#endif /* defined(__BasicMarkovBrainTemplate__ValueJudgementWorld__) */
