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

class Data;

class Genome: public enable_shared_from_this<Genome> {
private:
	static int genomeIDCounter; // used to issue unique ids to Genomes
	int registerGenome(); // get a Genome_id (uses genomeIDCounter)

public:
	static Genome* MRCA;

	static int& initialGenomeSize;
	static double& pointMutationRate;
	static double& insertionRate;
	static double& deletionRate;
	static int& minGenomeSize;
	static int& maxGenomeSize;

public:
	int ID;
	int birthDate;
	Genome* ancestor;
	int referenceCounter;
	vector<unsigned char> sites;
	DataMap dataMap;

	Genome();
	Genome(Genome* from);
	virtual ~Genome();

	virtual void fillRandom();
	virtual void copyGenome(Genome* from);
	virtual void applyMutations(double mutationRate);
	virtual Genome* makeMutatedOffspring(double mutationRate);
	virtual void makePointMutation();
	virtual void kill();

	virtual vector<string> GetLODItem(string key);
	virtual vector<Genome*> getLOD();
	virtual Genome* getMostRecentCommonAncestor();

	virtual void saveDataOnLOD(int flush = 0); // for genome, save to file data for this genome and it's LOD
	virtual void flushDataOnLOD(); // used at the end of a run to save data newer then the MRCA / convergance point

	void advanceIndex(int& genomeIndex, int distance = 1) {
		genomeIndex = (genomeIndex + distance) % (int) sites.size();
	}
};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
