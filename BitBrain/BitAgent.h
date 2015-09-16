//
//  BitBrainAgent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 9/7/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__BitBrainAgent__
#define __BasicMarkovBrainTemplate__BitBrainAgent__

#include <stdlib.h>
#include <vector>
#include "Genome.h"
#include "Data.h"
#include "Gate.h"
#include "Agent.h"

using namespace std;

class Data;

class BitAgent:public Agent{
public:
	static int nrOfInsPerGate;
	BitAgent();
	BitAgent(Genome *startGenome,int _nrOfStates);
	~BitAgent();
};

#endif /* defined(__BasicMarkovBrainTemplate__BitBrainAgent__) */
