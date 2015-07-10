//
//  HidatoWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/6/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__HidatoWorld__
#define __BasicMarkovBrainTemplate__HidatoWorld__

#include <stdio.h>
#include <stdlib.h>
#include "Agent.h"
#include "Genome.h"
#include "World.h"
#include "Analyse.h"

using namespace std;

const int xm[4]={0,1,0,-1};
const int ym[4]={-1,0,1,0};

enum tileState {EMPTY,FIXED,WRITTENINTO};

class HidatoPuzzle{
public:
	int xDim,yDim;
	int startX,startY;
	vector<vector<int>> maze;
	vector<vector<int>> state; //0 empty, 1 fixed, 2 written into it
	HidatoPuzzle();
	HidatoPuzzle(vector<int> data);
	~HidatoPuzzle();
	void refreshPuzzle(int missing);
	bool withinBounds(int X,int Y);
	void show();
};

class HidatoWorld: public World{
public:
	static int missing;
	vector<HidatoPuzzle*> puzzles;
	HidatoWorld();
	~HidatoWorld();
	virtual vector<double> evaluateFitness(vector<Agent*> agents,bool analyse);
	virtual double testIndividual(Agent *agent,bool analyse);
};


#endif /* defined(__BasicMarkovBrainTemplate__HidatoWorld__) */
