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

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "Chromosome.h"

#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/ParametersTable.h"
#include "../Utilities/Random.h"

using namespace std;

class AbstractGenome {

 public:

	static vector<string> genomeFileColumns;

	// Handlers are how you access Genomes for reading and writting.
	// to get a handle for a genome call that that genomes newHandler() method
	class Handler {
 	public:
		//shared_ptr<AbstractGenome> genome;
		bool readDirection;  // true = forward, false = backwards
		bool EOG;  // end of genome

		Handler() {
			readDirection = true;
			EOG = true;
		}

		Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) {
			readDirection = _readDirection;
			EOG = true;
		}

		virtual void resetEOG() {
			EOG = false;
		}

		virtual void setReadDirection(bool _readDirection) {
			readDirection = _readDirection;
		}

		virtual void toggleReadDirection() {
			readDirection = !readDirection;
		}

		virtual void resetHandler() = 0;

		virtual ~Handler() {
		}

		//// convert genome sites at location index into a value in range [min,max] and advance index to the next unused site
		//// any sites used will be assigned code in codingRegions
		//// no undefined action, this function must be defined
		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) = 0;

		virtual double readDouble(double valueMin, double valueMax, int code = -1, int CodingRegionIndex = 0) {
			cout << "ERROR: readDouble(double valueMin, double valueMax, int code = -1, int CodingRegionIndex = 0) in AbstractGenome::Handler was called!\n This has not been implemented yet the chromosome class you are using!\n";
			exit(1);
			return 0.0;
		}

		virtual void writeInt(int value, int valueMin, int valueMax) = 0;
		virtual vector<vector<int>> readTable(pair<int,int> tableSize, pair<int,int> tableMaxSize, pair<int,int> valueRange, int code = -1, int CodingRegionIndex = 0)=0;

		virtual void copyTo(shared_ptr<Handler> to) = 0;

		virtual bool atEOG() {
			return false;
		}

		virtual void printIndex() = 0;

		virtual bool inTelomere(int length) {
			return false;
		}

		virtual void randomize() = 0;
	};

	DataMap dataMap;

	AbstractGenome() = default;

	virtual ~AbstractGenome() = default;
	//virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override {

	virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) = 0;

	virtual void copyFrom(shared_ptr<AbstractGenome> from) = 0;

	virtual void fillRandom() = 0;

	//// gets data about genome which can be added to a data map
	//// data is in pairs of strings (key, value)
	//// the undefined action is to return an empty vector
	virtual vector<string> getStats() {
		vector<string> data;
		cout << "Warning! In AbstractGenome::getStats()...\n";
		return data;
	}

	virtual string genomeToStr() {
		cout << "Warning! In AbstractGenome::genomeToStr()...\n";
		return "";
	}

	virtual void loadGenome(string fileName, string key, string keyValue) {
	}

	virtual void loadGenomes(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) {
	}

	virtual bool isEmpty() = 0;

	virtual void mutate() = 0;
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(shared_ptr<AbstractGenome> parent) = 0;
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents) = 0;

};

class Genome : public AbstractGenome {
 public:

	static int& initialPloidy;
	static int& initialChromosomes;
	static int& initialChromosomeSize;
	static double& pointMutationRate;
	static double& insertionRate;
	static int& insertionMinSize;
	static int& insertionMaxSize;
	static double& deletionRate;
	static int& deletionMinSize;
	static int& deletionMaxSize;
	static int& maxChromosomeSize;
	static int& minChromosomeSize;
	static int& crossCount;  // number of crosses to make when performing crossover

	static vector<string> genomeFileColumns;  // = {"ID","alphabetSize","chromosomeCount","chromosomeLength","sitesCount","genomeAncestors","sites"};

	class Handler : public AbstractGenome::Handler {
 	public:
		shared_ptr<Genome> genome;
		int siteIndex;
		int chromosomeIndex;

		Handler() = delete;

		Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection = 1);
		virtual ~Handler() = default;

		virtual void resetHandler() override;

		// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
		// it uses readDirection to resolve them.	virtual void copyFrom(shared_ptr<Genome> from) {

		//  modulate index truncates nonexistant sites. i.e. if the current addres is chromosome 1, site 10 and
		// chromosome 10 is 8 long, modulateIndex will set the index to chromosome 2, site 0 (not site 2).
		// If this behavior is required, use advance Index instead.
		// If the chromosomeIndex has past the last chromosome (or the first
		// if read direction = -1) then EOG (end of genome) is set true.
		virtual void modulateIndex();

		// advanceIndex will move the index forward distance sites.
		// if there are too few sites, it will advance to the next chromosome and then advance addtional sites (if needed)
		// NOTE: if the advance is > the current chromosome size, it will be modded to the chromosome size.
		// i.e. if the chromosome was length 10, and the current siteIndex = 0, advanceIndex(15) will advance to
		// site 5 of the next chromosome. Should this be fixed?!??
		virtual void advanceIndex(int distance = 1);

		// returns true if this Handler has reached the end of genome (or start if direction is backwards).
		virtual bool atEOG() override;
		virtual void advanceChromosome();
		virtual void printIndex() override;
		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) override;
		virtual double readDouble(double valueMin, double valueMax, int code = -1, int CodingRegionIndex = 0) override;

		virtual void writeInt(int value, int valueMin, int valueMax) override;
		// copy contents of this handler to "to"
		virtual void copyTo(shared_ptr<AbstractGenome::Handler> to) override;
		// true if handler is within length sites from end of a chromosome
		virtual bool inTelomere(int length) override;
		// move this handler to a random location in genome
		virtual void randomize() override;
		virtual vector<vector<int>> readTable(pair<int,int> tableSize, pair<int,int> tableMaxSize, pair<int,int> valueRange, int code = -1, int CodingRegionIndex = 0) override;

	};
 public:
	ParametersTable PT;
	int ploidy;
	vector<shared_ptr<AbstractChromosome>> chromosomes;

	Genome();
	Genome(shared_ptr<AbstractChromosome> _chromosome);
	Genome(shared_ptr<AbstractChromosome> _chromosome, int chromosomeCount, int _plodiy = 1);
	virtual ~Genome() = default;

	virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override;

	// randomize this genomes contents
	virtual void fillRandom() override;

	// fill all sites of this genome with ascending values
	// This function is to make testing easy.
	virtual void fillAcending();

	// fill all sites of this genome with value
	// if "acendingChromosomes" = true, then increment value after each chromosome
	// This function is to make testing easy.
	virtual void fillConstant(int value, bool acendingChromosomes = false);

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
	virtual void copyFrom(shared_ptr<AbstractGenome> from) override;

// Mutation functions

	virtual int countSites();

	virtual bool isEmpty() override;

	// apply mutations to this genome
	virtual void mutate() override;

	// make a mutated genome. from this genome
	// the undefined action is to return a new genome
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(shared_ptr<AbstractGenome> parent) override;

	// make a mutated genome from a vector or genomes
	// inherit the ParamatersTable from the 0th parent
	// assumes all genomes have the same number of chromosomes and same ploidy
	// if haploid, then all chromosomes are directly crossed (i.e. if there are 4 parents,
	// each parents 0 chromosome is crossed to make a new 0 chromosome, then each parents 1 chromosome...
	// if ploidy > 1 then the number of parents must match ploidy (this may be extended in the future)
	// in this case, each parent crosses all of its chromosomes and contributs the result as a new chromosome
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents) override;

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector
	virtual vector<string> getStats() override;

	virtual void recordDataMap();

	// load all genomes from a file
	virtual void loadGenomes(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) override;
// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
//	virtual void loadGenome(string fileName, string key, string keyValue);

// Translation functions - convert genomes into usefull stuff

	// convert a genome to a string
	virtual string genomeToStr() override;

}
;

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
