//
//  BitBrainAgent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 9/7/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "BitAgent.h"
#include <math.h>

int BitAgent::nrOfInsPerGate=3;
vector<int> BitAgent::nodeMap;

/* Bit Genome Part */
BitAgent::BitAgent(){
	
}

BitAgent::BitAgent(Genome *startGenome,int _nrOfStates){
	// WAS nrOfStates=_nrOfStates;
	nrOfBrainStates=_nrOfStates;
	states.resize(nrOfBrainStates);
	nextStates.resize(nrOfBrainStates);
	nodeMap.resize(256);
	int bitsPerAddress= 1; // this = 1, right?? -> ceil(log(nrOfBrainStates)/log(nrOfBrainStates));
	int bitsPerLogic=1<<nrOfInsPerGate;
	int bitsPerGate=(bitsPerAddress*nrOfInsPerGate)+bitsPerLogic;
	for(int i=0;i<256;i++)
		nodeMap[i]=i%nrOfBrainStates;
	genome=startGenome;
	if(_nrOfStates!=32){
		printf("Nr of states MUST be 32!\n");
		exit(0);
	}
	gates.clear();
	for(int i=0;i<nrOfBrainStates;i++){
		int I[nrOfInsPerGate];
		int O=i;
		int logic=0;
		for(int j=0;j<nrOfInsPerGate;j++){
			I[j]=0;
			for(int k=0;k<bitsPerAddress;k++)
				I[j]=(  (I[j]<<1)  +  startGenome->sites[(i*bitsPerGate)+(j*bitsPerAddress)+k]  )  &  1; // Is this correct - sum, then & 1?
		}
		for(int j=0;j<bitsPerLogic;j++)
			logic=(  (logic<<1) + startGenome->sites[(bitsPerGate*nrOfInsPerGate)+j]  ) & 1; // Is this correct - sum, then & 1?
		// WAS DeterministicGate *G=new DeterministicGate();
		shared_ptr<DeterministicGate> G(new DeterministicGate());
		G->setupForBits(I,nrOfInsPerGate, O, logic);
		gates.resize((int)gates.size()+1);
		gates[(int)gates.size()-1]=G;
	}
	for(size_t i=0;i<gates.size();i++)
		gates[i]->applyNodeMap(nodeMap, _nrOfStates);
}

BitAgent::~BitAgent(){
}



