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
#include "Utilities.h"

#include <unistd.h>

using namespace std;

class MadBotAI;

class RedBlueBerryWorld: public World{
public:
	const int xm[8] = { 0, 1, 1, 1, 0, -1, -1, -1 }; //these are directions
	const int ym[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };

	const int EMPTY = 0;
	const int RED = 1;
	const int BLUE = 2;
	const int WALL = 9;

	static double& TSK;

	static int& foodSourceTypes;
	static double& rewardForFood1;
	static double& rewardForFood2;
	static double& rewardForFood3;
	static double& rewardForFood4;
	static double& rewardForFood5;
	static double& rewardForFood6;
	static double& rewardForFood7;
	static double& rewardForFood8;

	static int& xDim;
	static int& yDim;
	static bool& borderWalls;
	static int& randomWalls;

	static bool& allowMoveAndEat;

	static bool& senseDown;
	static bool& senseFront;
	static bool& senseFrontSides;
	static bool& senseWalls;
	static bool& clearOutputs;

	int inputStatesCount = 0;
	int outputStatesCount = 0;
	double testIndividual(Agent *agent,bool analyse);



public:
	RedBlueBerryWorld();

};

#endif /* defined(__BasicMarkovBrainTemplate__RedBlueBerryWorld__) */
