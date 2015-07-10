//
//  BlinkyWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/2/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__BlinkyWorld__
#define __BasicMarkovBrainTemplate__BlinkyWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

#define xDim 128
#define yDim 128

const int xm[8]={0,1,1,1,0,-1,-1,-1};
const int ym[8]={-1,-1,0,1,1,1,0,-1};

class BlinkyWorld: public World{
	double testIndividual(Agent *agent,bool analyse);
public:
	static bool doHammer;
	static bool constantHome;
	static bool constantFood;
	static int defaultDist;
	static double hammerP;
	static double externalCognitiveNoise;
};

#endif /* defined(__BasicMarkovBrainTemplate__BlinkyWorld__) */
