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
#include <iostream>
#include <fstream>
#include "Parameters.h"

#include <unistd.h>

using namespace std;

#define xDim 8
#define yDim 8

const int xm[8]={0,1,1,1,0,-1,-1,-1}; //these are directions
const int ym[8]={-1,-1,0,1,1,1,0,-1};


class RedBlueBerryWorldSettings{
public:
	static double TSK;
	static void initializeParameters();
};



class MadBotAI;

class RedBlueBerryWorld: public World{
	double testIndividual(Agent *agent,bool analyse);



public:
	RedBlueBerryWorld();
	static double TSK;

};

#endif /* defined(__BasicMarkovBrainTemplate__RedBlueBerryWorld__) */
