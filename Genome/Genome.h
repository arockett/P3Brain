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

#include "Chromosome.h"

#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/ParametersTable.h"
#include "../Utilities/Random.h"

using namespace std;

//// IO and Data Management functions
//
//// gets data about genome which can be added to a data map
//// data is in pairs of strings (key, value)
//// the undefined action is to return an empty vector
//	virtual vector<string> getStats() {
//		vector<string> data;
//		return data;
//	}
//// converts the sites vector to a FileManager::separator separated list in string format.
//// the undefined action is to return an empty string
//	virtual string convert_to_string() {
//		string data;
//		return data;
//	}
//
//// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
//// the undefined action is to take no action
//	virtual void loadGenome(string fileName, string key, string keyValue) {
//	}
//
//// Translation functions - convert genomes into usefull stuff
//
//// return total number of sites in this genome
//	virtual int getSize() {
//		return 0;
//	}
//
//// convert genome sites at location index into a value in range [min,max] and advance index to the next unused site
//// any sites used will be assigned code in codingRegions
//// no undefined action, this function must be defined
//	virtual int extractValue(shared_ptr<Index> index, vector<int> valueRange, int code = -1, int gateID = 0) = 0;
//
//// convert genome sites at location index into a table and advance index to next unused site
//	virtual vector<vector<int>> extractTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0);
//
//// Coding Region functions
//// add to the current coding region
//	virtual inline void assignCodingRegionValue(shared_ptr<Index> index, int code, int gateID) {
//		if (code != -1) {
//			//cout << "assigning: [" << (int) codingRegions.size() - 1 << "][" << index << "] = " << code << "\n";
//			codingRegions[gateID].assign(index, code);
//		}
//	}
//
//// display the getCodingRegions for a gate as (Index1,Type1,Index2,Type2...)
//	virtual inline string showCodingRegions() {
//		string S = "";
//		for (auto codingRegion : codingRegions) {
//			S = S + "gate: " + to_string(codingRegion.first) + " " + codingRegion.second.showCodingRegion();
//		}
//		S += "\n";
//		return S;
//	}

class AbstractGenome {

 public:

	// Handlers are how you access Genomes for reading and writting.
	// to get a handle for a genome call that that genomes newHandler() method
	class Handler {
 	public:
		//shared_ptr<AbstractGenome> genome;
		int readDirection;  // true = forward, false = backwards
		bool EOG;  // end of genome

		Handler() {
			readDirection = true;
			EOG = true;
		}

		Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection = 1) {
			readDirection = _readDirection;
			EOG = true;
		}

		virtual void resetEOG() {
			EOG = false;
		}

		virtual void setReadDirection(bool _readDirection) {
			readDirection = _readDirection;
		}

		virtual void resetHandler() {
			if (readDirection) {  // if reading forward
			} else {  // if reading backwards
			}
			resetEOG();
		}

		virtual ~Handler() {
		}

		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
			return 0;
		}

		virtual void writeInt(int value, int valueMin, int valueMax) {
		}

		virtual vector<vector<int>> readTable(vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int CodingRegionIndex = 0) {
			vector<vector<int>> temp;
			return temp;
		}

		virtual void copyTo(shared_ptr<Handler> to) {
		}

		virtual bool atEOG() {
			return false;
		}

		virtual void printIndex() = 0;

		virtual bool inTelomere(int length) {
			return 0;
		}

		////virtual int newHandle() // make a new handle in this genome, return the index for that handle
		////virtual int advanceIndex(int handle, int distance = 1)

		//virtual vector<vector<int>> readTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0);

	};

	DataMap dataMap;

	AbstractGenome() {
	}

	virtual ~AbstractGenome() = default;
	//virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override {

	virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) {
		cout << "This may be the problem" << endl;
		return nullptr;		//make_shared<Handler>(_genome);
	}

	virtual void copyFrom(shared_ptr<AbstractGenome> from) = 0;

	virtual void fillRandom() = 0;		//{
//		cout << "In AbstractGenome::fillRandom()...\n";
//	}

	virtual void mutate(int length) {

	}

	virtual vector<string> getStats() {
		vector<string> data;
		cout << "In AbstractGenome::getStats()...\n";
		return data;
	}

	virtual string genomeToStr() {
		cout << "In AbstractGenome::genomeToStr()...\n";
		return "";
	}

	virtual void loadGenome(string fileName, string key, string keyValue) {
	}

	virtual bool isEmpty() = 0;

	virtual shared_ptr<AbstractGenome> makeMutatedGenome(shared_ptr<AbstractGenome> parent) = 0;
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents) = 0;

};

class Genome : public AbstractGenome {
 public:

	static int& initialChromosomeSize;
	static double& pointMutationRate;
	static double& insertionRate;
	static int& insertionMinSize;
	static int& insertionMaxSize;
	static double& deletionRate;
	static int& deletionMinSize;
	static int& deletionMaxSize;
	static int& minGenomeSize;
	static int& maxGenomeSize;
	static int& maxChromosomeSize;
	static int& minChromosomeSize;
	static int& crossCount;  // number of crosses to make when performing crossover

	class Handler : public AbstractGenome::Handler {
 	public:
		shared_ptr<Genome> genome;
		int siteIndex;
		int chromosomeIndex;

		virtual void resetHandler() {
			if (readDirection) {  // if reading forward
				chromosomeIndex = 0;
				siteIndex = 0;
			} else {  // if reading backwards
				chromosomeIndex = ((int) genome->chromosomes.size()) - 1;  // set to last chromosome
				siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;  // set to last site in last chromosome
			}
			resetEOG();
		}

		// constructor
		Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection = 1) {
			genome = dynamic_pointer_cast<Genome>(_genome);
			setReadDirection(_readDirection);
			resetHandler();
		}

		// destructor
		virtual ~Handler() = default;

		// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
		// it uses readDirection to resolve them.	virtual void copyFrom(shared_ptr<Genome> from) {

		//  modulate index truncates nonexistant sites. i.e. if the current addres is chromosome 1, site 10 and
		// chromosome 10 is 8 long, modulateIndex will set the index to chromosome 2, site 0 (not site 2).
		// If this behavior is required, use advance Index instead.
		// If the chromosomeIndex has past the last chromosome (or the first
		// if read direction = -1) then EOG (end of genome) is set true.
		virtual void modulateIndex() {
			if (readDirection) {
				// first see if we are past last chromosome
				if (chromosomeIndex >= (int) genome->chromosomes.size()) {
					chromosomeIndex = 0;  // reset chromosomeIndex
					EOG = true;  // if we are past the last chromosome then EOG = true
				}

				// now that we know that the chromosome index is in range, check the site index
				if (genome->chromosomes[chromosomeIndex]->modulateIndex(siteIndex)) {
					chromosomeIndex++;	// if the site index is out of range, increment the chromosomeIndex
					siteIndex = 0;		// ... and reset the site index
					// now that we know that the site index is also in range, we have to check the chromosome index again!
					if (chromosomeIndex >= (int) genome->chromosomes.size()) {
						chromosomeIndex = 0;	// if the site index is out of range, increment the chromosomeIndex
						siteIndex = 0;			// ... and reset the site index
						EOG = true;
					};
				}
			} else {  //reading backwards!
				// first see if we are past last chromosome
				if (chromosomeIndex < 0) {
					chromosomeIndex = ((int) genome->chromosomes.size()) - 1;  // reset chromosomeIndex (to last chromosome)
					siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;  // reset siteIndex (to last site in this chromosome)
					EOG = true;  // if we are past the last chromosome then EOG = true
				}

				// now that we know that the chromosome index is in range, check the site index
				if (genome->chromosomes[chromosomeIndex]->modulateIndex(siteIndex)) {
					chromosomeIndex--;	// if the site index is out of range, increment the chromosomeIndex
					// now that we know that the site index is also in range, we have to check the chromosome index again!
					if (chromosomeIndex < 0) {
						chromosomeIndex = ((int) genome->chromosomes.size()) - 1;	// if the site index is out of range, decrement the chromosomeIndex
						EOG = true;
					}
					siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;	// reset siteIndex (to last site in this chromosome)
				}

			}
		}

		// advanceIndex will move the index forward distance sites.
		// if there are too few sites, it will advance to the next chromosome and then advance addtional sites (if needed)
		// NOTE: if the advance is > the current chromosome size, it will be modded to the chromosome size.
		// i.e. if the chromosome was length 10, and the current siteIndex = 0, advanceIndex(15) will advance to
		// site 5 of the next chromosome. Should this be fixed?!??
		virtual void advanceIndex(int distance = 1) {
			modulateIndex();
			if (readDirection) {	// reading forward
				if ((genome->chromosomes[chromosomeIndex]->size() - siteIndex) > distance) {
					siteIndex += distance;	// if there are enough sites left in the current chromosome, just move siteIndex
				} else {	// there are not enough sites in the current chromosome, must move to next chromosome
					distance = distance - (genome->chromosomes[chromosomeIndex]->size() - siteIndex);
					// reduce distance by the number of sites between siteIndex and end of Chromosome
					advanceChromosome();	// advance to the next chromosome;
					advanceIndex(distance);  // advanceIndex by remaining distance
				}

			} else { 				// reading backwards

				if (siteIndex > distance) {
					siteIndex -= distance;	// if there are enough sites in the current chromosome (between siteIndex and start of chromosome) just move siteIndex
				} else {	// there are not enough sites in the current chromosome, must move to next chromosome
					distance = distance - siteIndex;	// reduce distance by the number of sites between siteIndex and start of Chromosome
					advanceChromosome();	// advance to the next chromosome;
					advanceIndex(distance);  // advanceIndex by remaining distance
				}

			}

		}

		// returns true if this Handler has reached the end of genome (or start if direction is backwards).
		virtual bool atEOG() {
			modulateIndex();
			return EOG;
		}

		virtual void advanceChromosome() {
			chromosomeIndex += (readDirection) ? 1 : (-1);  //move index
			siteIndex = 0;	// set siteIndex to 0 so modulateIndex can not advance again
			                // if we are reading forward, siteIndex should = 0 at this time
			modulateIndex();
			if (!readDirection) {  // if we are reading backwards, set siteIndex to the last site
				siteIndex = genome->chromosomes[chromosomeIndex]->size() - 1;
			}
		}

		virtual void printIndex() {
			cout << "chromosomeIndex: " << chromosomeIndex << "  siteIndex: " << siteIndex << "  EOG: " << EOG << "\n";
		}

		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
			////cout << "readInt() " << valueMin << " " << valueMax << "\n";
			int value;
			modulateIndex();
			if (genome->chromosomes[chromosomeIndex]->readInt(siteIndex, value, valueMin, valueMax, readDirection, code, CodingRegionIndex)) {
				advanceChromosome();
			}
			return value;
		}

		virtual void writeInt(int value, int valueMin, int valueMax) {
			modulateIndex();
			if (genome->chromosomes[chromosomeIndex]->writeInt(siteIndex, value, valueMin, valueMax, readDirection)) {
				advanceChromosome();
			}
		}

		virtual void copyTo(shared_ptr<AbstractGenome::Handler> to) {
			auto castTo = dynamic_pointer_cast<Genome::Handler>(to);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
			castTo->genome = genome;
			castTo->chromosomeIndex = chromosomeIndex;
			castTo->siteIndex = siteIndex;
			castTo->EOG = EOG;
		}

		virtual bool inTelomere(int length) {
			modulateIndex();
			if (readDirection) {  // if reading forward
				return (siteIndex >= (genome->chromosomes[chromosomeIndex]->size() - length));
			} else {
				return (siteIndex - 1 < length);
			}
		}

		virtual vector<vector<int>> readTable(vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int CodingRegionIndex = 0) {
			vector<vector<int>> table;
			int x = 0;
			int y = 0;
			int Y = tableSize[0];
			int X = tableSize[1];
			int maxY = tableMaxSize[0];
			int maxX = tableMaxSize[1];

			table.resize(Y);  // set the number of rows in the table

			for (; y < (Y); y++) {
				table[y].resize(X);  // set the number of columns in this row
				for (x = 0; x < X; x++) {
					//table[y][x] = (Type) (sites[index]);
					table[y][x] = readInt(valueRange[0], valueRange[1], code, CodingRegionIndex);
				}
				for (; x < maxX; x++) {
					readInt(valueRange[0], valueRange[1]);  // advance genomeIndex to account for unused entries in the max sized table for this row
				}
			}
			for (; y < (maxY); y++) {
				for (x = 0; x < maxX; x++) {
					readInt(valueRange[0], valueRange[1]);  // advance to account for unused rows
				}
			}
			return table;
		}

	};
 public:
	ParametersTable PT;
	int ploidy;
	vector<shared_ptr<AbstractChromosome>> chromosomes;

	Genome() {
		ploidy = 1;
	}

	Genome(shared_ptr<AbstractChromosome> _chromosome) {
		ploidy = 1;
		chromosomes.push_back(_chromosome->makeLike());
		/////////chromosomes[0]->fillRandom();  // resize and set with random values
		recordDataMap();
	}

	Genome(shared_ptr<AbstractChromosome> _chromosome, int chromosomeCount, int _plodiy = 1) {
		if (_plodiy < 1) {
			cout << "Error: Genome must have plodiy >= 1";
			exit(1);
		}
		ploidy = _plodiy;
		if (chromosomeCount < 1) {
			cout << "Error: Genome must have at least one chromosome";
			exit(1);
		}
		for (int i = 0; i < (chromosomeCount * _plodiy); i++) {
			chromosomes.push_back(_chromosome->makeLike());
			/////////chromosomes[i]->fillRandom();  // resize and set with random values
		}
		recordDataMap();
	}

	virtual ~Genome() = default;

	virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override {
		////////////////////////////////////cout << "In Genome::newHandler()" << endl;
		return make_shared<Handler>(_genome, _readDirection);
	}

	// randomize this genomes contents
	virtual void fillRandom() {
		for (auto chromosome : chromosomes) {
			chromosome->fillRandom();
		}
	}

	// fill all sites of this genome with ascending values
	// This function is to make testing easy.
	virtual void fillAcending() {
		int start = 0;
		for (int i = 0; i < (int) chromosomes.size(); i++) {
			chromosomes[i]->fillAcending(start);
		}
	}

	// fill all sites of this genome with value
	// if "acendingChromosomes" = true, then increment value after each chromosome
	// This function is to make testing easy.
	virtual void fillConstant(int value, bool acendingChromosomes = false) {
		for (int i = 0; i < (int) chromosomes.size(); i++) {
			chromosomes[i]->fillConstant(value);
			if (acendingChromosomes) {
				value++;
			}
		}
	}

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
	virtual void copyFrom(shared_ptr<AbstractGenome> from) {
		auto castFrom = dynamic_pointer_cast<Genome>(from);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
		chromosomes.resize(0);
		for (auto chromosome : castFrom->chromosomes) {
			chromosomes.push_back(chromosome->makeCopy());
		}
		PT.copy(castFrom->PT);
		ploidy = castFrom->ploidy;
	}

// Mutation functions

	virtual int countSites() {
		int nucleotides = 0;
		for (auto chromosome : chromosomes) {
			nucleotides += chromosome->size();
		}
		return nucleotides;
	}

	virtual bool isEmpty() {
		//cout << "in Genome::isEmpty(): " << to_string(countSites() == 0) << " : " << to_string(countSites()) << endl;
		return (countSites() == 0);
	}

// old version gets numbers for number of events from entire genome sites count
//// apply mutations to this genome
//	virtual void mutate() {
//		int nucleotides = countSites();
//		int howManyPoint = Random::getBinomial(nucleotides, PT.lookup("pointMutationRate"));
//		int howManyCopy = Random::getBinomial(nucleotides, PT.lookup("mutationCopyRate"));
//		int howManyDelete = Random::getBinomial(nucleotides, PT.lookup("mutationDeletionRate"));
//		// do some point mutations
//		for (int i = 0; i < howManyPoint; i++) {
//			chromosomes[Random::getIndex(chromosomes.size())]->mutatePoint();
//		}
//		// do some copy mutations
//		if (nucleotides < PT.lookup("genomeSizeMax")) {
//			for (int i = 0; i < howManyCopy && (nucleotides < PT.lookup("genomeSizeMax")); i++) {
//				chromosomes[Random::getIndex(chromosomes.size())]->mutateCopy(PT.lookup("mutationCopyMinSize"), PT.lookup("mutationCopyMaxSize"), PT.lookup("chromosomeSizeMax"));
//				nucleotides = countSites();
//			}
//		}
//		// do some deletion mutations
//		if (nucleotides > PT.lookup("genomeSizeMin")) {
//			for (int i = 0; i < howManyDelete && (nucleotides > PT.lookup("genomeSizeMin")); i++) {
//				chromosomes[Random::getIndex(chromosomes.size())]->mutateDelete(PT.lookup("mutationDeletionMinSize"), PT.lookup("mutationDeletionMaxSize"), PT.lookup("chromosomeSizeMin"));
//				nucleotides = countSites();
//			}
//		}
//	}

	// new version gets numbers for number of events from sites count in each chromosome
	// apply mutations to this genome
	virtual void mutate() {
		for (auto chromosome : chromosomes) {
			int nucleotides = chromosome->size();
			int howManyPoint = Random::getBinomial(nucleotides, PT.lookup("pointMutationRate"));
			int howManyCopy = Random::getBinomial(nucleotides, PT.lookup("mutationCopyRate"));
			int howManyDelete = Random::getBinomial(nucleotides, PT.lookup("mutationDeletionRate"));
			// do some point mutations
			for (int i = 0; i < howManyPoint; i++) {
				chromosome->mutatePoint();
			}
			// do some copy mutations
			if (nucleotides < PT.lookup("genomeSizeMax")) {
				for (int i = 0; i < howManyCopy && (nucleotides < PT.lookup("genomeSizeMax")); i++) {
					chromosome->mutateCopy(PT.lookup("mutationCopyMinSize"), PT.lookup("mutationCopyMaxSize"), PT.lookup("chromosomeSizeMax"));
					nucleotides = chromosome->size();
				}
			}
			// do some deletion mutations
			if (nucleotides > PT.lookup("genomeSizeMin")) {
				for (int i = 0; i < howManyDelete && (nucleotides > PT.lookup("genomeSizeMin")); i++) {
					chromosome->mutateDelete(PT.lookup("mutationDeletionMinSize"), PT.lookup("mutationDeletionMaxSize"), PT.lookup("chromosomeSizeMin"));
					nucleotides = chromosome->size();
				}
			}
		}
	}

	// make a mutated genome. from this genome
	// the undefined action is to return a new genome
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(shared_ptr<AbstractGenome> parent) {
		auto newGenome = make_shared<Genome>();
		newGenome->copyFrom(parent);
		newGenome->mutate();
		newGenome->recordDataMap();
		return newGenome;
	}

	// make a mutated genome from a vector or genomes
	// inherit the ParamatersTable from the 0th parent
	// assumes all genomes have the same number of chromosomes and same ploidy
	// if haploid, then all chromosomes are directly crossed (i.e. if there are 4 parents,
	// each parents 0 chromosome is crossed to make a new 0 chromosome, then each parents 1 chromosome...
	// if ploidy > 1 then the number of parents must match ploidy (this may be extended in the future)
	// in this case, each parent crosses all of its chromosomes and contributs the result as a new chromosome
	virtual shared_ptr<AbstractGenome> makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents) {
		// first, check to make sure that parent genomes are conpatable.
		auto castParent0 = dynamic_pointer_cast<Genome>(parents[0]);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
		int testPloidy = castParent0->ploidy;
		size_t testChromosomeCount = castParent0->chromosomes.size();
		for (auto rawParent : parents) {
			auto parent = dynamic_pointer_cast<Genome>(rawParent);
			if (parent->ploidy != testPloidy) {
				cout << "ERROR! In Genome::makeMutatedGenome(vector<shared_ptr<Genome>> parents). Parents are incompatible due to mismatched ploidy!\nExiting!\n";
				exit(1);
			}
			if (parent->chromosomes.size() != testChromosomeCount) {
				cout << "ERROR! In Genome::makeMutatedGenome(vector<shared_ptr<Genome>> parents). Parents are incompatible due to mismatched number of Chromosomes!\nExiting!\n";
				exit(1);
			}

		}
		auto newGenome = make_shared<Genome>();
		newGenome->PT.copy(castParent0->PT);  // copy ParametersTable from 0th parent
		newGenome->ploidy = castParent0->ploidy;  // copy ploidy from 0th parent

		if (ploidy == 1) {  // if haploid then cross chromosomes from all parents
			for (size_t i = 0; i < castParent0->chromosomes.size(); i++) {
				newGenome->chromosomes.push_back(castParent0->chromosomes[0]->makeLike());
				vector<shared_ptr<AbstractChromosome>> parentChromosomes;
				for (auto rawParent : parents) {
					auto parent = dynamic_pointer_cast<Genome>(rawParent);
					parentChromosomes.push_back(parent->chromosomes[i]);  // make a vector that contains the nth chromosome from each parent
				}
				newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, newGenome->PT.lookup("genomecrossCount"));  // create a crossover chromosome
			}
		} else if (ploidy == (int) parents.size()) {  // if diploid than cross chromosomes from all parents
			int setCount = castParent0->chromosomes.size() / ploidy;  // number of sets of chromosomes
			for (int currSet = 0; currSet < setCount; currSet++) {
				int parentCount = 0;
				for (auto parent : parents) {

					newGenome->chromosomes.push_back(castParent0->chromosomes[0]->makeLike());  // add a chromosome to this genome
					vector<shared_ptr<AbstractChromosome>> parentChromosomes;
					for (int pCount = 0; pCount < ploidy; pCount++) {  // make a vector containing all the chromosomes in this chromosome set from this parent
						parentChromosomes.push_back(dynamic_pointer_cast<Genome>(parent)->chromosomes[(currSet * ploidy) + pCount]);
					}
					newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, newGenome->PT.lookup("genomecrossCount"));
					parentCount++;
				}
			}
		} else {  // ploidy does not match number of parents...
			cout << "In Genome::makeMutatedGenome(). ploidy > 1 and ploidy != number of parents... this is not supported (yet)!\nWhat does it even mean?\nExiting!";
			exit(1);
		}

		newGenome->mutate();
		newGenome->recordDataMap();
		return newGenome;
	}

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector
	virtual vector<string> getStats() {
		vector<string> dataPairs;
		dataPairs.push_back("genomeLength");
		dataPairs.push_back(to_string(countSites()));
		return (dataPairs);
	}

	virtual void recordDataMap() {
		dataMap.SetMany(chromosomes[0]->getFixedStats());
		dataMap.Set("ploidy", ploidy);
		dataMap.Set("chromosomeCount", chromosomes.size());
		dataMap.Set("sitesCount", countSites());
		dataMap.Clear("chromosomeLengths");
		for (size_t c = 0; c < chromosomes.size(); c++) {
			dataMap.Append("chromosomeLengths", chromosomes[c]->size());
		}
	}

// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
	virtual void loadGenome(string fileName, string key, string keyValue) {
	}

// Translation functions - convert genomes into usefull stuff

	// convert a chromosome to a string
	virtual string genomeToStr() {
		string S = "";

		for (size_t c = 0; c < chromosomes.size(); c++) {
			S = S + FileManager::separator + chromosomes[c]->chromosomeToStr();
		}
		S.erase(S.begin());  // clip off the leading separator
		 S = "\"[" + S + "]\"";
		return S;
	}

}
;

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
