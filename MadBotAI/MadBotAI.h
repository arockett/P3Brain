//
//  MadBotAI.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__MadBotAI__
#define __BasicMarkovBrainTemplate__MadBotAI__

#include <stdlib.h>
#include "Genome.h"
#include "Agent.h"

using namespace std;

/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */
struct Decision{
	int actionToDo,nextState;
	static Decision randomDecision(int actions,int states);
};

class MadBotGenome : public Genome{
public:
	int state=0;
	vector<vector<Decision>> brain;
	int inputs,actions,states,granularity;
	MadBotGenome();
	MadBotGenome(Genome *from);
	virtual void fillRandom();
	virtual void fillRandom(int _inputs,int _actions,int _states,int _granularity);
	virtual void alloc();
	virtual void copyGenome(Genome *from);
	virtual void applyMutations(double mutationRate);
	virtual Genome* makeMutatedOffspring(double mutationRate);

};

class MadBotAI : public Agent{
private:
	int state;
	void allocBrain();
	void fillRandom();
	int Ibits,Obits,Hbits;
public:
	//MadBotGenome *genome;
	MadBotAI();
	MadBotAI(MadBotGenome *startGenome,int _nrOfStates);
	MadBotAI(MadBotGenome *startGenome);
	~MadBotAI();
	virtual void resetBrain();
	virtual void updateStates();
	virtual string gateList();
	virtual int makeUpdate(int input);
};

#endif /* defined(__BasicMarkovBrainTemplate__MadBotAI__) */
