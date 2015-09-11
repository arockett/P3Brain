//
//  FrenchFlagWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Heather Goldsby 9.10.2015
//  Copyright (c) 2015 Heather Goldsby. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__FrenchFlagWorld__
#define __BasicMarkovBrainTemplate__FrenchFlagWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

class FrenchFlagWorld: public World{
private:

public:
    static int max_x, max_y;
	FrenchFlagWorld();
	double testIndividual(Agent *agent,bool analyse);
};


#endif
