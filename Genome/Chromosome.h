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

template<class T> class Chromosome {
 public:

	class CodingRegions {
 	private:
		map<int, vector<pair<int, int>>> regions;

	public:

		// set a coding region value if that value is > -1 (values < 0 denote that the
		// action should not be recorded in codingRegions
		virtual void set(int code, int siteIndex, int CodingRegionIndex = 0) {
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

		static const int START_CODE = 0;
		static const int IN_COUNT_CODE = 10;
		static const int IN_ADDRESS_CODE = 11;
		static const int OUT_COUNT_CODE = 20;
		static const int OUT_ADDRESS_CODE = 21;
		static const int DATA_CODE = 30;
	};

	vector<T> sites;
	double alphabetSize;  // values in this chromosome will be in the range of 0 to < alphabetSize
	                      // i.e. the number of diffrent values that can be at a site
	                      // for double this is the range of the value
	CodingRegions codingRegions;

	// default fail constructor - if alphebetSize default has not been explicitly defined, this is run.
	Chromosome() {
		alphabetSize = 0;
		cout << "ERROR : default alphabetSize for this type being used with Chromosome has not been defined.\n Use Constructor with alphabetSize argument.";
		exit(1);
	}

	// make a Chromosome of any type with a defined alpabetSize
	Chromosome(double _alphabetSize) {
		alphabetSize = _alphabetSize;
	}

	// insures that a site index is valid
	inline void checkIndex(int &siteIndex) {
		siteIndex = siteIndex % sites.size();
	}

	// advance a site index 1 or distance sites and check that new index is valid
	inline void advanceIndex(int &siteIndex, int distance = 1) {
		siteIndex += distance;
		checkIndex(siteIndex);
	}

	// read an int value in range [valueMin,valueMax] from chromosome starting at index.
	// will use as many sites as needed. works in base alphabetSize
	int readInt(int &siteIndex, int valueMin, int valueMax, int code = -1, int CodingRegionIndex = 0) {
		if (valueMin > valueMax) {
			int temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		double currentMax = alphabetSize;
		checkIndex(siteIndex);
		int value = (int) sites[siteIndex];
		codingRegions.set(code, siteIndex, CodingRegionIndex);
		advanceIndex(siteIndex);
		while ((valueMax - valueMin + 1) > currentMax) {  // we don't have enough bits of information
			value = (value * alphabetSize) + (int) sites[siteIndex];// next site
			codingRegions.set(code, siteIndex, CodingRegionIndex);
			advanceIndex(siteIndex);
			currentMax = currentMax * alphabetSize;
		}
		return (value % (valueMax - valueMin + 1)) + valueMin;
	}

	T readSite(int &siteIndex, int code = -1, int CodingRegionIndex = 0) {
		checkIndex(siteIndex);
		T value = sites[siteIndex];
		advanceIndex(siteIndex);
		codingRegions.set(code, siteIndex, CodingRegionIndex);
		return value;
	}

	// this is a scaling function - while it will work with other types it should only be used with double and float
	double siteToDouble(int &siteIndex, double valueMin, double valueMax, int code, int CodingRegionIndex) {
		if (valueMin > valueMax) {
			double temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		checkIndex(siteIndex);
		double value = (double) sites[siteIndex];
		codingRegions.set(code, siteIndex, CodingRegionIndex);
		advanceIndex(siteIndex);
		//scale the value
		return (value * ((valueMax - valueMin + 1) / alphabetSize) + valueMin);
	}

	// resize a chromosome to length and fill will values from alphabet
	void fillRandom(int length) {
		sites.resize(length);
		for (int i = 0; i < length; i++) {
			sites[i] = (T) Random::getIndex(alphabetSize);
		}
	}

	// convert a chromosome to a string
	string chromosomeToStr() {
		string S = "";

		for (int i = 0; i < sites.size(); i++) {
			S = S + to_string(sites[i]) + " ";
		}
		S = S + "\n";
		return S;
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__Chromosome__) */

