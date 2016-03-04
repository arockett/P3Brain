//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Chromosome__
#define __BasicMarkovBrainTemplate__Chromosome__

#include <algorithm>
#include <set>
#include <stdlib.h>
#include <vector>

#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"

using namespace std;

class AbstractChromosome {
 public:

	virtual ~AbstractChromosome() {
	}
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0) = 0;
	virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax, bool readDirection) = 0;
	virtual bool writeDouble(int &siteIndex, double value, double valueMin, double valueMax, bool readDirection) {
		cout << "ERROR: writeDouble() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, bool readDirection, int code, int CodingRegionIndex) {
		cout << "ERROR: siteToDouble() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return false;
	}

	virtual shared_ptr<AbstractChromosome> makeLike() = 0;

	virtual shared_ptr<AbstractChromosome> makeCopy() = 0;

	virtual void fillRandom() {
		cout << "ERROR: fillRandom() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}
	virtual void fillRandom(int length) {
		cout << "ERROR: fillRandom(int length) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void fillAcending(int &start) {
		cout << "ERROR: fillAcending(int &start) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void fillConstant(const int value) {
		cout << "ERROR: fillConstant(const int value) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void readChromosomeFromSS(stringstream &ss, int _chromosomeLength) {
		cout << "ERROR: readChromosomeFromSS(stringstream &ss, int _chromosomeLength) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual string chromosomeToStr() = 0;

	virtual void resize(int size) {
		cout << "ERROR: resize(int size) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual int size() {
		cout << "ERROR: size() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return -1;
	}

	virtual inline bool modulateIndex(int &siteIndex) {
		cout << "ERROR: modulateIndex(int &siteIndex) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return false;  // not at end of chromosome
	}
	virtual inline bool advanceIndex(int &siteIndex, bool readDirection = 1, int distance = 1) {
		cout << "ERROR: advanceIndex(int &siteIndex, bool readDirection = 1, int distance = 1) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return false;  // not at end of chromosome
	}

	virtual vector<string> getFixedStats() {
		cout << "ERROR: getFixedStats() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		vector<string> data;
		return data;
	}

	virtual vector<string> getStats() {
		cout << "ERROR: getStats() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		vector<string> data;
		return data;
	}

	virtual shared_ptr<AbstractChromosome> getSegment(int minSize, int maxSize) {
		cout << "ERROR: getSegment(int minSize, int maxSize) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
		return nullptr;
	}

	virtual void insertSegment(shared_ptr<AbstractChromosome> segment) {
		cout << "ERROR: insertSegment(shared_ptr<AbstractChromosome> segment) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void mutatePoint() {
		cout << "ERROR: mutatePoint() in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) {
		cout << "ERROR: mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) {
		cout << "ERROR: mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}

	virtual void crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount) {
		cout << "ERROR: crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount) in AbstractChromosome was called!\n This has not been implemented yet the chromosome class you are using!\n";
		exit(1);
	}
};

template<class T> class Chromosome : public AbstractChromosome {
	vector<T> sites;
 public:

	class CodingRegions {
 	private:
		map<int, vector<pair<int, int>>> regions;

	public:

		// set a coding region value if that value is > -1 (values < 0 denote that the
		// action should not be recorded in codingRegions
		virtual void assignCode(int code, int siteIndex, int CodingRegionIndex);

		// converts codingRegions to a string. If argument is provided, then only that region will
		// will be returned (or a blank string)
		virtual string codingRegionsToString(int CodingRegionIndex = -1);
	};

	double alphabetSize;  // values in this chromosome will be in the range of 0 to < alphabetSize
	                      // i.e. the number of diffrent values that can be at a site
	                      // for double this is the range of the value
	CodingRegions codingRegions;

	                      // default fail constructor - if alphebetSize default has not been explicitly defined, this is run.
	Chromosome();

	Chromosome(int chromosomeLength);

	Chromosome(int chromosomeLength, double _alphabetSize);

	virtual ~Chromosome () = default;

	// return a shared_ptr to a new chromosome like this one (same alphabetSize and sites.size())
	virtual shared_ptr<AbstractChromosome> makeLike();
	virtual shared_ptr<AbstractChromosome> makeCopy();

	// insures that a site index is valid, if in index is > sites.size(), mod it.
	// return true if siteIndex went out of range
	virtual inline bool modulateIndex(int &siteIndex) {
		if (siteIndex >= (int)sites.size() || siteIndex<0) {
			siteIndex = loopMod(siteIndex,(int)sites.size());
			return true;
		}
		return false;
	}

	// advance or reverse (if readDirection = false) a site index 1 or distance sites and check that new index is valid
	// return true if siteIndex went out of range
	virtual inline bool advanceIndex(int &siteIndex, bool readDirection = 1, int distance = 1) {
		siteIndex += (readDirection)?distance:(-1*distance);  //move index
		return modulateIndex(siteIndex);// confirm that new index is in range
	}

	// read an int value in range [valueMin,valueMax] from chromosome starting at index.
	// will use as many sites as needed. works in base alphabetSize
	// return true if siteIndex went out of range
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0);

	// writes a value into a chromosome, uses a number of sites baised on valueMin and valueMax);
	// return true if siteIndex went out of range
	virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax, bool readDirection);
	// read one site of type T
	// return true if siteIndex went out of range
	virtual bool readSite(int &siteIndex, T &value, bool readDirection, int code = -1, int CodingRegionIndex = 0);

	// scale value using valueMin and valueMax to alphabetSize and write at siteIndex
	// value - MIN(valueMin,valueMax) must be < ABS(valueMax - valueMin)
	virtual bool writeDouble(int &siteIndex, double value, double valueMin, double valueMax, bool readDirection);

	// this is a scaling function - while it will work with other types it should only be used with double and float
	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0);

	// resize a chromosome to length and fill will values from alphabet
	virtual void fillRandom(int length) override;
	virtual void fillRandom() override;

	// starting with value start, fill this chromosome with acending values.
	// This function is to make testing easy.
	virtual void fillAcending(int &start) override;

	// fill all the sites of this chromosome with value.
	// This function is to make testing easy.
	virtual void fillConstant(const int value) override;

	virtual void readChromosomeFromSS(std::stringstream &ss, int _chromosomeLength);
	// convert a chromosome to a string
	virtual string chromosomeToStr();
	virtual void resize(int size);
	virtual int size();
	virtual vector<string> getFixedStats();
	virtual vector<string> getStats();
	virtual string codingRegionsToString();

	//////////////////////////////
	// MUTATION RELATED METHODS //
	//////////////////////////////

	// create a new chromosome with a copied peice of this chromosome. The size of the copy
	// will be random between minSize and maxSize. the copy location will be determined randomly
	// but will at lease segmentSize before the end of the chromosome.
	virtual shared_ptr<AbstractChromosome> getSegment(int minSize, int maxSize);
	// insert from segement into this chromosome. The insertion location is random.
	virtual void insertSegment(shared_ptr<AbstractChromosome> segment);
	virtual void mutatePoint();
	// mutate chromosome by getting a copy of a segment of this chromosome and
	// inserting that segment randomly into this chromosome
	virtual void mutateCopy(int minSize, int maxSize, int chromosomeSizeMax);
	// delete a random segement from the chromosome
	virtual void mutateDelete(int minSize, int maxSize, int chromosomeSizeMin);
	// delete the sites of this chromosome. Then set sites to a crossed over chromosome made up of parents
	virtual void crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount);
};

#endif /* defined(__BasicMarkovBrainTemplate__Chromosome__) */

