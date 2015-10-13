//
//  DecisionMakerWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/27/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__DecisionMakerWorld__
#define __BasicMarkovBrainTemplate__DecisionMakerWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

class DecisionMakerWorld: public World{
private:
	double getPi(double p,double theAlpha);

public:
	static int thinkTime;
	static double pNoise,piNoise,alpha;
	static string DATAFileName;
	DecisionMakerWorld();
	double testIndividual(Agent *agent,bool analyse);
};


#endif /* defined(__BasicMarkovBrainTemplate__DecisionMakerWorld__) */
