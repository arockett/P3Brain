//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Agent.h"


bool Agent::serialProcessing=false;

Agent::Agent(){

}

Agent::Agent(ClassicMBGenome *startGenome,int _nrOfStates){
	nrOfStates=_nrOfStates;
	states=new double[nrOfStates];
	nextStates=new double[nrOfStates];
	nodeMap=new int[256];
	for(int i=0;i<256;i++)
		nodeMap[i]=i%nrOfStates;
	genome=startGenome;
	gates.clear();
	for(int i=0;i<genome->genome.size()-1;i++){
		const int S=genome->genome[i];
		if((S+genome->genome[i+1])==256){
			if(Gate::makeGate[S]!=NULL){
				gates.push_back(Gate::makeGate[S](genome,i));
			}
		}
	}
	for(int i=0;i<gates.size();i++)
		gates[i]->applyNodeMap(nodeMap, _nrOfStates);
}

Agent::~Agent(){
	for(int i=0;i<gates.size();i++)
		delete gates[i];
	delete states;
	delete nextStates;
}

void Agent::resetBrain(){
	for(int i=0;i<nrOfStates;i++)
		states[i]=0.0;
	for(int i=0;i<gates.size();i++)
		gates[i]->resetGate();
}

void Agent::updateStates(){
	/*
    if(serialProcessing){
        //this is a special way of serialized updating
        //experimental feature, should be default off!
        //only change if you really know what you are doing!
        for(int i=0;i<gates.size();i++){
            gates[i]->update(states, states);
        }
    } else {*/
        //this is the default way of parallel updating
        for(int i=0;i<nrOfStates;i++)
            nextStates[i]=0.0;
        for(int i=0;i<gates.size();i++)
            gates[i]->update(states, nextStates);
        for(int i=0;i<nrOfStates;i++)
            states[i]=nextStates[i];
//    }
}

int Agent::Bit(double d){
	if (d>0.0)
		return 1;
	return 0;
}

int Agent::IntFromState(vector<int> I){
	int r=0;
	for(int i=0;i<I.size();i++)
		r=(r<<1)+Bit(states[I[i]]);
	return r;
}

int Agent::IntFromAllStates(){
	int r=0;
	for(int i=0;i<nrOfStates;i++)
		r=(r<<1)+Bit(states[i]);
	return r;
	
}

string Agent::gateList(){
	string S="";
	for(int i=0;i<gates.size();i++){
		S=S+gates[i]->description();
	}
	return S;
}


