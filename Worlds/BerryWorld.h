//
//  BerryWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__BerryWorld__
#define __BasicMarkovBrainTemplate__BerryWorld__

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../BaseCode/Brain.h"
#include "../BaseCode/Genome.h"
#include "../BaseCode/World.h"
#include "../Analyse/Analyse.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Utilities.h"

using namespace std;

class MadBotAI;

class BerryWorld: public World {
public:
    const int xm[8] = { 0, 1, 1, 1, 0, -1, -1, -1 }; //these are directions
    const int ym[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };

    const int EMPTY = 0;
    const int RED = 1;
    const int BLUE = 2;
    const int WALL = 9;

    static double& TSK;
    static int& worldUpdates;
    static int& foodSourceTypes;
    static double& rewardForFood1;
    static double& rewardForFood2;
    static double& rewardForFood3;
    static double& rewardForFood4;
    static double& rewardForFood5;
    static double& rewardForFood6;
    static double& rewardForFood7;
    static double& rewardForFood8;

    static int& worldColumns;
    static int& worldRows;
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
    double testIndividual(shared_ptr<Organism> org, bool analyse);

public:
    BerryWorld();

};

#endif /* defined(__BasicMarkovBrainTemplate__BerryWorld__) */
