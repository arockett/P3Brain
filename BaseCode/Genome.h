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

#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;

class Genome {
private:

public:
	static int& initialGenomeSize;
	static double& pointMutationRate;
	static double& insertionRate;
	static double& deletionRate;
	static int& minGenomeSize;
	static int& maxGenomeSize;

public:
	vector<unsigned char> sites;

	Genome() = default;
	Genome(Genome* from);
	virtual ~Genome() = default;

	virtual void fillRandom();
	virtual void copyGenome(Genome* from);
	virtual void applyMutations(double mutationRate);
	virtual Genome* makeMutatedOffspring(double mutationRate);
	virtual void makePointMutation();

	void advanceIndex(int& genomeIndex, int distance = 1) {
		genomeIndex = (genomeIndex + distance) % (int) sites.size();
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
