//
//  ValueJudgementWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "ValueJudgementWorld.h"

int ValueJudgementWorld::thinkTime=1;
double ValueJudgementWorld::alpha=0.3;

ValueJudgementWorld::ValueJudgementWorld(){
	
}

double ValueJudgementWorld::testIndividual(Agent *agent, bool analyse){
	double fitness=1.0;
	int correct=0;
	int incorrect=0;
	for(int i=0;i<repeats;i++){
		agent->resetBrain();
		double P=0.0;
		if(rand()&1)
			P=alpha;
		else
			P=1.0-alpha;
		bool timedOut=true;
		for(int j=0;j<thinkTime+10;j++){
			agent->states[0]=((double)rand()/(double)RAND_MAX)<P?1.0:0.0;
			agent->states[1]=((double)rand()/(double)RAND_MAX)<(1.0-P)?1.0:0.0;
			if(j>thinkTime)
				agent->states[2]=1;
			else
				agent->states[2]=0;
			agent->updateStates();
			if(j>thinkTime){
				int action=0;
				if(agent->Bit(agent->states[3])==0)
					action=agent->Bit(agent->states[4]);
				if(agent->Bit(agent->states[5])==0)
					action=agent->Bit(agent->states[6])*2;
				switch(action){
					case 0: case 3: // no decicive answer
						break;
					case 1:
						if(P>(1.0-P)){
							correct++;
						}
						else{
							incorrect++;
						}
						j=thinkTime*100;
						timedOut=false;
						break;
					case 2:
						if(P<(1.0-P)){
							correct++;
						}
						else{
							incorrect++;
						}
						j=thinkTime*100;
						timedOut=false;
						break;
				}
			}
		}
		if(timedOut){
			incorrect++;
		}
	}
	fitness=pow(1.05,(double)correct);
	if(analyse){
		Data::Add(correct, "correct", (Genome*)agent->genome);
		Data::Add(incorrect, "incorrect", (Genome*)agent->genome);
		Data::Add(fitness, "fitness", (Genome*)agent->genome);
		Data::Add(agent->brainSize(),"gates",(Genome*)agent->genome);
		vector<vector<int>> M=agent->getConnectivityMatrix();
		int n=0;
		for(int i=0;i<M.size();i++)
			for(int j=0;j<M[i].size();j++){
				n+=M[i][j];
			}
		Data::Add(n,"connections",(Genome*)agent->genome);
		Data::Add(Analyse::getDensity(M),"density",(Genome*)agent->genome);
	}
	return fitness;
}