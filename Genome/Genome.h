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

#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"

using namespace std;

class Genome {
 public:

	////////////// class Index

	class Index {
 	private:
		int site;  // index->site is what this index is looking at
		int genomeSize;  // size of genome

 	public:
		bool eog;  // set to true when end of genome is reached

		// make a blank index
		Index() {
			site = 0;
			genomeSize = 0;
			eog = false;
		}

		// make a new index pointing at site (first site by default);
		Index(int _genomeSize) {
			site = 0;
			genomeSize = _genomeSize;
			eog = false;
		}

		// advance index by some number of sites.
		// if end of genome is reached, set eog = true and loop.
		virtual void advance(int by = 1) {
			site = site + by;
			if (site >= genomeSize) {
				eog = true;
				site = site % genomeSize;
			}
		}

		virtual vector<int> current() {
			vector<int> currIndex;
			currIndex.push_back(site);
			//currIndex.push_back(chromosome);
			return currIndex;
		}

		// did we get to (or pass) the end of the genome?
		virtual bool atEnd() {
			return eog;
		}

		virtual void resetEOG() {
			eog = false;
		}

		virtual void setSite(int _site = 0) {
			site = _site;
		}

		virtual void randomize(vector<int> safe = { 0, 0 }) {
			site = Random::getInt(safe[0], genomeSize - safe[1] - 1);
		}

		virtual void copy(shared_ptr<Index> to) {
			to->site = site;
			to->genomeSize = genomeSize;
			to->eog = eog;
		}

		virtual Index mkCopy() {
			Index to;
			to.site = site;
			to.genomeSize = genomeSize;
			to.eog = eog;
			return to;
		}

		virtual inline string showIndex() {
			return to_string(site);
		}

	};
	////////////// end of class Index

	shared_ptr<Index> getIndex() {
		return make_shared<Index>(getSize());
	}

	class CodingRegion {
 	private:
		vector<pair<Index, int>> codingRegion;

 	public:

		virtual void assign(shared_ptr<Index> index, int code) {
			codingRegion.push_back( { index->mkCopy(), code });
		}

		virtual inline string showCodingRegion() {
			string S = "";
			for (auto site : codingRegion) {
				S = S + site.first.showIndex() + ":" + to_string(site.second) + "  ";
			}
			S += "\n";
			return S;
		}

		static const int START_CODE = 0;
		static const int IN_COUNT_CODE = 10;
		static const int IN_ADDRESS_CODE = 11;
		static const int OUT_COUNT_CODE = 20;
		static const int OUT_ADDRESS_CODE = 21;
		static const int DATA_CODE = 30;
	};

	map<int, CodingRegion> codingRegions;

	Genome() = default;
	virtual ~Genome() = default;

// randomize this genomes contents
// the undefined action is to take no action
	virtual void fillRandom() {
	}

// assign a value at a given location
// the undefined action is to take no action
	virtual void assignValue(Index index, int value) {
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
		vector<string> data;
		return data;
	}
// converts the sites vector to a FileManager::separator separated list in string format.
// the undefined action is to return an empty string
	virtual string convert_to_string() {
		string data;
		return data;
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

// convert genome sites at location index into a value in range [min,max] and advance index to the next unused site
// any sites used will be assigned code in codingRegions
// no undefined action, this function must be defined
	virtual int extractValue(shared_ptr<Index> index, vector<int> valueRange, int code = -1, int gateID = 0) = 0;

// convert genome sites at location index into a table and advance index to next unused site
	virtual vector<vector<int>> extractTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = CodingRegion::DATA_CODE, int gateID = 0);

// Coding Region functions
// add to the current coding region
	virtual inline void assignCodingRegionValue(shared_ptr<Index> index, int code, int gateID) {
		if (code != -1) {
			//cout << "assigning: [" << (int) codingRegions.size() - 1 << "][" << index << "] = " << code << "\n";
			codingRegions[gateID].assign(index, code);
		}
	}

// display the getCodingRegions for a gate as (Index1,Type1,Index2,Type2...)
	virtual inline string showCodingRegions() {
		string S = "";
		for (auto codingRegion : codingRegions) {
			S = S + "gate: " + to_string(codingRegion.first) + " " + codingRegion.second.showCodingRegion();
		}
		S += "\n";
		return S;
	}

}
;

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
