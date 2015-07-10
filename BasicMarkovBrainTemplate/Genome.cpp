//
//  Genome.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Genome.h"
#include <random>

Genome::Genome(){
	referenceCounter=1;
	ancestor=NULL;
	ID=-1;
}

Genome::Genome(Genome *from){
	referenceCounter=1;
	ancestor=from;
	from->referenceCounter++;
	copyGenome(from);
	ID=Data::registerGenome(this);
}

Genome::~Genome(){
	if(ancestor!=NULL){
		ancestor->kill();
	}
}

void Genome::copyGenome(Genome *from){
}

void Genome::applyMutations(double mutationRate){
}


void Genome::kill(){
	referenceCounter--;
	if(referenceCounter<=0){
		Data::removeGenome(this);
		delete this;
	}
}

void Genome::fillRandom(){
	
}

Genome* Genome::makeMutatedOffspring(double mutationRate){
	Genome* G=new Genome(this);
	G->applyMutations(mutationRate);
	return G;
}

void Genome::saveToFile(FILE *F){
	fprintf(F,"<empty>\n");
}

/* *** classic MB Genome implementation *** */

double ClassicMBGenome::insertionDeletionP=0.02;

ClassicMBGenome::ClassicMBGenome(){
	referenceCounter=1;
	ancestor=NULL;
}

ClassicMBGenome::ClassicMBGenome(Genome *from){
	referenceCounter=1;
	ancestor=from;
	from->referenceCounter++;
	copyGenome(from);
	ID=Data::registerGenome(this);
}


void ClassicMBGenome::fillRandom(){
	genome.resize(5000);
	for(int i=0;i<genome.size();i++)
		genome[i]=(unsigned char)rand()&255;
	for(int codon=42;codon<50;codon++)
		for(int i=0;i<4;i++){
			int j=rand()%(int)(genome.size()-2);
			genome[j]=codon;
			genome[j+1]=256-codon;
		}
}

void ClassicMBGenome::copyGenome(Genome *from){
	ClassicMBGenome *who=(ClassicMBGenome*)from;
	genome.resize(who->genome.size());
	for(int i=0;i<genome.size();i++)
		genome[i]=who->genome[i];
}

void ClassicMBGenome::applyMutations(double mutationRate){
	if(mutationRate>0.0){
		int nucleotides=(int)genome.size();
		int i,s,o,w;
		vector<unsigned char> buffer;
		default_random_engine generator;
		binomial_distribution<int> distribution(nucleotides,mutationRate);
		int localMutations= distribution(generator);
		for(i=0;i<localMutations;i++)
			genome[rand()%nucleotides]=rand()&255;
		if((((double)rand()/(double)RAND_MAX)<insertionDeletionP)&&(genome.size()<20000)){
			//duplication
			w=128+rand()%(512-128);
			s=rand()%((int)genome.size()-w);
			o=rand()%(int)genome.size();
			buffer.clear();
			buffer.insert(buffer.begin(),genome.begin()+s,genome.begin()+s+w);
			genome.insert(genome.begin()+o,buffer.begin(),buffer.end());
		}
		if((((double)rand()/(double)RAND_MAX)<insertionDeletionP)&&(genome.size()>1000)){
			//deletion
			w=128+rand()%(512-128);
			s=rand()%((int)genome.size()-w);
			genome.erase(genome.begin()+s,genome.begin()+s+w);
		}
	}
}

void ClassicMBGenome::makePointMutation(){
	genome[rand()%genome.size()]=(unsigned char)(rand()&255);
}


Genome* ClassicMBGenome::makeMutatedOffspring(double mutationRate){
	ClassicMBGenome* G=new ClassicMBGenome(this);
	G->applyMutations(mutationRate);
	return (Genome*)G;
}

void ClassicMBGenome::saveToFile(FILE *F){
	for(int i=0;i<genome.size();i++)
		fprintf(F,"	%i",genome[i]);
	fprintf(F,"\n");
}

