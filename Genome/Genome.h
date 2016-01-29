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

//class Genome {
// public:

////////////// class Index

//	class Index {
// 	private:
//		int site;  // index->site is what this index is looking at
//
// 	public:
//		bool eog;  // set to true when end of genome is reached
//
//		// make a blank index
//		Index() {
//			site = 0;
//			eog = false;
//		}
//
//		// make a new index pointing at site (first site by default);
//		Index(int _genomeSize) {
//			site = 0;
//			eog = false;
//		}
//
//		// advance index by some number of sites.
//		// if end of genome is reached (new index > old index), set eog = true.
//		virtual void advanceIndex(int by = 1) {
//			int oldSite = site;
//			chromosome.advanceIndex(site,by);
//			if (site <= oldSite) {
//				eog = true;
//			}
//		}
//
//		virtual vector<int> current() {
//			vector<int> currIndex;
//			currIndex.push_back(site);
//			//currIndex.push_back(chromosome);
//			return currIndex;
//		}
//
//		// did we get to (or pass) the end of the genome?
//		virtual bool atEnd() {
//			return eog;
//		}
//
//		virtual void resetEOG() {
//			eog = false;
//		}
//
//		virtual void setSite(int _site = 0) {
//			site = _site;
//		}
//
//		virtual void randomize(vector<int> safe = { 0, 0 }) {
//			site = Random::getInt(safe[0], genomeSize - safe[1] - 1);
//		}
//
//		virtual void copyTo(shared_ptr<Index> to) {
//			to->site = site;
//			to->genomeSize = genomeSize;
//			to->eog = eog;
//		}
//
//		virtual Index mkCopy() {
//			Index to;
//			to.site = site;
//			to.genomeSize = genomeSize;
//			to.eog = eog;
//			return to;
//		}
//
//		virtual inline string showIndex() {
//			return to_string(site);
//		}
//
//	};
//	////////////// end of class Index
//
//	shared_ptr<Index> getIndex() {
//		return make_shared<Index>(getSize());
//	}
//
//	class CodingRegion {
// 	private:
//		vector<pair<Index, int>> codingRegion;
//
// 	public:
//
//		virtual void assign(shared_ptr<Index> index, int code) {
//			codingRegion.push_back( { index->mkCopy(), code });
//		}
//
//		virtual inline string showCodingRegion() {
//			string S = "";
//			for (auto site : codingRegion) {
//				S = S + site.first.showIndex() + ":" + to_string(site.second) + "  ";
//			}
//			S += "\n";
//			return S;
//		}
//	};
//
//	map<int, CodingRegion> codingRegions;
//
//	Genome() = default;
//	virtual ~Genome() = default;
//
//// randomize this genomes contents
//// the undefined action is to take no action
//	virtual void fillRandom() {
//	}
//
//	virtual void fillRandom(int sites) {
//	}
//
//// assign a value at a given location
//// the undefined action is to take no action
//	virtual void assignValue(Index index, int value) {
//	}
//
//// Copy functions
//
//// copy the contents of another genome to this genome
//// no undefined action, this function must be defined
//	virtual void copyGenome(shared_ptr<Genome> from) {
//	}
//
//// make a mutated genome. from this genome
//// the undefined action is to return a new genome
//	virtual shared_ptr<Genome> makeMutatedGenome() {
//		shared_ptr<Genome> genome;
//		return genome;
//	}
//
//// make a mutated genome. from a vector or genomes
//// the undefined action is to return a new genome
//	virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<Genome>> from) {
//		shared_ptr<Genome> genome;
//		return genome;
//	}
//
//// Mutation functions
//
//// apply mutations to this genome
//// the undefined action is to take no action
//	virtual void mutate() {
//	}
//
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

	class Handler {
 	public:
		shared_ptr<AbstractGenome> genome;

		Handler() = default;

		Handler(shared_ptr<AbstractGenome> _genome) {
			genome = _genome;
		}

		virtual ~Handler() {
		}

		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
			return false;
		}

		virtual void writeInt(int value, int valueMin, int valueMax) {
		}

		virtual void copyTo(shared_ptr<Handler> to) {
		}

		virtual bool atEOG() {
			return false;
		}

		void printIndex() {
		}

		////virtual int newHandle() // make a new handle in this genome, return the index for that handle
		////virtual int advanceIndex(int handle, int distance = 1)

		//virtual vector<vector<int>> readTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0);

	};

	AbstractGenome() {
	}

	virtual ~AbstractGenome() = default;

	virtual shared_ptr<Handler> newHandler(shared_ptr<AbstractGenome> _genome) {
		return nullptr;		//make_shared<Handler>(_genome);
	}

	virtual void fillRandom() = 0;		//{
//		cout << "In AbstractGenome::fillRandom()...\n";
//	}

	virtual vector<string> getStats() {
		vector<string> data;
		cout << "In AbstractGenome::getStats()...\n";
		return data;
	}

	virtual string genomeToStr() {
		cout << "In AbstractGenome::genomeToStr()...\n";
		return "";
	}

//	virtual vector<vector<int>> readTable(vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0){
//		vector<vector<int>> tempTable;
//		return tempTable;
//	}

	//virtual void loadGenome(string fileName, string key, string keyValue) {

	//virtual void mutate()
	//virtual void fillRandom()
	//virtual shared_ptr<Genome> makeMutatedGenome()
	//virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<Genome>> from)

	//virtual vector<string> getStats() {
	//virtual string convert_to_string() {

};

class Genome : public AbstractGenome {
 public:

	class Handler : public AbstractGenome::Handler {
 	public:
		shared_ptr<Genome> genome;
		int siteIndex;
		int chromosomeIndex;

		bool EOG;

		// constructor
		Handler(shared_ptr<Genome> _genome) {
			genome = _genome;
			siteIndex = 0;
			chromosomeIndex = 0;
			EOG = false;
		}

		// destructor
		virtual ~Handler() = default;

		virtual void modulateIndex() {
			// first see if we are past last chromosome
			if (chromosomeIndex >= (int) genome->chromosomes.size()) {
				chromosomeIndex = 0;  // reset chromosomeIndex
				EOG = true;  // if we are past the last chromosome then EOG = true
			}

			// now that we know that the chromosome index is in range, check the site index
			if (genome->chromosomes[chromosomeIndex]->modulateIndex(siteIndex)) {
				chromosomeIndex++;
				siteIndex = 0;
			}
			// now that we know that the site index is also in range, we have to check the chromosome index again!
			if (chromosomeIndex >= (int) genome->chromosomes.size()) {
				chromosomeIndex = 0;  // reset chromosomeIndex
				siteIndex = 0;
				EOG = true;
			};
		}

		virtual void advanceChromosome() {
			chromosomeIndex++;
			siteIndex = 0;
			modulateIndex();
		}
		virtual void printIndex() {
			cout << "chromosomeIndex: " << chromosomeIndex << "  siteIndex: " << siteIndex << "  EOG: " << EOG << "\n";
		}

		virtual int readInt(int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
			int value;
			modulateIndex();
			if (genome->chromosomes[chromosomeIndex]->readInt(siteIndex, value, valueMin, valueMax, code, CodingRegionIndex)) {
				advanceChromosome();
			}
			return value;
		}

		virtual void writeInt(int value, int valueMin, int valueMax) {
		}
		;

		////virtual int newHandle() // make a new handle in this genome, return the index for that handle
		virtual bool advanceIndex(int distance = 1) {
			siteIndex = siteIndex + distance;
			modulateIndex();  // return EOG = false
			return EOG;
		}

		virtual bool atEOG() {
			modulateIndex();
			return EOG;
		}

		virtual void copyTo(shared_ptr<Handler> to) {
			to->genome = genome;
			to->chromosomeIndex = chromosomeIndex;
			to->siteIndex = siteIndex;
			to->EOG = EOG;
		}

		//virtual vector<vector<int>> readTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0);

	};
 public:
	vector<shared_ptr<AbstractChromosome>> chromosomes;

	Genome() {
	}

	Genome(shared_ptr<AbstractChromosome> _chromosome) {
		chromosomes.resize(2);
		chromosomes[0] = _chromosome->makeCopy();
		chromosomes[0]->resize(10);
		chromosomes[1] = _chromosome->makeCopy();
		chromosomes[1]->resize(5);
	}

	Genome(shared_ptr<AbstractChromosome> _chromosome, int chromosomeCount, int initalChromosomeSize) {
		if (chromosomeCount < 1) {
			cout << "Error: Genome must have atleast one chromosome";
			exit(1);
		}
		for (int i = 0; i < chromosomeCount; i++) {
			chromosomes.push_back(_chromosome->makeCopy());
			chromosomes[i]->fillRandom(initalChromosomeSize); // resize and set with random values
		}
	}

	virtual ~Genome() = default;

	virtual shared_ptr<Handler> newHandler(shared_ptr<Genome> _genome) {
		return make_shared<Handler>(_genome);
	}

// randomize this genomes contents
// the undefined action is to take no action
	virtual void fillRandom() {
		for (auto chromosome : chromosomes) {
			chromosome->fillRandom();
		}
	}

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
	virtual void copyGenome(shared_ptr<Genome> from) {
	}

// make a mutated genome. from this genome
// the undefined action is to return a new genome
	virtual shared_ptr<Genome> makeMutatedGenome() {
		shared_ptr<Genome> genome;
		return genome;
	}

// make a mutated genome. from a vector or genomes
// the undefined action is to return a new genome
	virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<Genome>> from) {
		shared_ptr<Genome> genome;
		return genome;
	}

// Mutation functions

// apply mutations to this genome
// the undefined action is to take no action
	virtual void mutate() {
	}

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector
	virtual vector<string> getStats() {
		vector<string> dataPairs;
		dataPairs.push_back("ChromosomeCount");
		dataPairs.push_back(to_string(chromosomes.size()));
		return (dataPairs);
	}

// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
	virtual void loadGenome(string fileName, string key, string keyValue) {
	}

// Translation functions - convert genomes into usefull stuff

// return total number of sites in this genome
	virtual int getSize() {
		return 0;
	}

	// convert a chromosome to a string
	virtual string genomeToStr() {
		string S = "";
		for (auto chromosome : chromosomes) {
			S = S + chromosome->chromosomeToStr();
		}
		S = S + "\n";
		return S;
	}

}
;

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
