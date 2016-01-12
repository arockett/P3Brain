//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__ByteGenome__
#define __BasicMarkovBrainTemplate__ByteGenome__

#include "Genome.h"

using namespace std;

class ByteGenome : public Genome {
 private:

 public:

	vector<unsigned char> sites;  // move to private ASAP

	static int& initialGenomeSize;
	static int& minGenomeSize;
	static int& maxGenomeSize;
	static double& pointMutationRate;
	static double& insertionRate;
	static double& deletionRate;

	ByteGenome() = default;
	ByteGenome(shared_ptr<ByteGenome> from);
	virtual ~ByteGenome() = default;

	virtual void fillRandom(int size = ByteGenome::initialGenomeSize);
	virtual inline void assignValue(Index index, int value) {
		//sites[index.current[0]] = value;
	}

	// copy functions
	virtual void copyGenome(shared_ptr<ByteGenome> from);

	virtual shared_ptr<Genome> makeMutatedGenome();
	virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<ByteGenome>> from);

	// IO functions
	virtual void loadGenome(string fileName, string key, string keyValue);

	virtual string convert_to_string();
	virtual vector<string> getStats();
	virtual inline int getSize() {
		return sites.size();
	}

	// mutation functions
	virtual void mutate();
	virtual void applyMutations(double _pointMutationRate = pointMutationRate, double _insertionRate = insertionRate, double _deletionRate = deletionRate, int _minGenomeSize = minGenomeSize, int _maxGenomeSize = maxGenomeSize);
	virtual void makePointMutation();

	virtual int extractValue(shared_ptr<Index> index, vector<int> valueRange, int code = -1, int gateID = 0);

};

#endif /* defined(__BasicMarkovBrainTemplate__ByteGenome__) */

