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
/* Bit Genome Part */
BitAgent::BitAgent(){
	
}

BitAgent::BitAgent(Genome *startGenome,int _nrOfStates){
	nrOfStates=_nrOfStates;
	states=new double[nrOfStates];
	nextStates=new double[nrOfStates];
	nodeMap=new int[256];
	int bitsPerAddress=ceil(log(nrOfStates)/log(nrOfStates));
	int bitsPerLogic=1<<nrOfInsPerGate;
	int bitsPerGate=(bitsPerAddress*nrOfInsPerGate)+bitsPerLogic;
	for(int i=0;i<256;i++)
		nodeMap[i]=i%nrOfStates;
	genome=startGenome;
	if(_nrOfStates!=32){
		printf("Nr of states MUST be 32!\n");
		exit(0);
	}
	gates.clear();
	for(int i=0;i<nrOfStates;i++){
		int I[nrOfInsPerGate];
		int O=i;
		int logic=0;
		for(int j=0;j<nrOfInsPerGate;j++){
			I[j]=0;
			for(int k=0;k<bitsPerAddress;k++)
				I[j]=(I[j]<<1)+startGenome->sites[(i*bitsPerGate)+(j*bitsPerAddress)+k]&1;
		}
		for(int j=0;j<bitsPerLogic;j++)
			logic=(logic<<1)+startGenome->sites[(bitsPerGate*nrOfInsPerGate)+j]&1;
		DeterministicGate *G=new DeterministicGate();
		G->setupForBits(I,nrOfInsPerGate, O, logic);
		gates.push_back(G);
	}
	for(int i=0;i<gates.size();i++)
		gates[i]->applyNodeMap(nodeMap, _nrOfStates);
}

BitAgent::~BitAgent(){
	for(int i=0;i<gates.size();i++)
		delete gates[i];
	delete states;
	delete nextStates;
	delete nodeMap;	
}



