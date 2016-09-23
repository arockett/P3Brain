//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Genome__
#define __BasicMarkovBrainTemplate__Genome__

#include <stdlib.h>
#include <vector>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"

using namespace std;

class AbstractGenome {

public:

	virtual shared_ptr<AbstractGenome> makeCopy(shared_ptr<ParametersTable> _PT = nullptr);

	static shared_ptr<ParameterLink<string>> genomeTypeStrPL;
	static shared_ptr<ParameterLink<double>> alphabetSizePL;
	static shared_ptr<ParameterLink<string>> genomeSitesTypePL;

	const shared_ptr<ParametersTable> PT;

	// Handlers are how you access Genomes for reading and writting.
	// to get a handle for a genome call that that genomes newHandler() method
	class Handler {
	public:
		//shared_ptr<AbstractGenome> genome;
		bool readDirection;  // true = forward, false = backwards
		bool EOG;  // end of genome
		bool EOC;  // end of chromosome - in this context chromosome is a subsection of the genome
				   // which after having passed we may want to perform some different behavior

		Handler() {
			readDirection = true;
			EOG = false;
			EOC = false;
		}

		Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) {
			readDirection = _readDirection;
			EOG = false;
			EOC = false;
		}

		virtual void resetEOG() {
			EOG = false;
			EOC = false;
		}

		virtual void resetEOC() {
			EOC = false;
		}

		virtual void setReadDirection(bool _readDirection) {
			readDirection = _readDirection;
		}

		virtual void toggleReadDirection() {
			readDirection = !readDirection;
		}

		virtual void resetHandler() = 0;
		virtual void resetHandlerOnChromosome() = 0;

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
		virtual vector<vector<int>> readTable(pair<int, int> tableSize, pair<int, int> tableMaxSize, pair<int, int> valueRange, int code = -1, int CodingRegionIndex = 0)=0;
		virtual void advanceIndex(int distance = 1) = 0;

		virtual void copyTo(shared_ptr<Handler> to) = 0;

		virtual bool atEOG() {
			return false;
		}

		virtual bool atEOC() {
			return false;
		}

		virtual void printIndex() = 0;

		virtual bool inTelomere(int length) {
			return false;
		}

		virtual void randomize() = 0;
	};

	DataMap dataMap;
	vector<string> genomeFileColumns;  // = {"ID","alphabetSize","chromosomeCount","chromosomeLength","sitesCount","genomeAncestors","sites"};
	vector<string> aveFileColumns;  // = {"genomeLength"};

	AbstractGenome() = delete;
	AbstractGenome(shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT) {}

	virtual ~AbstractGenome() = default;
	//virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) override {

	virtual shared_ptr<AbstractGenome> makeLike() = 0;

	virtual shared_ptr<AbstractGenome::Handler> newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection = true) = 0;
	virtual double getAlphabetSize() = 0;

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

	virtual void printGenome() {
		cout << "Warning! In AbstractGenome::printGenome()...\n";
	}

	virtual void loadGenomeFile(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) {
		cout << "loadGenomeFile() has not been defined for this type of genome!" << endl;
		exit(1);
	}

	// loadGenome will load file "fileName" and return the first genome whos dataMap matches key,value
	// if no genome matches, function returns nullptr
	// if more then one genome matches, function return first match
	virtual shared_ptr<AbstractGenome> loadGenome(string fileName, string key, string value) {
		vector<shared_ptr<AbstractGenome>> genomes;
		loadGenomeFile(fileName, genomes);
		for (auto g : genomes) {
			if (g->dataMap.Get(key) == value) {
				return g;
			}
		}
		return nullptr;
	}

	virtual bool isEmpty() = 0;

	virtual void mutate() = 0;
	virtual shared_ptr<AbstractGenome> makeMutatedGenomeFrom(shared_ptr<AbstractGenome> parent) = 0;
	virtual shared_ptr<AbstractGenome> makeMutatedGenomeFromMany(vector<shared_ptr<AbstractGenome>> parents) = 0;

	virtual int countSites() {
		cout << "Warning! In AbstractGenome::countSites()...\n";
		return 0;
	}

	virtual void recordDataMap() = 0;

};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
