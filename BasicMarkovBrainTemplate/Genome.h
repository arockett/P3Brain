//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Genome__
#define __BasicMarkovBrainTemplate__Genome__

#include <stdlib.h>
#include <vector>
#include "Data.h"

using namespace std;

class Data;

class Genome{
public:
	int ID;
	Genome *ancestor;
	int referenceCounter;
	Genome();
	Genome(Genome *from);

	virtual ~Genome();
	virtual void fillRandom();
	virtual void copyGenome(Genome *from);
	virtual void applyMutations(double mutationRate);
	virtual Genome* makeMutatedOffspring(double mutationRate);
	virtual void saveToFile(FILE *F);
	virtual void kill();
};

class ClassicMBGenome:public Genome{
public:
	ClassicMBGenome();
	ClassicMBGenome(Genome *from);
	vector<unsigned char> genome;
	static double insertionDeletionP;
	
	virtual void fillRandom();
	virtual void copyGenome(Genome *from);
	virtual void applyMutations(double mutationRate);
	virtual void makePointMutation();
	virtual Genome* makeMutatedOffspring(double mutationRate);
	virtual void saveToFile(FILE *F);
};
#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
