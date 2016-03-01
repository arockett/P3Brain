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
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0) {
		cout << "method readInt() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}
	virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax, bool readDirection) {
		cout << "method writeInt() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}
	virtual bool writeDouble(int &siteIndex, double value, double valueMin, double valueMax, bool readDirection) {
		cout << "method writeDouble() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, bool readDirection, int code, int CodingRegionIndex) {
		cout << "method siteToDouble() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}

	virtual shared_ptr<AbstractChromosome> makeLike() {
		cout << "method makeCopy() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return nullptr;
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

	virtual void fillAcending(int &start) {
		cout << "method fillAcending(int &start) in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual void fillConstant(const int value) {
		cout << "method fillConstant(const int value) in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual void readChromosomeFromSS(stringstream &ss, int _chromosomeLength){
		cout << "method readChromosomeFromFile() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
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
	virtual inline bool advanceIndex(int &siteIndex, bool readDirection = 1, int distance = 1) {
		cout << "method advanceIndex() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return false;  // not at end of chromosome
	}

	virtual vector<string> getFixedStats() {
		cout << "method getStatsFixed() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		vector<string> data;
		return data;
	}

	virtual vector<string> getStats() {
		cout << "method getStats() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		vector<string> data;
		return data;
	}

	virtual shared_ptr<AbstractChromosome> getSegment(int minSize, int maxSize) {
		cout << "method getSegment() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
		return nullptr;
	}

	virtual void insertSegment(shared_ptr<AbstractChromosome> segment) {
		cout << "method insertSegment() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual void mutatePoint() {
		cout << "method mutatePoint() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual void mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) {
		cout << "method mutateCopy() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual void mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) {
		cout << "method mutateDelete() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
		exit(1);
	}

	virtual void crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount) {
		cout << "method crossover() in AbstractChromosome was called!\n This class only exists for polymorphism.\n";
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
		cout << "ERROR : Chromosome constructor must specify chromosome length\n";
		exit(1);
	}

	Chromosome(int chromosomeLength) {
		alphabetSize = 0;
		cout << "ERROR : TYPE specified for Chromosome is not supported.\nTypes supported are: int, double, bool, unsigned char\n";
		exit(1);
	}

	Chromosome(int chromosomeLength, double _alphabetSize) {
		alphabetSize = 0;
		cout << "ERROR : TYPE specified for Chromosome is not supported.\nTypes supported are: int, double, bool, unsigned char\n";
		exit(1);
	}

	virtual ~Chromosome () = default;

	// return a shared_ptr to a new chromosome like this one (same alphabetSize and sites.size())
	virtual shared_ptr<AbstractChromosome> makeLike() {
		return make_shared<Chromosome<T>>(sites.size(),alphabetSize);
	}

	virtual shared_ptr<AbstractChromosome> makeCopy() {
		shared_ptr<Chromosome> newChromosome = make_shared<Chromosome<T>>(sites.size(),alphabetSize);
		newChromosome->sites = sites;
		return newChromosome;
	}

	// insures that a site index is valid, if in index is > sites.size(), mod it.
	// return true if siteIndex went out of range
	virtual inline bool modulateIndex(int &siteIndex) {
		if (siteIndex >= (int)sites.size() || siteIndex<0){
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
	virtual bool readInt(int &siteIndex, int &value, int valueMin, int valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0) {
		if (valueMin > valueMax) {
			int temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		double currentMax = alphabetSize;
		modulateIndex(siteIndex);
		value = (int) sites[siteIndex];
		codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		int EOC = advanceIndex(siteIndex,readDirection);  // EOC = end of chromosome
		while ((valueMax - valueMin + 1) > currentMax) {  // we don't have enough bits of information
			value = (value * alphabetSize) + (int) sites[siteIndex];// next site
			codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
			EOC = EOC | advanceIndex(siteIndex,readDirection);
			currentMax = currentMax * alphabetSize;
		}
		value = (value % (valueMax - valueMin + 1)) + valueMin;
		return EOC;
	}

	// writes a value into a chromosome, uses a number of sites baised on valueMin and valueMax);
	// return true if siteIndex went out of range
	virtual bool writeInt(int &siteIndex, int value, int valueMin, int valueMax, bool readDirection) {
		bool EOC;
		if (valueMin > valueMax) {
			int temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		vector<T> decomposedValue;
		int writeValueSize = valueMax - valueMin + 1;
		if (writeValueSize < value) {
			cout << "ERROR : attempting to write value to chromosome. \n value is too large :: (valueMax - valueMin + 1) < value!\n";
			exit(1);
		}
		while (writeValueSize > alphabetSize) {  // load value in alphabetSize chunks into decomposedValue
			decomposedValue.push_back(value%((int)alphabetSize));
			value = value/alphabetSize;
			writeValueSize = writeValueSize/alphabetSize;
		}
		decomposedValue.push_back(value);
		EOC = modulateIndex(siteIndex);  // make sure that the index is in range
		while ( decomposedValue.size()>0 ) {  // starting with the last element in decomposedValue, copy into genome.
			sites[siteIndex] = decomposedValue[decomposedValue.size()-1];
			EOC = EOC | advanceIndex(siteIndex,readDirection);
			decomposedValue.pop_back();
		}
		return EOC;
	}

	// read one site of type T
	// return true if siteIndex went out of range
	virtual bool readSite(int &siteIndex, T &value, bool readDirection, int code = -1, int CodingRegionIndex = 0) {
		bool EOC = modulateIndex(siteIndex);
		value = sites[siteIndex];
		EOC = EOC | advanceIndex(siteIndex,readDirection);
		codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		return EOC;
	}

	// scale value using valueMin and valueMax to alphabetSize and write at siteIndex
	// value - MIN(valueMin,valueMax) must be < ABS(valueMax - valueMin)
	virtual bool writeDouble(int &siteIndex, double value, double valueMin, double valueMax, bool readDirection) {
		bool EOC = modulateIndex(siteIndex);
		if (valueMin > valueMax) {
			double temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		if ((value - valueMin) > (valueMax - valueMin)) {
			cout << "Error: attempting to write double. given range is to small, value: " << value << " is not < valueMax: " << valueMin << " - valueMin: << valueMin\n";
			exit(1);
		}
		value = ((value - valueMin) / (valueMax - valueMin)) * alphabetSize;
		sites[siteIndex] = value;
		EOC = EOC | advanceIndex(siteIndex,readDirection);
		return EOC;
	}

	// this is a scaling function - while it will work with other types it should only be used with double and float
	virtual bool siteToDouble(int &siteIndex, double &value, double valueMin, double valueMax, bool readDirection, int code = -1, int CodingRegionIndex = 0) {
		if (valueMin > valueMax) {
			double temp = valueMin;
			valueMax = valueMin;
			valueMin = temp;
		}
		bool EOC = modulateIndex(siteIndex);
		value = (double) sites[siteIndex];
		codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		EOC = EOC | advanceIndex(siteIndex,readDirection);
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

	// starting with value start, fill this chromosome with acending values.
	// This function is to make testing easy.
	virtual void fillAcending(int &start) override {
		for (int i = 0; i < sites.size(); i++) {
			sites[i] = (T)(start%(int)alphabetSize);
			start++;
		}
	}

	// fill all the sites of this chromosome with value.
	// This function is to make testing easy.
	virtual void fillConstant(const int value) override {
		for (int i = 0; i < sites.size(); i++) {
			sites[i] = (T)(value%(int)alphabetSize);
		}
	}

	virtual void readChromosomeFromSS(std::stringstream &ss, int _chromosomeLength){
		int value;
		char rubbish;
		sites.clear();
		for (int i = 0; i<_chromosomeLength;i++){
			ss >> value >> rubbish;
			sites.push_back(value);
		}
	}

	// convert a chromosome to a string
	virtual string chromosomeToStr() {
		string S = "";

		for (int i = 0; i < sites.size(); i++) {
			S = S + FileManager::separator + to_string(sites[i]);
		}
		S.erase(S.begin());  // clip off the leading separator
		return S;
	}

	virtual void resize(int size) {
		sites.resize(size);
	}
	virtual int size() {
		return (int)sites.size();
	}

	virtual vector<string> getFixedStats() {
		vector<string> dataPairs;
		dataPairs.push_back("alphabetSize");
		dataPairs.push_back(to_string(alphabetSize));
		return (dataPairs);
	}

	virtual vector<string> getStats() {
		vector<string> dataPairs;
		dataPairs.push_back("alphabetSize");
		dataPairs.push_back(to_string(alphabetSize));
		dataPairs.push_back("size");
		dataPairs.push_back(to_string(sites.size()));
		return (dataPairs);
	}

	//////////////////////////////
	// MUTATION RELATED METHODS //
	//////////////////////////////

	// create a new chromosome with a copied peice of this chromosome. The size of the copy
	// will be random between minSize and maxSize. the copy location will be determined randomly
	// but will at lease segmentSize before the end of the chromosome.
	virtual shared_ptr<AbstractChromosome> getSegment(int minSize, int maxSize) {
		int segmentSize = Random::getInt(maxSize-minSize)+minSize;
		if (segmentSize > sites.size()) {
			cout << "segmentSize = " << segmentSize << "  sites.size() = " << sites.size() << endl;
			cout << "maxSize:minSize" << maxSize << ":" << minSize << endl;
			cout << "ERROR: in Chromosome::getSegement, segmentSize is > then sites.size()!\nExitting!"<<endl;
			exit(1);
		}
		int segmentStart = Random::getInt(sites.size()-segmentSize);
		shared_ptr<Chromosome<T>> segment = make_shared<Chromosome<T>>(0,alphabetSize);
		auto it = sites.begin();
		segment->sites.insert(segment->sites.begin(),it+segmentStart,it+segmentStart+segmentSize);
		// copy sites from this to segment (the new chromosome)
		// starting at segementStart and ending at segementStart+segmentSize
		return segment;
	}

	// insert from segement into this chromosome. The insertion location is random.
	virtual void insertSegment(shared_ptr<AbstractChromosome> segment) {
		auto it = sites.begin();
		auto castSegment = dynamic_pointer_cast<Chromosome<T>>(segment);
		sites.insert(it+Random::getInt(sites.size()),castSegment->sites.begin(),castSegment->sites.end());
	}

	virtual void mutatePoint() {
		sites[Random::getIndex(sites.size())]=Random::getIndex(alphabetSize);

	}

	// mutate chromosome by getting a copy of a segment of this chromosome and
	// inserting that segment randomly into this chromosome
	virtual void mutateCopy(int minSize, int maxSize, int chromosomeSizeMax) {
		//cout << "C .. " << size() << " " << chromosomeSizeMax << ": ";
		if (size() < chromosomeSizeMax) {
			//cout << "size: " << sites.size() << "->";
			shared_ptr<Chromosome<T>> segment = dynamic_pointer_cast<Chromosome<T>>(getSegment(minSize, maxSize));
			insertSegment(segment);
			//cout << sites.size() << endl;
		}
	}

	// delete a random segement from the chromosome
	virtual void mutateDelete(int minSize, int maxSize, int chromosomeSizeMin) {
		//cout << "D .. " << size() << " " << chromosomeSizeMin << ": ";
		if (size() > chromosomeSizeMin) {
			int segmentSize = Random::getInt(maxSize-minSize)+minSize;
			//cout << "segSize: " << segmentSize << "\tsize: " << sites.size() << "\t->\t";
			if (segmentSize > sites.size()) {
				cout << "segmentSize = " << segmentSize << "  sites.size() = " << sites.size() << endl;
				cout << "maxSize : minSize   " << maxSize << " : " << minSize << endl;
				cout << "ERROR: in Chromosome::mutateDelete, segmentSize is > then sites.size()!\nExitting!\n";
				exit(1);
			}
			int segmentStart = Random::getInt(sites.size()-segmentSize);
			sites.erase(sites.begin()+segmentStart,sites.begin()+segmentStart+segmentSize);
			//cout << sites.size() << endl;
		}
		//cout << endl;
	}

	// delete the sites of this chromosome. Then set sites to a crossed over chromosome made up of parents
	virtual void crossover(vector<shared_ptr<AbstractChromosome>> parents, int crossCount) {
		sites.resize(0);  // clear sites for this chromosome

		if (parents.size() == 0) {
			cout << "ERROR! in Chromosome crossover... attempt to cross 0 parents\nExiting!\n";
			exit(1);
		}
		if (parents.size() == 1) {  // if there is only one parent, we are done.
			sites = dynamic_pointer_cast<Chromosome<T>>(parents[0])->sites;
		} else {

			// extract the sites list from each parent
			vector<vector<T>> parentSites;
			for (auto parent:parents) {
				parentSites.push_back(dynamic_pointer_cast<Chromosome<T>>(parent)->sites);
			}

			// randomly determine crossCount number crossLocations
			vector<double> crossLocations;
			for (int i = 0; i<crossCount; i++) {	// get some cross locations (% of length of chromosome)
				crossLocations.push_back(Random::getDouble(1.0));
			}
			crossLocations.push_back(0.0);  // add start and end
			crossLocations.push_back(1.0);

			sort(crossLocations.begin(), crossLocations.end());// sort crossLocations

//			cout << "crossing: ";
//			for (auto location:crossLocations){
//				cout << location << "  ";
//			}
//			cout << "size init: " << size() << " -> ";

			int pick;
			int lastPick = Random::getIndex(parents.size());
			for(int c = 0; c < crossLocations.size()-1; c++) {
				// pick a chromosome to cross with. Make sure it's not the same chromosome!
				pick = Random::getIndex(parents.size()-1);
				if (pick == lastPick) {
					pick++;
				}
				lastPick = pick;
				// add the segment to this chromosome
//				cout << "(" << parentSites[pick].size() << ") "<< (int)((double)parentSites[pick].size()*crossLocations[c]) << " " << (int)((double)parentSites[pick].size()*crossLocations[c+1]) << " ";
				sites.insert(sites.end(),
						parentSites[pick].begin()+(int)((double)parentSites[pick].size()*crossLocations[c]),
						parentSites[pick].begin()+(int)((double)parentSites[pick].size()*crossLocations[c+1]));
			}
//			cout << "size post: " << size() << endl;
//			cout << "      " << chromosomeToStr() << "\nfrom: " << parents[0]->chromosomeToStr() << "\n+\n and: " << parents[1]->chromosomeToStr() << endl << endl;
		}
		 //sites = dynamic_pointer_cast<Chromosome<T>>(parents[0])->sites; // uncomment to turn of crossover
	}

};

////////////////////////
// SUPPORT FOR DOUBLE //
////////////////////////

template<> inline void Chromosome<double>::fillRandom(int length) {
	sites.resize(length);
	for (int i = 0; i < length; i++) {
		sites[i] = Random::getDouble(0, alphabetSize);
	}
}
template<> inline void Chromosome<double>::fillRandom() {
	Chromosome<double>::fillRandom(sites.size());
}

template<> inline void Chromosome<double>::mutatePoint() {
	sites[Random::getIndex(sites.size())] = Random::getDouble(alphabetSize);
}

#endif /* defined(__BasicMarkovBrainTemplate__Chromosome__) */

