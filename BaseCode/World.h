//
//  World.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__World__
#define __BasicMarkovBrainTemplate__World__

#include <stdlib.h>
#include <thread>
#include <vector>

#include "Agent.h"

using namespace std;

class World{
public:
	static int& repeats;

public:
	World();
	virtual ~World();
	virtual vector<double> evaluateFitness(vector<Agent*> agents,bool analyse);
	virtual double testIndividual(Agent *agent,bool analyse);
};

class ExampleWorld: public World{
public:
	ExampleWorld();
	~ExampleWorld();
	virtual vector<double> evaluateFitness(vector<Agent*> agents,bool analyse);
	double testIndividual(Agent *agent,bool analyse);
};

class MultiThreadWorld: public World{
private:
	int nthreads;
public:
//	void threadedEvaluateFitness(int chunkBegin, int chunkEnd, const vector<Agent*>& agents, int& evaluations);
	
    MultiThreadWorld();
   ~ MultiThreadWorld();
	virtual vector<double> evaluateFitness(vector<Agent*> agents,bool analyse);
	//virtual double testIndividual(Agent *agent,bool analyse);
	static void staticTestIndividual(Agent *agent,bool analyse);
};
#endif /* defined(__BasicMarkovBrainTemplate__World__) */
