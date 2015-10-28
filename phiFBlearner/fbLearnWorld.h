//
//  fbLearnWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/10/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__fbLearnWorld__
#define __BasicMarkovBrainTemplate__fbLearnWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

//class MadBotAI;

class fbLearnWorld: public World{
private:
	int mapping[24][4]={
		{0,1,2,3} , {1,0,2,3} , {2,1,0,3} , {3,1,2,0},
		{0,1,3,2} , {1,0,3,2} , {2,1,3,0} , {3,1,0,2},
		{0,2,1,3} , {1,2,0,3} , {2,0,1,3} , {3,2,1,0},
		{0,2,3,1} , {1,2,3,0} , {2,0,3,1} , {3,2,0,1},
		{0,3,1,2} , {1,3,0,2} , {2,3,1,0} , {3,0,1,2},
		{0,3,2,1} , {1,3,2,0} , {2,3,0,1} , {3,0,2,1}
	};
	int subsample[24]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
	int currentMapID;
	void makeMap();
	int makeNumberNotBorder(int range);
	vector<vector<int> > distMap;
	vector<vector<int> > area;
	vector<vector<int> > dirMap;
	int targetX,targetY,startX,startY;
	map<int,map<int,vector<int>>> phiDataCollector;
public:
	
	fbLearnWorld();
	vector<double> evaluateFitness(vector<Agent*> agents,bool analyse);
	double testIndividual(Agent *agent,bool analyse);
};

#endif /* defined(__BasicMarkovBrainTemplate__fbLearnWorld__) */
