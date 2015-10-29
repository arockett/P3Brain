//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Agent.h"

#define SKIPGATE 1

bool Agent::serialProcessing=false;

Agent::Agent(){
	
}

Agent::Agent(Genome *startGenome,int _nrOfStates){ //this is a constructor. it is run whenever a new agent is created.
	skipGate = *Data::parameterDouble["skipGate"];
	nrOfStates=_nrOfStates;
	states=new double[nrOfStates];
	nextStates=new double[nrOfStates];
	nodeMap=new int[256];
	for(int i=0;i<256;i++)
		nodeMap[i]=i%nrOfStates;
	genome=startGenome;
	gates.clear();
	for(int i=0;i<genome->sites.size()-1;i++){
		const int S=genome->sites[i];
		if((S+genome->sites[i+1])==256){
			if(Gate::makeGate[S]!=nullptr){
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
	delete nodeMap;
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
	for(int i=0;i<nrOfStates;i++){ //reset all states to 0
		nextStates[i]=0.0;
	}
	for(int i=0;i<gates.size();i++){ //update each gate
#if SKIPGATE==1
		//if(*Data::parameterDouble["skipGate"]>0){
		if(((double)rand()/(double)RAND_MAX)>skipGate){
			gates[i]->update(states, nextStates);
		}
		//}else{
		//gates[i]->update(states, nextStates);
		//}
#else
		gates[i]->update(states, nextStates);
#endif // SKIPGATE
	}
	for(int i=0;i<nrOfStates;i++){ //sets states for t+1 to states just updated in the last line
		states[i]=nextStates[i];
	}
}

int Agent::Bit(double d){ //this is a function that returns a 1 if the number inputted is greater than 0 or returns a 0 if not
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


vector<vector<int>> Agent::getConnectivityMatrix(){
	vector<vector<int>> M;
	M.resize(nrOfStates);
	for(int i=0;i<nrOfStates;i++){
		M[i].resize(nrOfStates);
		for(int j=0;j<nrOfStates;j++)
			M[i][j]=0;
	}
	for(Gate* G:gates){
		vector<int> I=G->getIns();
		vector<int> O=G->getOuts();
		for(int i:I)
			for(int o:O)
				M[i][o]++;
	}
	return M;
}

set<int> Agent::findCodingRegions(int mask){ //if mask=0, all coding regions are returned. if mask=1, return everything except start codon. if mask=2, return everything except start codon and wiring.
	set<int> allCodingRegions;
	for(auto gateIterator = gates.begin(); gateIterator != gates.end(); gateIterator++) {
		for(auto iterator = (*gateIterator)->codingRegions.begin(); iterator != (*gateIterator)->codingRegions.end(); iterator++) {
			if(iterator->second >= mask){
				allCodingRegions.insert(iterator->first);
			}
		}
	}
	/*for(auto iterator = allCodingRegions.begin(); iterator != allCodingRegions.end(); iterator++){
	 cout << *iterator << "(" << (int)genome->genome[*iterator] << ") ";
	 }
	 cout << "\n";
	 */
	return allCodingRegions;
}

int Agent::brainSize(){
	return (int)gates.size();
}

int Agent::numGates(){
	return brainSize();
}

