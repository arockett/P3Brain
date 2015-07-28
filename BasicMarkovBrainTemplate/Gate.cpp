//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Gate.h"
#include <math.h>

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
function<Gate*(ClassicMBGenome*,int)> Gate::makeGate[256];
void Gate::AddGate(int ID, function<Gate*(ClassicMBGenome*,int)> theFunction){
	makeGate[ID]=theFunction;
}

/* *** Gate implementation *** */

/*
 */
Gate::Gate(){
}

Gate::Gate(ClassicMBGenome *genome,int startCodonAt){
	int i,j,k;
	I.clear();
	O.clear();
	int _xDim,_yDim;

	//find the first nucleotide that isn't part of the start codon
	k=(startCodonAt+2)%(int)genome->genome.size();
	
	//get the dimensions of the table
	_xDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	_yDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	
	//prepare the containers for the I and O addresses
	I.resize(_yDim);
	O.resize(_xDim);
	
	for(i=0;i<_yDim;i++)
		I[i]=genome->genome[(k+i)%genome->genome.size()];
	for(i=0;i<_xDim;i++)
		O[i]=genome->genome[(k+4+i)%genome->genome.size()];
	
	k=k+16;
	//get all the values into the table
	table.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		table[i].resize(1<<_xDim);
		double S=0.0;
		for(j=0;j<(1<<_xDim);j++){
			table[i][j]=(double)genome->genome[(k+j+((1<<_xDim)*i))%genome->genome.size()];
			S+=table[i][j];
		}
		//normalize the row
		if(S==0.0){
			for(j=0;j<(1<<_xDim);j++)
				table[i][j]=1.0/(double)(1<<_xDim);
		} else {
			for(j=0;j<(1<<_xDim);j++)
				table[i][j]/=S;
		}
	}

}

Gate::~Gate(){
	
}

int Gate::Bit(double d){
	if (d>0.0)
		return 1;
	return 0;
}

void Gate::update(double *states,double *nextStates){
	int input=0,output=0;
	double r=(double)rand()/(double)RAND_MAX;
	for(int i=0;i<I.size();i++)
		input=(input<<1)+Bit(states[I[i]]);
	while(r>table[input][output]){
		r-=table[input][output];
		output++;
	}
	for(int i=0;i<O.size();i++)
		nextStates[O[i]]+=1.0*((output>>i)&1);
}

string Gate::description(){
	string S="Gate\n";
	S=S+"IN:";
	for(int i=0;i<I.size();i++)
		S=S+" "+to_string(I[i]);
	S=S+"\n";
	S=S+"OUT:";
	for(int i=0;i<O.size();i++)
		S=S+" "+to_string(O[i]);
	S=S+"\n";
	return S;
}

void Gate::applyNodeMap(int *nodeMap,int maxNodes){
	for(int i=0;i<I.size();i++)
		I[i]=nodeMap[I[i]%maxNodes];
	for(int i=0;i<O.size();i++)
		O[i]=nodeMap[O[i]%maxNodes];
}

void Gate::resetGate(void){
		//nothing to reset here!
}

vector<int> Gate::getIns(){
	return I;
}

vector<int> Gate::getOuts(){
	return O;
}


/* *** Determistic Gate Implementation *** */
DeterministicGate::DeterministicGate(){
}

DeterministicGate::DeterministicGate(ClassicMBGenome *genome,int startCodonAt){
	int i,j,k;
	I.clear();
	O.clear();
	int _xDim,_yDim;
	
	//find the first nucleotide that isn't part of the start codon
	k=(startCodonAt+2)%(int)genome->genome.size();
	
	//get the dimensions of the table
	_xDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	_yDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	
	//prepare the containers for the I and O addresses
	I.resize(_yDim);
	O.resize(_xDim);
	
	for(i=0;i<_yDim;i++)
		I[i]=genome->genome[(k+i)%genome->genome.size()];
	for(i=0;i<_xDim;i++)
		O[i]=genome->genome[(k+4+i)%genome->genome.size()];
	
	k=k+16;
	//get all the values into the table
	table.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		table[i].resize(_xDim);
		for(j=0;j<_xDim;j++){
			table[i][j]=1.0*(double)(genome->genome[(k+j+(_xDim*i))%genome->genome.size()]&1);
		}
	}
}

DeterministicGate::~DeterministicGate(){
}

void DeterministicGate::update(double *states,double *nextStates){
	int input=0;
	for(int i=0;i<I.size();i++)
		input=(input<<1)+Bit(states[I[i]]);
	for(int i=0;i<O.size();i++)
		nextStates[O[i]]+=table[input][i];
}

string DeterministicGate::description(){
	return "Deterministic "+Gate::description();
}



/* ***** ADVANCED IMPLEMENTATION! - ENTER ON OWN RISK ***** */

/* *** GP Gate implementation *** */
union intToFloatBitByBit{
	int I;
	float F;
};

GPGate::GPGate(){
	
}

GPGate::GPGate(ClassicMBGenome *genome,int startCodonAt){
	int i,j,k;
	I.clear();
	O.clear();
	int _xDim,_yDim;
	
	//find the first nucleotide that isn't part of the start codon
	k=(startCodonAt+2)%(int)genome->genome.size();
	
	//get the dimensions of the table
	_xDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	_yDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	
	//prepare the containers for the I and O addresses
	I.resize(_yDim);
	O.resize(_xDim);
	
	for(i=0;i<_yDim;i++)
		I[i]=genome->genome[(k+i)%genome->genome.size()];
	for(i=0;i<_xDim;i++)
		O[i]=genome->genome[(k+4+i)%genome->genome.size()];
	k+=8;
	myGateType=genome->genome[(k++)%genome->genome.size()]%8;
	myValues.clear();
	for(i=0;i<_xDim;i++){
		intToFloatBitByBit V;
		V.I=0;
		for(int j=0;j<4;j++)
			V.I=(V.I<<(int)8)+(int)genome->genome[(k++)%genome->genome.size()];
		myValues.push_back(V.F);
	}
}

GPGate::~GPGate(){
	
}

void GPGate::update(double *states,double *nextStates){
	double retValue=I[0];
	int Z=0;
	int i,o;
	bool convReturn=true;
	switch(myGateType){
		case 0: //constant
			convReturn=false;
			for(o=0;o<O.size();o++)
				nextStates[O[o]]=myValues[o];
			break;
		case 1: // add
			for(i=1;i<I.size();i++)
				retValue+=states[I[i]];
			break;
		case 2: // sub
			for(i=1;i<I.size();i++)
				retValue-=states[I[i]];
			break;
		case 3: // mult
			for(i=1;i<I.size();i++)
				retValue*=states[I[i]];
			break;
		case 4: // div
			for(i=1;i<I.size();i++)
				retValue/=states[I[i]];
			break;
		case 5: // sin
			convReturn=false;
			for(o=0;o<O.size();o++){
				nextStates[O[o]]+=sin(states[I[Z]]);
				Z=(Z+1)%(int)I.size();
			}
			break;
		case 6: // cos
			convReturn=false;
			for(o=0;o<O.size();o++){
				nextStates[O[o]]+=cos(states[I[Z]]);
				Z=(Z+1)%(int)I.size();
			}
			break;
		case 7: // log
			convReturn=false;
			for(o=0;o<O.size();o++){
				if(I[Z]>0.0)
					nextStates[O[o]]+=log(states[I[Z]]);
				Z=(Z+1)%(int)I.size();
			}
			break;
		case 8: // exp
			convReturn=false;
			for(o=0;o<O.size();o++){
				nextStates[O[o]]+=exp(states[I[Z]]);
				Z=(Z+1)%(int)I.size();
			}
			break;
	}
	if(convReturn){
		for(int o=0;o<O.size();o++)
			nextStates[O[o]]+=retValue;
	}
}

string GPGate::description(){
	string gateTypeName[8]={"+","-","*","/","Sin","Cos","Log","Exp"};
	return "GP "+gateTypeName[myGateType]+"\n"+Gate::description();
}

/* *** Feedback Gate *** */
bool FeedbackGate::feedbackON=true;

FeedbackGate::FeedbackGate(){

}

FeedbackGate::FeedbackGate(ClassicMBGenome *genome,int startCodonAt){
	int i,j,k;
	I.clear();
	O.clear();
	int _xDim,_yDim;
	
	//find the first nucleotide that isn't part of the start codon
	k=(startCodonAt+2)%(int)genome->genome.size();
	
	//get the dimensions of the table
	_xDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	_yDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	
	//get the dimensions of the feedback buffer
	posFBNode=genome->genome[(k++)%genome->genome.size()];
	negFBNode=genome->genome[(k++)%genome->genome.size()];
	nrPos=genome->genome[(k++)%genome->genome.size()]&3;
	nrNeg=genome->genome[(k++)%genome->genome.size()]&3;
	
	//prepare the containers for the I and O addresses
	I.resize(_yDim);
	O.resize(_xDim);

	//prepare the containers for the feedback pipelines
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);

	//get the IO addresses
	for(i=0;i<_yDim;i++)
		I[i]=genome->genome[(k+i)%genome->genome.size()];
	for(i=0;i<_xDim;i++)
		O[i]=genome->genome[(k+4+i)%genome->genome.size()];
	
	//get the Feedback forces
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=((double)(1+genome->genome[(k+8+i)%genome->genome.size()]))/256.0;
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=((double)(1+genome->genome[(k+12+i)%genome->genome.size()]))/256.0;
	

	k=k+16;
	//get all the values into the table
	table.resize(1<<_yDim);
	originalTable.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		table[i].resize(1<<_xDim);
		originalTable[i].resize(1<<_xDim);
		double S=0.0;
		for(j=0;j<(1<<_xDim);j++){
			table[i][j]=(double)genome->genome[(k+j+((1<<_xDim)*i))%genome->genome.size()];
			S+=table[i][j];
		}
		//normalize the row
		if(S==0.0){
			for(j=0;j<(1<<_xDim);j++)
				table[i][j]=1.0/(double)(1<<_xDim);
		} else {
			for(j=0;j<(1<<_xDim);j++)
				table[i][j]/=S;
		}
		for(j=0;j<(1<<_xDim);j++)
			originalTable[i][j]=table[i][j];
	}

	
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
}
FeedbackGate::~FeedbackGate(){
	
}

void FeedbackGate::update(double *states,double *nextStates){
	int i;
	double mod;
	
	//Apply the feedback
	if((feedbackON)&&(nrPos!=0)&&(states[posFBNode]>0.0)){
		for(i=0;i<chosenInPos.size();i++){
			mod=((double)rand()/(double)RAND_MAX)*posLevelOfFB[i];
			table[chosenInPos[i]][chosenOutPos[i]]+=mod;
			double s=0.0;
			for(int k=0;k<table[chosenInPos[i]].size();k++)
				s+=table[chosenInPos[i]][k];
			for(int k=0;k<table[chosenInPos[i]].size();k++)
				table[chosenInPos[i]][k]/=s;
		}
	}
	if((feedbackON)&&(nrNeg!=0)&&(states[negFBNode]>0.0)){
		for(i=0;i<chosenInNeg.size();i++){
			mod=((double)rand()/(double)RAND_MAX)*negLevelOfFB[i];
			table[chosenInNeg[i]][chosenOutNeg[i]]-=mod;
			if(table[chosenInNeg[i]][chosenOutNeg[i]]<0.001)
				table[chosenInNeg[i]][chosenOutNeg[i]]=0.001;
			double s=0.0;
			for(int k=0;k<table[chosenInNeg[i]].size();k++)
				s+=table[chosenInNeg[i]][k];
			for(int k=0;k<table[chosenInNeg[i]].size();k++)
				table[chosenInNeg[i]][k]/=s;
		}
	}
	 
	//do the logic of the gate
	int input=0;
	int output=0;
	double r=(double)rand()/(double)RAND_MAX;
	for(int i=0;i<I.size();i++)
		input=(input<<1)+Bit(states[I[i]]);
	while(r>table[input][output]){
		r-=table[input][output];
		output++;
	}
	for(int i=0;i<O.size();i++)
		nextStates[O[i]]+=1.0*((output>>i)&1);

	//remember the last actions for future feedback
	if(feedbackON){
		chosenInPos.push_back(input);
		chosenInNeg.push_back(input);
		chosenOutPos.push_back(output);
		chosenOutNeg.push_back(output);
		while(chosenInPos.size()>nrPos)
		{
			chosenInPos.pop_front();
		}
		while(chosenOutPos.size()>nrPos)
		{
			chosenOutPos.pop_front();
		}
		while(chosenInNeg.size()>nrNeg)
		{
			chosenInNeg.pop_front();
		}
		while(chosenOutNeg.size()>nrNeg)
		{
			chosenOutNeg.pop_front();
		}
	}
}

string FeedbackGate::description(){
	string S="pos node:"+to_string(posFBNode)+"\n neg node:"+to_string(negFBNode);
	return "Feedback Gate\n "+S+"\n"+Gate::description();
}

void FeedbackGate::applyNodeMap(int *nodeMap, int maxNodes){
	Gate::applyNodeMap(nodeMap, maxNodes);
	posFBNode=nodeMap[posFBNode%maxNodes];
	negFBNode=nodeMap[negFBNode%maxNodes];
}

void FeedbackGate::resetGate(){
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
	for(int i=0;i<table.size();i++)
		for(int j=0;j<table[i].size();j++)
			table[i][j]=originalTable[i][j];
}

vector<int> FeedbackGate::getIns(){
	vector<int> R;
	R.insert(R.begin(),I.begin(),I.end());
	R.push_back(posFBNode);
	R.push_back(negFBNode);
	return R;
}

/* *** Threshold Gate *** */
Thresholdgate::Thresholdgate(){
	
}

Thresholdgate::Thresholdgate(ClassicMBGenome *genome,int startCodonAt){
	int i,k;
	I.clear();
	O.clear();
	int _xDim;
	
	//find the first nucleotide that isn't part of the start codon
	k=(startCodonAt+2)%(int)genome->genome.size();
	
	//The dimensions are different here, the first I is the trigger
	//the next one are the current state Counter
	//the outputSize is the same, the first bit of the output is the ping
	//the others are the current state counter, they are the same as the inputs!
	//get the dimensions of the table
	_xDim=1+(genome->genome[(k++)%genome->genome.size()]&7);
	
	//prepare the containers for the I and O addresses
	I.resize(_xDim);
	O.resize(_xDim);
	
	I[0]=genome->genome[k%genome->genome.size()];
	for(i=1;i<_xDim;i++){
		I[i]=genome->genome[(k+i)%genome->genome.size()];
		O[i]=genome->genome[(k+i)%genome->genome.size()];
	}
	//keep calm, i is at the right value
	O[0]=genome->genome[(k+i)%genome->genome.size()];
	
	//yes the threshold can be higher than the maximal number countable to by this threshold counter
	threshold=genome->genome[(k+i+1)%genome->genome.size()];
}

Thresholdgate::~Thresholdgate(){
	
}

void Thresholdgate::update(double *states,double *nextStates){
	int theCount=0;
	for(int i=1;i<I.size();i++)
		theCount=(theCount<<1)+Bit(states[I[i]]);
	theCount+=Bit(states[I[0]]);
	if(theCount>threshold)
		states[O[0]]+=1.0;
	for(int i=1;i<O.size();i++)
		nextStates[O[i]]+=1.0*((theCount>>(i-1))&1);
}

string Thresholdgate::description(){
	return "ThresholdgateGate: "+to_string(threshold)+"\n"+Gate::description();;
}


/* *** Fixed Epison Gate *** */
/* this gate behaves like a deterministic gate with a constant externally set error */

double FixedEpsilonGate::epsilon=0.0;

FixedEpsilonGate::FixedEpsilonGate(){
	
}

FixedEpsilonGate::FixedEpsilonGate(ClassicMBGenome *genome,int startCodonAt){
	int i,j,k;
	I.clear();
	O.clear();
	int _xDim,_yDim;
	
	//find the first nucleotide that isn't part of the start codon
	k=(startCodonAt+2)%(int)genome->genome.size();
	
	//get the dimensions of the table
	_xDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	_yDim=1+(genome->genome[(k++)%genome->genome.size()]&3);
	
	//prepare the containers for the I and O addresses
	I.resize(_yDim);
	O.resize(_xDim);
	
	for(i=0;i<_yDim;i++)
		I[i]=genome->genome[(k+i)%genome->genome.size()];
	for(i=0;i<_xDim;i++)
		O[i]=genome->genome[(k+4+i)%genome->genome.size()];
	
	k=k+16;
	//get all the values into the table
	table.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		table[i].resize(_xDim);
		for(j=0;j<_xDim;j++){
			table[i][j]=1.0*(double)(genome->genome[(k+j+(_xDim*i))%genome->genome.size()]&1);
		}
	}
	//now to the specifics of this gate
	defaultOutput.clear();
	for(int i=0;i<table.size();i++){
		int D=0;
		for(int j=0;j<table[i].size();j++)
			D|=Bit(table[i][j])<<j;
		defaultOutput.push_back(D);
	}
}

FixedEpsilonGate::~FixedEpsilonGate(){
	
}

void FixedEpsilonGate::update(double *states,double *nextStates){
	int input=0;
	for(int i=0;i<I.size();i++)
		input=(input<<1)+Bit(states[I[i]]);
	if(((double)rand()/(double)RAND_MAX)<epsilon){
		//do a random output excluding the one otherwise given
		int output=0;
		do{
			output=rand()&(table.size()-1);
		}while(output==defaultOutput[input]);
		for(int i=0;i<O.size();i++)
			nextStates[O[i]]+=1.0*(double)((output>>i)&1);
	} else {
		//do the default output
		for(int i=0;i<O.size();i++)
			nextStates[O[i]]+=table[input][i];
	}
}

string FixedEpsilonGate::description(){
	return "Fixed Epsilon "+to_string(epsilon)+" "+Gate::description();
}

