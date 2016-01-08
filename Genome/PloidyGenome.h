////
////  Genome.h
////  BasicMarkovBrainTemplate
////
////  Created by Arend Hintze on 5/29/15.
////  Copyright (c) 2015 Arend Hintze. All rights reserved.
////
//
//#ifndef __BasicMarkovBrainTemplate__PloidyGenome__
//#define __BasicMarkovBrainTemplate__PloidyGenome__
//
//#include "Genome.h"
//
//using namespace std;
//
//class PloidyGenome : public Genome {
// private:
//
// public:
//
//	vector<Genome> chromosomes;  // move to private ASAP
//
//	PloidyGenome() = default;
//	PloidyGenome(shared_ptr<ByteGenome> from);
//	virtual ~PloidyGenome() = default;
//
//	virtual void fillRandom(int chromosomeCount = 2, int siteCount = -1){
//		chromosomes.resize(chromosomeCount);
//		for (int i = 0; i < chromosomeCount; i++){
//			if (siteCount == -1){
//			chromosomes[i].fillRandom();
//			} else {
//			chromosomes[i].fillRandom(siteCount);
//			}
//		}
//	}
//
//	virtual inline void assignValue(int loc, int value) {
//		sites[loc % (int) sites.size()] = value;
//	}
//
//	// copy functions
//	virtual void copyGenome(shared_ptr<ByteGenome> from);
//
//	virtual shared_ptr<Genome> makeMutatedGenome();
//	virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<ByteGenome>> from);
//
//	// IO functions
//	virtual void loadGenome(string fileName, string key, string keyValue);
//
//	virtual string convert_to_string();
//	virtual vector<string> getStats();
//	virtual inline int getSize() {
//		return sites.size();
//	}
//
//	// mutation functions
//	virtual void mutate();
//	virtual void applyMutations(double _pointMutationRate = pointMutationRate, double _insertionRate = insertionRate, double _deletionRate = deletionRate, int _minGenomeSize = minGenomeSize, int _maxGenomeSize = maxGenomeSize);
//	virtual void makePointMutation();
//
//	virtual int extractValue(shared_ptr<Index> index, vector<int> valueRange, int code = -1, int gateID = 0);
//
//};
//
//#endif /* defined(__BasicMarkovBrainTemplate__PloidyGenome__) */
//
