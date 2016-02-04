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

		virtual bool inTelomere(int length) {
			return 0;
		}

		////virtual int newHandle() // make a new handle in this genome, return the index for that handle
		////virtual int advanceIndex(int handle, int distance = 1)

		//virtual vector<vector<int>> readTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0);

	};

	AbstractGenome() {
	}

	virtual ~AbstractGenome() = default;

	virtual shared_ptr<Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = 1) {
		return nullptr;		//make_shared<Handler>(_genome);
	}

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
//		bool readDirection;
//		bool EOG;

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
		Handler(shared_ptr<Genome> _genome, bool _readDirection = 1) {
			genome = _genome;
			setReadDirection(_readDirection);
			resetHandler();
		}

		// destructor
		virtual ~Handler() = default;

		// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
		// it uses readDirection to resolve them.
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
		;

		virtual void copyTo(shared_ptr<Handler> to) {
			to->genome = genome;
			to->chromosomeIndex = chromosomeIndex;
			to->siteIndex = siteIndex;
			to->EOG = EOG;
		}

		virtual bool inTelomere(int length) {
			modulateIndex();
			if (readDirection) {  // if reading forward
				return (siteIndex >= (genome->chromosomes[chromosomeIndex]->size() - length));
			} else {
				return (siteIndex - 1 < length);
			}
		}

		//virtual vector<vector<int>> readTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = -1, int gateID = 0);

	};
 public:
	ParametersTable PT;

	vector<shared_ptr<AbstractChromosome>> chromosomes;

	Genome() {
	}

	Genome(shared_ptr<AbstractChromosome> _chromosome) {
		chromosomes.push_back(_chromosome->makeLike());
		chromosomes[0]->fillRandom();  // resize and set with random values
	}

	Genome(shared_ptr<AbstractChromosome> _chromosome, int chromosomeCount) {
		if (chromosomeCount < 1) {
			cout << "Error: Genome must have at least one chromosome";
			exit(1);
		}
		for (int i = 0; i < chromosomeCount; i++) {
			chromosomes.push_back(_chromosome->makeLike());
			chromosomes[i]->fillRandom();  // resize and set with random values
		}
	}

	virtual ~Genome() = default;

	virtual shared_ptr<Handler> newHandler(shared_ptr<Genome> _genome, bool _readDirection = true) {
		return make_shared<Handler>(_genome, _readDirection);
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
		int nucleotides = 0;
		for (auto chromosome : chromosomes){
			nucleotides += chromosome->size();
		}
		// do some point mutations
		int howMany = Random::getBinomial(nucleotides,PT.lookup("pointMutationRate"));
		for (int i = 0; i<howMany; i++){
			chromosomes[Random::getIndex(chromosomes.size())]->mutatePoint();
		}
		//int howMany = Random::getBinomial(nucleotides,PT.lookup("pointMutationRate"));


//		if (pointMutationRate > 0.0) {
//			int nucleotides = (int) sites.size();
//			int i, s, o, w;
//			vector<unsigned char> buffer;
//			int localMutations = Random::getBinomial(nucleotides, pointMutationRate);
//			for (i = 0; i < localMutations; i++) {
//				sites[Random::getIndex(nucleotides)] = Random::getIndex(256);
//			}
//			int numInsertions = Random::getBinomial((int) sites.size(), (insertionRate / 1000));
//			while (numInsertions > 0) {
//				if ((int) nucleotides < maxGenomeSize) {
//					//duplication
//					w = 128 + Random::getIndex(512 - 128);  // w is between 128 and 512 (size of the chunk to be duplicated)
//					if (w >= nucleotides) {  // if w is >= the size of the genome, make w smaller!
//						w = nucleotides - 1;
//					}
//					s = Random::getIndex(nucleotides - w);  // s is where to start copying from.
//					o = Random::getIndex(nucleotides);  // o is where the chunk will be written
//					buffer.clear();
//					buffer.insert(buffer.begin(), sites.begin() + s, sites.begin() + s + w);  // put s to (s+w) in buffer
//					sites.insert(sites.begin() + o, buffer.begin(), buffer.end());  // insert buffer into genome
//				}
//				nucleotides = (int) sites.size();
//				numInsertions--;
//			}
//
//			int numDels = Random::getBinomial(nucleotides, (deletionRate / 1000));
//			while (numDels > 0) {
//
//				if (nucleotides > minGenomeSize) {
//					//deletion
//					w = 128 + Random::getIndex(512 - 128);  //  w is between 128 and 255 (size of the chunk to be deleted)
//					if (w >= nucleotides) {  // if w is >= the size of the genome, make w smaller!
//						w = nucleotides - 1;
//					}
//					s = Random::getIndex(nucleotides - w);  // s is where to start deleting from.
//					sites.erase(sites.begin() + s, sites.begin() + s + w);  // erase everything between s and (s+w)
//				}
//				nucleotides = (int) sites.size();
//				numDels--;
//			}
//		}
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
