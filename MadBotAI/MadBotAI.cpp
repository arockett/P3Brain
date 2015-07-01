//
//  MadBotAI.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "MadBotAI.h"
#include <random>

/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */
/* *** MadBotAI implementation *** */

/* helper class implementation*/
Decision Decision::randomDecision(int actions,int states){
	Decision D;
	D.actionToDo=rand()%actions;
	D.nextState=rand()%states;
	return D;
}
/* *** MadBotGenome Implementation *** */
MadBotGenome::MadBotGenome(){
	referenceCounter=1;
	ancestor=NULL;
	ID=-1;
}

MadBotGenome::MadBotGenome(Genome *from){
	inputs=((MadBotGenome*)from)->inputs;
	actions=((MadBotGenome*)from)->actions;
	states=((MadBotGenome*)from)->states;
	granularity=((MadBotGenome*)from)->granularity;
	referenceCounter=1;
	ancestor=from;
	from->referenceCounter++;
	alloc();
	copyGenome(from);
	ID=Data::registerGenome((Genome*)this);
}

void MadBotGenome::fillRandom(int _inputs,int _actions,int _states,int _granularity){
	inputs=_inputs;
	actions=_actions;
	states=_states;
	granularity=_granularity;
	alloc();
	fillRandom();
	ID=Data::registerGenome((Genome*)this);
}

void MadBotGenome::alloc(){
	brain.resize(inputs*states);
	for(int i=0;i<brain.size();i++)
		brain[i].resize(granularity);
}

void MadBotGenome::fillRandom(){
	for(int i=0;i<states*inputs;i++)
		for(int j=0;j<granularity;j++)
			brain[i][j]=Decision::randomDecision(actions,states);
}

void MadBotGenome::copyGenome(Genome *from){
	for(int i=0;i<states*inputs;i++)
		for(int j=0;j<granularity;j++)
			brain[i][j]=((MadBotGenome*)from)->brain[i][j];
}

void MadBotGenome::applyMutations(double mutationRate){
	default_random_engine generator;
	binomial_distribution<int> distribution(states*inputs*granularity,mutationRate);
	int localMutations= distribution(generator);

	for(int l=0;l<localMutations;l++){
		int i=rand()%(states*inputs);
		int j=rand()%granularity;
		brain[i][j]=Decision::randomDecision(actions, states);
	}
}

Genome* MadBotGenome::makeMutatedOffspring(double mutationRate){
	MadBotGenome* G=new MadBotGenome(this);
	G->applyMutations(mutationRate);
	return (Genome*)G;
}

/* actual implementation */
MadBotAI::MadBotAI(MadBotGenome *startGenome,int _nrOfStates){
	if(startGenome==NULL){
		printf("DUDE! there was an inheritance from a Null pointer in MadBotAI!\n");
		exit(0);
	}
//	int Ibits,Obits,Hbits;
	genome=(ClassicMBGenome*)startGenome;
	Ibits=(int)log2(startGenome->inputs);//*log(2.0));
	Obits=(int)log2(startGenome->actions);//*log(2.0));
	Hbits=(int)log2(startGenome->states);//*log(2.0));
	if(Ibits+Obits+Hbits>=_nrOfStates){
		printf("DUDE! You have a discrepancy between the _nrOfStates and how you initialized the Genome\n");
		printf("I:%i A:%i S:%i G:%i\n",startGenome->inputs,startGenome->actions,startGenome->states,startGenome->granularity);
		printf("BI:%i BO:%i BH:%i\n",Ibits,Obits,Hbits);
		exit(0);
	}
	state=0;
	states=new double[Ibits+Obits+Hbits];
	nextStates=new double[Ibits+Obits+Hbits];
	//printf("Initialized with: BI:%i BO:%i BH:%i\n",Ibits,Obits,Hbits);
}

MadBotAI::~MadBotAI(){
	delete states;
	delete nextStates;
}

void MadBotAI::resetBrain(){
	state=0;
}

void MadBotAI::updateStates(){
	int I=0;
	for(int i=0;i<Ibits;i++)
		if(states[i]>0.0)
			I|=1<<i;
	int R=rand()&((1<<((MadBotGenome*)genome)->granularity)-1);
	R|=1<<(((MadBotGenome*)genome)->granularity-1);
	int A=0;
	while((R&1)==0){
		A++;
		R=R>>1;
	}
	/* this is bullshit!!!
	int R=rand()&((1<<genome->granularity)-1);
	int A=0;
	int L=genome->granularity;
	while(A>(1<<L)){
		L--;
	}
	 */
//	int A=rand()%genome->granularity;
//	A=0;
	int theAction=((MadBotGenome*)genome)->brain[I+(((MadBotGenome*)genome)->states*state)][A].actionToDo;
	state=((MadBotGenome*)genome)->brain[I+(((MadBotGenome*)genome)->states*state)][A].nextState;
	for(int i=0;i<Obits;i++)
		states[Ibits+i]=(double)((theAction>>i)&1);
	for(int i=0;i<Hbits;i++)
		states[Ibits+Obits+i]=(double)((state>>i)&1);
}

string MadBotAI::gateList(){
	return "";
}
