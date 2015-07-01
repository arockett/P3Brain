//
//  RedBlueBerryWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__RedBlueBerryWorld__
#define __BasicMarkovBrainTemplate__RedBlueBerryWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

#define xDim 16
#define yDim 16

const int xm[8]={0,1,1,1,0,-1,-1,-1};
const int ym[8]={-1,-1,0,1,1,1,0,-1};

class MadBotAI;

class RedBlueBerryWorld: public World{
	double testIndividual(Agent *agent,bool analyse);
public:
	static double TSK;
};

#endif /* defined(__BasicMarkovBrainTemplate__RedBlueBerryWorld__) */
