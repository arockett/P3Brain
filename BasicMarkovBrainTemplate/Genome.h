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
#include "Parameters.h"

using namespace std;


class Data;

class Genome : public enable_shared_from_this<Genome>{
private:
	static int genomeIDCounter; // used to issue unique ids to Genomes
	int registerGenome(); // get a Genome_id (uses genomeIDCounter)

public:
	static int& initialGenomeSize;
	static double& pointMutationRate;
	static double& insertionRate;
	static double& deletionRate;
	static int& minGenomeSize;
	static int& maxGenomeSize;


public:
	int ID;
	Genome* ancestor;
	int referenceCounter;
	vector<unsigned char> sites;
	map <string,string> data;
	
	Genome();
	Genome(Genome* from);
	virtual ~Genome();
	
	virtual void fillRandom();
	virtual void copyGenome(Genome* from);
	virtual void applyMutations(double mutationRate);
	virtual Genome* makeMutatedOffspring(double mutationRate);
	virtual void makePointMutation();
	virtual void saveToFile(FILE *F);
	virtual void kill();
};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
