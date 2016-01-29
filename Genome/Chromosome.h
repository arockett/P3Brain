//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Chromosome__
#define __BasicMarkovBrainTemplate__Chromosome__

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
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
		cout << "method readInt() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}
	virtual void writeDouble(int &siteIndex, double value, double valueMin, double valueMax) {
		cout << "method writeDouble() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, int code, int CodingRegionIndex) {
		cout << "method siteToDouble() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}

	virtual shared_ptr<AbstractChromosome> makeCopy() {
		cout << "method makeCopy() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return nullptr;
	}
	virtual void fillRandom() {
		cout << "method fillRandom() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}
	virtual void fillRandom(int length) {
		cout << "method fillRandom(int length) in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual string chromosomeToStr() {
		cout << "method chromosomeToStr() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return "AbstractChromosome";  // return garbage!
	}

	virtual void resize(int size) {
		cout << "method size() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual int size() {
		cout << "method size() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return -1;  // only non-negative values are valid, so return -1
	}

	virtual inline bool modulateIndex(int &siteIndex) {
		cout << "method modulateIndex() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}
	virtual inline bool advanceIndex(int &siteIndex, int distance = 1) {
		cout << "method advanceIndex() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}

	virtual vector<string> getStats() {
		cout << "method getStats() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		vector<string> data;
		return data;
	}

//	virtual void pointMutate(int &siteIndex = -1){ // if -1, mutate a random site
//		cout << "method size() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
//		exit(1);
//	}
//copySegment
//crossover
//deleteSegment
//extractSegment
//insertSegment

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
		virtual void assignCode(int code, int siteIndex, int CodingRegionIndex = 0) {
			if (code >= 0) {
				regions[CodingRegionIndex].push_back( {siteIndex, code});
			}
		}

		// converts codingRegions to a string. If argument is provided, then only that region will
		// will be returned (or a blank string)
		virtual inline string codingRegionsToString(int CodingRegionIndex = -1) {
			string S = "";
			for (auto region : regions) {
				if (CodingRegionIndex < 0 || region.first == CodingRegionIndex) {
					S = S + to_string(region.first) + " -> ";
					for (auto site : region.second) {
						S = S + to_string(site.first) + ":" + to_string(site.second) + "  ";
					}
					S += "\n";
				}
			}
			return S;
		}
	};

	double alphabetSize;  // values in this chromosome will be in the range of 0 to < alphabetSize
	                      // i.e. the number of diffrent values that can be at a site
	                      // for double this is the range of the value
	CodingRegions codingRegions;

	                      // default fail constructor - if alphebetSize default has not been explicitly defined, this is run.
	Chromosome() {
		alphabetSize = 0;
		cout << "ERROR : default alphabetSize for this type being used with Chromosome has not been defined.\nUse Constructor with alphabetSize argument.\n";
		exit(1);
	}

	// make a Chromosome of any type with a defined alpabetSize
	Chromosome(double _alphabetSize) {
		alphabetSize = _alphabetSize;
	}

	virtual ~Chromosome () = default;

	virtual shared_ptr<AbstractChromosome> makeCopy() {
		return make_shared<Chromosome<T>>(alphabetSize);
	}

	// insures that a site index is valid, if in index is > sites.size(), mod it.
	virtual inline bool modulateIndex(int &siteIndex) {
		bool EOC = siteIndex >= (int)sites.size();
		siteIndex = siteIndex % sites.size();
		return EOC;
	}

	// advance a site index 1 or distance sites and check that new index is valid
	virtual inline bool advanceIndex(int &siteIndex, int distance = 1) {
		siteIndex += distance;
		return modulateIndex(siteIndex);
	}

	// read an int value in range [valueMin,valueMax] from chromosome starting at index.
	// will use as many sites as needed. works in base alphabetSize
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
		if (valueMin > valueMax) {
			int temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		double currentMax = alphabetSize;
		modulateIndex(siteIndex);
		value = (int) sites[siteIndex];
		codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		int EOC = advanceIndex(siteIndex);  // EOC = end of chromosome
		while ((valueMax - valueMin + 1) > currentMax) {  // we don't have enough bits of information
			value = (value * alphabetSize) + (int) sites[siteIndex];// next site
			codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
			EOC = EOC | advanceIndex(siteIndex);
			currentMax = currentMax * alphabetSize;
		}
		value = (value % (valueMax - valueMin + 1)) + valueMin;
		return EOC;
	}

	// writes a value into a chromosome, uses a number of sites baised on valueMin and valueMax);
	virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax) {
		bool EOC;
		if (valueMin > valueMax) {
			int temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		vector<T> decomposedValue;
		int writeValueSize = valueMax - valueMin + 1;
		if (writeValueSize < value) {
			cout << "ERROR : attempting to write value to chromosome. \n (valueMax - valueMin + 1) < value!";
		}
		while (writeValueSize > alphabetSize) {  // load value in alphabetSize chunks into decomposedValue
			decomposedValue.push_back(value%((int)alphabetSize));
			value = value/alphabetSize;
			writeValueSize = writeValueSize/alphabetSize;
		}

		modulateIndex(siteIndex);  // make sure that the index is in range
		while ( decomposedValue.size()>0 ) {  // starting with the last element in decomposedValue, copy into genome.
			sites[siteIndex] = decomposedValue[decomposedValue.size()-1];
			EOC = EOC | advanceIndex(siteIndex);
			decomposedValue.pop_back();
		}
		return EOC;
	}

	virtual bool readSite(int &siteIndex, T &value, int code = -1, int CodingRegionIndex = 0) {
		modulateIndex(siteIndex);
		value = sites[siteIndex];
		bool EOC = advanceIndex(siteIndex);
		codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		return EOC;
	}

	// scale value using valueMin and valueMax to alphabetSize and write at siteIndex
	// value - MIN(valueMin,valueMax) must be < ABS(valueMax - valueMin)
	virtual void writeDouble(int &siteIndex, double value, double valueMin, double valueMax) {
		if (valueMin > valueMax) {
			double temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		if ((value - valueMin) > (valueMax - valueMin)) {
			cout << "Error: attempting to write double. range is to small, value: " << value << " is not < valueMax: " << valueMin << " - valueMin: << valueMin\n";
			exit(1);
		}
		value = ((value - valueMin) / (valueMax - valueMin)) * alphabetSize;
		sites[siteIndex] = value;

	}

	// this is a scaling function - while it will work with other types it should only be used with double and float
	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, int code, int CodingRegionIndex) {
		if (valueMin > valueMax) {
			double temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		modulateIndex(siteIndex);
		value = (double) sites[siteIndex];
		codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		int EOC = advanceIndex(siteIndex);
		//scale the value
		value = (value * ((valueMax - valueMin) / alphabetSize)) + valueMin;
		return EOC;
	}

	// resize a chromosome to length and fill will values from alphabet
	virtual void fillRandom(int length) override {
		if (length == -1) {
			length = sites.size();
		}
		sites.resize(length);
		for (int i = 0; i < length; i++) {
			sites[i] = (T) Random::getIndex(alphabetSize);
		}
	}

	virtual void fillRandom() override {
		fillRandom(sites.size());
	}

	// convert a chromosome to a string
	virtual string chromosomeToStr() {
		string S = "";

		for (int i = 0; i < sites.size(); i++) {
			S = S + to_string(sites[i]) + " ";
		}
		S = S + "\n";
		return S;
	}

	virtual void resize(int size) {
		sites.resize(size);
	}
	virtual int size() {
		return sites.size();
	}

	virtual vector<string> getStats() {
		vector<string> dataPairs;
		dataPairs.push_back("alphabetSize");
		dataPairs.push_back(to_string(alphabetSize));
		dataPairs.push_back("size");
		dataPairs.push_back(to_string(sites.size()));
		return (dataPairs);
	}
};

template<> inline void Chromosome<double>::fillRandom(int length) {
	sites.resize(length);
	for (int i = 0; i < length; i++) {
		sites[i] = Random::getDouble(0, alphabetSize);
	}
}
template<> inline void Chromosome<double>::fillRandom() {
	Chromosome<double>::fillRandom(sites.size());
}

#endif /* defined(__BasicMarkovBrainTemplate__Chromosome__) */

