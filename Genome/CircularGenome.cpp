//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "../Global.h"
#include "CircularGenome.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

// Initialize Parameters
const int& CircularGenomeParameters::initialGenomeSize = Parameters::register_parameter("circularGenome_SizeInitial", 2000, "starting size for genome", "GENOME - CIRCULAR");
const double& CircularGenomeParameters::pointMutationRate = Parameters::register_parameter("circularGenome_pointMutationRate", 0.005, "per site point mutation rate", "GENOME - CIRCULAR");
const double& CircularGenomeParameters::insertionRate = Parameters::register_parameter("circularGenome_mutationCopyRate", 0.00002, "per site insertion rate", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::insertionMinSize = Parameters::register_parameter("circularGenome_mutationCopyMinSize", 10, "minimum size of insertion mutation", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::insertionMaxSize = Parameters::register_parameter("circularGenome_mutationCopyMaxSize", 200, "maximum size of insertion mutation", "GENOME - CIRCULAR");
const double& CircularGenomeParameters::deletionRate = Parameters::register_parameter("circularGenome_mutationDeletionRate", 0.00002, "per site deletion rate", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::deletionMinSize = Parameters::register_parameter("circularGenome_mutationDeletionMinSize", 10, "minimum size of insertion mutation", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::deletionMaxSize = Parameters::register_parameter("circularGenome_mutationDeletionMaxSize", 200, "maximum size of insertion mutation", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::maxGenomeSize = Parameters::register_parameter("circularGenome_SizeMin", 400, "if genome is smaller then this, mutations will only increase chromosome size", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::minGenomeSize = Parameters::register_parameter("circularGenome_SizeMax", 5000, "if genome is larger then this, mutations will only decrease chromosome size", "GENOME - CIRCULAR");
const int& CircularGenomeParameters::crossCount = Parameters::register_parameter("circularGenome_crossCount", 3, "number of crosses when performing crossover", "GENOME - CIRCULAR");

// constructor
template<class T>
CircularGenome<T>::Handler::Handler(shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	genome = dynamic_pointer_cast<CircularGenome>(_genome);
	setReadDirection(_readDirection);
	resetHandler();
}

template<class T>
void CircularGenome<T>::Handler::resetHandler() {
	if (readDirection) {  // if reading forward
		siteIndex = 0;
	} else {  // if reading backwards
		siteIndex = genome->size() - 1;  // set to last site in last chromosome
	}
	resetEOG();
	resetEOC();
}

template<class T>
void CircularGenome<T>::Handler::resetHandlerOnChromosome() {
	if (readDirection) {  // if reading forward
		siteIndex = 0;
	} else {  // if reading backwards
		siteIndex = genome->size() - 1;  // set to last site in last chromosome
	}
	resetEOC();
}
// modulateIndex checks to see if the current chromosomeIndex and siteIndex are out of range. if they are
// it uses readDirection to resolve them.	virtual void copyFrom(shared_ptr<Genome> from) {

//  modulate index truncates nonexistant sites. i.e. if the current addres is chromosome 1, site 10 and
// chromosome 10 is 8 long, modulateIndex will set the index to chromosome 2, site 0 (not site 2).
// If this behavior is required, use advance Index instead.
// If the chromosomeIndex has past the last chromosome (or the first
// if read direction = -1) then EOG (end of genome) is set true.
template<class T>
void CircularGenome<T>::Handler::modulateIndex() {
	if (readDirection) {
		// first see if we are past last chromosome
		if (siteIndex >= (int) genome->size()) {
			siteIndex = 0;  // reset chromosomeIndex
			EOG = true;  // if we are past the last chromosome then EOG = true
			EOC = true;
		}
	} else {  //reading backwards!
		// first see if we are past last chromosome
		if (siteIndex < 0) {
			siteIndex = genome->size() - 1;
			EOG = true;  // if we are past the last chromosome then EOG = true
			EOC = true;
		}
	}
}

// advanceIndex will move the index forward distance sites.
// if there are too few sites, it will advance to the next chromosome and then advance addtional sites (if needed)
// NOTE: if the advance is > the current chromosome size, it will be modded to the chromosome size.
// i.e. if the chromosome was length 10, and the current siteIndex = 0, advanceIndex(15) will advance to
// site 5 of the next chromosome. Should this be fixed?!??
template<class T>
void CircularGenome<T>::Handler::advanceIndex(int distance) {
	if (readDirection) {  // reading forward
			siteIndex += distance;  // if there are enough sites left in the current chromosome, just move siteIndex
	} else {  // reading backwards
			siteIndex -= distance;  // if there are enough sites in the current chromosome (between siteIndex and start of chromosome) just move siteIndex
	}
	modulateIndex();
}

// returns true if this Handler has reached the end of genome (or start if direction is backwards).
template<class T>
bool CircularGenome<T>::Handler::atEOG() {
	modulateIndex();
	return EOG;
}

template<class T>
bool CircularGenome<T>::Handler::atEOC() {
	modulateIndex();
	return EOC;
}

template<class T>
void CircularGenome<T>::Handler::printIndex() {
	string rd = (readDirection) ? "forward" : "backwards";

	cout << "  siteIndex: " << siteIndex << "  EOC: " << EOC << "  EOG: " << EOG << "  direction: " << rd << endl;
}

template<class T>
int CircularGenome<T>::Handler::readInt(int valueMin, int valueMax, int code, int CodingRegionIndex) {
	int value;
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	double currentMax = genome->alphabetSize;
	value = (int) genome->sites[siteIndex];
	//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
	advanceIndex();  // EOC = end of chromosome
	while ((valueMax - valueMin + 1) > currentMax) {  // we don't have enough bits of information
		value = (value * genome->alphabetSize) + (int) genome->sites[siteIndex];  // next site
		//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
		advanceIndex();
		currentMax = currentMax * genome->alphabetSize;
	}
	return (value % (valueMax - valueMin + 1)) + valueMin;;
}

template<class T>
double CircularGenome<T>::Handler::readDouble(double valueMin, double valueMax, int code, int CodingRegionIndex) {
	double value;
	if (valueMin > valueMax) {
		double temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	value = (double) genome->sites[siteIndex];
	//codingRegions.assignCode(code, siteIndex, CodingRegionIndex);
	advanceIndex();
	//scale the value
	return (value * ((valueMax - valueMin) / genome->alphabetSize)) + valueMin;
}

template<class T>
void CircularGenome<T>::Handler::writeInt(int value, int valueMin, int valueMax) {
	if (valueMin > valueMax) {
		int temp = valueMin;
		valueMax = valueMin;
		valueMin = temp;
	}
	vector < T > decomposedValue;
	int writeValueBase = valueMax - valueMin + 1;
	value = value - valueMin;
	if (writeValueBase < value) {
		cout << "ERROR : attempting to write value to chromosome. \n value is too large :: (valueMax - valueMin + 1) < value!\n";
		exit(1);
	}
	while (writeValueBase > genome->alphabetSize) {  // load value in alphabetSize chunks into decomposedValue
		decomposedValue.push_back(value % ((int) genome->alphabetSize));
		value = value / genome->alphabetSize;
		writeValueBase = writeValueBase / genome->alphabetSize;
	}
	decomposedValue.push_back(value);
	while (decomposedValue.size() > 0) {  // starting with the last element in decomposedValue, copy into genome.
		genome->sites[siteIndex] = decomposedValue[decomposedValue.size() - 1];
		advanceIndex();
		decomposedValue.pop_back();
	}
}

template<class T>
void CircularGenome<T>::Handler::copyTo(shared_ptr<AbstractGenome::Handler> to) {
	auto castTo = dynamic_pointer_cast<CircularGenome<T>::Handler>(to);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	castTo->readDirection = readDirection;
	castTo->genome = genome;
	castTo->siteIndex = siteIndex;
	castTo->EOG = EOG;
}

template<class T>
bool CircularGenome<T>::Handler::inTelomere(int length) {
	modulateIndex();
	if (atEOG()) {
		return true;
	}
	if (readDirection) {  // if reading forward
		return (siteIndex >= (genome->size() - length));
	} else {
		return (siteIndex - 1 < length);
	}
}

template<class T>
void CircularGenome<T>::Handler::randomize() {
	siteIndex = Random::getIndex(genome->size());
}

template<class T>
vector<vector<int>> CircularGenome<T>::Handler::readTable(pair<int, int> tableSize, pair<int, int> tableMaxSize, pair<int, int> valueRange, int code, int CodingRegionIndex) {
	vector<vector<int>> table;
	int x = 0;
	int y = 0;
	int Y = tableSize.first;
	int X = tableSize.second;
	int maxY = tableMaxSize.first;
	int maxX = tableMaxSize.second;

	table.resize(Y);  // set the number of rows in the table

	for (; y < (Y); y++) {
		table[y].resize(X);  // set the number of columns in this row
		for (x = 0; x < X; x++) {
			//table[y][x] = (Type) (sites[index]);
			table[y][x] = readInt(valueRange.first, valueRange.second, code, CodingRegionIndex);
		}
		for (; x < maxX; x++) {
			readInt(valueRange.first, valueRange.second);  // advance genomeIndex to account for unused entries in the max sized table for this row
		}
	}
	for (; y < (maxY); y++) {
		for (x = 0; x < maxX; x++) {
			readInt(valueRange.first, valueRange.second);  // advance to account for unused rows
		}
	}
	return table;
}

template<class T>
void CircularGenome<T>::setupCircularGenome(int _size, double _alphabetSize){
	sites.resize(_size);
	alphabetSize = _alphabetSize;
	// define columns to be written to genome files
	genomeFileColumns.clear();
	genomeFileColumns.push_back("update");
	genomeFileColumns.push_back("ID");
	genomeFileColumns.push_back("alphabetSize");
	genomeFileColumns.push_back("genomeLength");
	genomeFileColumns.push_back("sites");
	// define columns to added to ave files
	aveFileColumns.clear();
	aveFileColumns.push_back("genomeLength");

	recordDataMap();
}

template<class T>
CircularGenome<T>::CircularGenome(int _size, double _alphabetSize) {
	setupCircularGenome(_size, _alphabetSize);
	cout << "ERROR : TYPE specified for CircularGenome is not supported.\nTypes supported are: int, double, bool, unsigned char\n";
	exit(1);
}

template<>
CircularGenome<int>::CircularGenome(int _size, double _alphabetSize) {
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<bool>::CircularGenome(int _size, double _alphabetSize) {
	if (_alphabetSize != 2) {
		cout << "ERROR: in CircularGenome constructor, alphabetSize for bool must be 2!\n";
		exit(1);
	}
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<double>::CircularGenome(int _size, double _alphabetSize) {
	setupCircularGenome(_size, _alphabetSize);
}
template<>
CircularGenome<unsigned char>::CircularGenome(int _size, double _alphabetSize) {
	if (_alphabetSize > 256 || _alphabetSize < 2) {
		cout << "ERROR: in CircularGenome constructor, alphabetSize for unsigned char must be 2 or greater and 256 or less!\n";
		exit(1);
	}
	setupCircularGenome(_size, _alphabetSize);
}


template<class T>
shared_ptr<AbstractGenome::Handler> CircularGenome<T>::newHandler(shared_ptr<AbstractGenome> _genome, bool _readDirection) {
	//cout << "In Genome::newHandler()" << endl;
	return make_shared<Handler>(_genome, _readDirection);
}

template<class T> int CircularGenome<T>::size() {
	return (int) sites.size();
}

template<class T>
double CircularGenome<T>::getAlphabetSize() {
	return alphabetSize;
}

// randomize this genomes contents
template<class T>
void CircularGenome<T>::fillRandom() {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = (T) Random::getIndex(alphabetSize);
	}
}

template<> inline void CircularGenome<double>::fillRandom() {
	for (size_t i = 0; i < sites.size(); i++) {
		sites[i] = Random::getDouble(0, alphabetSize);
	}
}

// fill all sites of this genome with ascending values
// This function is to make testing easy.
template<class T>
void CircularGenome<T>::fillAcending() {
	for (int i = 0; i < sites.size(); i++) {
		sites[i] = i % (int) alphabetSize;
	}
}

// fill all sites of this genome with value
// if "acendingChromosomes" = true, then increment value after each chromosome
// This function is to make testing easy.
template<class T>
void CircularGenome<T>::fillConstant(int value) {
	for (int i = 0; i < sites.size(); i++) {
		sites[i] = value;
	}
}

// Copy functions

// copy the contents of another genome to this genome
// no undefined action, this function must be defined
template<class T>
void CircularGenome<T>::copyFrom(shared_ptr<AbstractGenome> from) {
	auto castFrom = dynamic_pointer_cast<CircularGenome<T>>(from);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.
	alphabetSize = castFrom->alphabetSize;
	sites.clear();
	for (auto site : castFrom->sites) {
		sites.push_back(site);
	}
	PT.copy(castFrom->PT);
}

// Mutation functions

template<class T>
int CircularGenome<T>::countSites() {
	return sites.size();
}

template<class T>
bool CircularGenome<T>::isEmpty() {
	//cout << "in Genome::isEmpty(): " << to_string(countSites() == 0) << " : " << to_string(countSites()) << endl;
	return (countSites() == 0);
}

template<class T>
void CircularGenome<T>::pointMutate() {
	sites[Random::getIndex(sites.size())] = Random::getIndex(alphabetSize);
}

template<>
void CircularGenome<double>::pointMutate() {
	sites[Random::getIndex(sites.size())] = Random::getDouble(alphabetSize);
}

// apply mutations to this genome
template<class T>
void CircularGenome<T>::mutate() {
	int howManyPoint = Random::getBinomial(sites.size(), PT.lookup("circularGenome_pointMutationRate"));
	int howManyCopy = Random::getBinomial(sites.size(), PT.lookup("circularGenome_mutationCopyRate"));
	int howManyDelete = Random::getBinomial(sites.size(), PT.lookup("circularGenome_mutationDeletionRate"));
	// do some point mutations
	for (int i = 0; i < howManyPoint; i++) {
		pointMutate();
	}
	// do some copy mutations
	for (int i = 0; i < howManyCopy && (sites.size() < PT.lookup("circularGenome_SizeMax")); i++) {
		//chromosome->mutateCopy(PT.lookup("mutationCopyMinSize"), PT.lookup("mutationCopyMaxSize"), PT.lookup("chromosomeSizeMax"));

		//cout << "size: " << sites.size() << "->";
		////shared_ptr<Chromosome<T>> segment = dynamic_pointer_cast<Chromosome<T>>(getSegment(minSize, maxSize));

		int segmentSize = Random::getInt(PT.lookup("circularGenome_mutationCopyMaxSize") - PT.lookup("circularGenome_mutationCopyMinSize")) + PT.lookup("circularGenome_mutationCopyMinSize");
		if (segmentSize > sites.size()) {
			cout << "segmentSize = " << segmentSize << "  sites.size() = " << sites.size() << endl;
			cout << "maxSize:minSize" << PT.lookup("circularGenome_mutationCopyMaxSize") << ":" << PT.lookup("circularGenome_mutationCopyMinSize") << endl;
			cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for insert is > then sites.size()!\nExitting!" << endl;
			exit(1);
		}
		int segmentStart = Random::getInt(sites.size() - segmentSize);
		vector<T> segment;
		segment.clear();
		auto it = sites.begin();
		segment.insert(segment.begin(), it + segmentStart, it + segmentStart + segmentSize);

		////insertSegment(segment);
		it = sites.begin();
		sites.insert(it + Random::getInt(sites.size()), segment.begin(), segment.end());

		//cout << sites.size() << endl;
	}
	// do some deletion mutations
	for (int i = 0; i < howManyDelete && (sites.size() > PT.lookup("circularGenome_SizeMin")); i++) {
		//chromosome->mutateDelete(PT.lookup("mutationDeletionMinSize"), PT.lookup("mutationDeletionMaxSize"), PT.lookup("chromosomeSizeMin"));
		int segmentSize = Random::getInt(PT.lookup("circularGenome_mutationDeletionMaxSize") - PT.lookup("circularGenome_mutationDeletionMinSize")) + PT.lookup("circularGenome_mutationDeletionMinSize");
		//cout << "segSize: " << segmentSize << "\tsize: " << sites.size() << "\t->\t";
		if (segmentSize > sites.size()) {
			cout << "segmentSize = " << segmentSize << "  sites.size() = " << sites.size() << endl;
			cout << "maxSize : minSize   " << PT.lookup("circularGenome_mutationDeletionMaxSize") << " : " << PT.lookup("circularGenome_mutationDeletionMinSize") << endl;
			cout << "ERROR: in curlarGenome<T>::mutate(), segmentSize for delete is > then sites.size()!\nExitting!" << endl;
			exit(1);
		}
		int segmentStart = Random::getInt(sites.size() - segmentSize);
		sites.erase(sites.begin() + segmentStart, sites.begin() + segmentStart + segmentSize);

	}
}

// make a mutated genome. from this genome
// the undefined action is to return a new genome
template<class T>
shared_ptr<AbstractGenome> CircularGenome<T>::makeMutatedGenomeFrom(shared_ptr<AbstractGenome> parent) {
	auto newGenome = make_shared<CircularGenome<T>>();
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
template<class T>
shared_ptr<AbstractGenome> CircularGenome<T>::makeMutatedGenomeFromMany(vector<shared_ptr<AbstractGenome>> parents) {
//	cout << "In Genome::makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents)\n";
	// first, check to make sure that parent genomes are conpatable.
	auto castParent0 = dynamic_pointer_cast<CircularGenome<T>>(parents[0]);  // we will be pulling all sorts of stuff from this genome so lets just cast it once.

	auto newGenome = make_shared<CircularGenome<T>>();
	newGenome->alphabetSize = castParent0->alphabetSize;
	newGenome->PT.copy(castParent0->PT);  // copy ParametersTable from 0th parent

	vector<shared_ptr<AbstractChromosome>> parentChromosomes;
//	for (auto rawParent : parents) {
//		auto parent = dynamic_pointer_cast<CircularGenome<T>>(rawParent);
//		parentChromosomes.push_back(parent->chromosomes[i]);  // make a vector that contains the nth chromosome from each parent
//	}
//	newGenome->chromosomes[newGenome->chromosomes.size() - 1]->crossover(parentChromosomes, newGenome->PT.lookup("genomecrossCount"));  // create a crossover chromosome

	if (parents.size() == 0) {
		cout << "ERROR! in CircularGenome<T>::makeMutatedGenomeFromMany crossover... attempt to cross 0 parents\nExiting!\n";
		exit(1);
	}
	if (parents.size() == 1) {  // if there is only one parent...
		return castParent0->makeMutatedGenomeFrom(castParent0);
	} else {

		// extract the sites list from each parent
		vector < vector < T >> parentSites;
		for (auto parent : parents) {
			parentSites.push_back(dynamic_pointer_cast < CircularGenome<T>> (parent)->sites);
		}

		// randomly determine crossCount number crossLocations
		vector<double> crossLocations;
		for (int i = 0; i < PT.lookup("circularGenome_crossCount"); i++) {	// get some cross locations (% of length of chromosome)
			crossLocations.push_back(Random::getDouble(1.0));
		}
		crossLocations.push_back(0.0);  // add start and end
		crossLocations.push_back(1.0);

		sort(crossLocations.begin(), crossLocations.end());  // sort crossLocations

//			cout << "crossing: ";
//			for (auto location:crossLocations){
//				cout << location << "  ";
//			}
//			cout << "size init: " << size() << " -> ";

		int pick;
		int lastPick = Random::getIndex(parents.size());
		newGenome->sites.clear();
		for (int c = 0; c < crossLocations.size() - 1; c++) {
			// pick a chromosome to cross with. Make sure it's not the same chromosome!
			pick = Random::getIndex(parents.size() - 1);
			if (pick == lastPick) {
				pick++;
			}
			lastPick = pick;
			// add the segment to this chromosome
			//cout << "(" << parentSites[pick].size() << ") "<< (int)((double)parentSites[pick].size()*crossLocations[c]) << " " << (int)((double)parentSites[pick].size()*crossLocations[c+1]) << " ";
			newGenome->sites.insert(newGenome->sites.end(), parentSites[pick].begin() + (int) ((double) parentSites[pick].size() * crossLocations[c]), parentSites[pick].begin() + (int) ((double) parentSites[pick].size() * crossLocations[c + 1]));
		}
	}
	newGenome->mutate();
	newGenome->recordDataMap();
//	cout << "  Leaving Genome::makeMutatedGenome(vector<shared_ptr<AbstractGenome>> parents)\n";
	return newGenome;
}

// IO and Data Management functions

// gets data about genome which can be added to a data map
// data is in pairs of strings (key, value)
// the undefined action is to return an empty vector

template<class T>
vector<string> CircularGenome<T>::getStats() {
	vector<string> dataPairs;
	dataPairs.push_back("genomeLength");
	dataPairs.push_back(to_string(countSites()));
	return (dataPairs);
}

template<class T>
void CircularGenome<T>::recordDataMap() {
	dataMap.Set("alphabetSize", alphabetSize);
	dataMap.Set("genomeLength", countSites());
}

// load all genomes from a file
template<class T>
void CircularGenome<T>::loadGenomeFile(string fileName, vector<shared_ptr<AbstractGenome>> &genomes) {
	genomes.clear();
	std::ifstream FILE(fileName);
	string rawLine;
	int _update, _ID, _genomeLength;
	double _alphabetSize;
	double value;
	char rubbish;
	if (FILE.is_open()) {  // if the file named by configFileName can be opened
		getline(FILE, rawLine);  // bypass first line
		while (getline(FILE, rawLine)) {  // keep loading one line from the file at a time into "line" until we get to the end of the file
			std::stringstream ss(rawLine);
//				ss >> target;
//				if (ss.fail()) {
//					return false;
//				} else {
//					string remaining;
//					ss >> remaining;
//					// stream failure means nothing left in stream, which is what we want
//					return ss.fail();
//				}
			ss >> _update >> rubbish >> _ID >> rubbish >> _alphabetSize >> rubbish >> _genomeLength >> rubbish >> rubbish >> rubbish;

			shared_ptr<CircularGenome> newGenome = make_shared<CircularGenome>(_alphabetSize);
			newGenome->sites.clear();
			for (int i = 0; i < _genomeLength; i++) {
				ss >> value >> rubbish;
				newGenome->sites.push_back(value);
			}
			newGenome->dataMap.Set("update", _update);
			newGenome->dataMap.Set("ID", _ID);
			genomes.push_back(newGenome);
		}
	} else {
		cout << "\n\nERROR: In CircularGenome::loadGenomeFile, unable to open file \"" << fileName << "\"\n\nExiting\n" << endl;
		exit(1);
	}


}
// load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
// the undefined action is to take no action
//virtual void loadGenome(string fileName, string key, string keyValue) {
//}

// Translation functions - convert genomes into usefull stuff

// convert a chromosome to a string
template<class T>
string CircularGenome<T>::genomeToStr() {
	string S = "";

	for (int i = 0; i < sites.size(); i++) {
		S = S + FileManager::separator + to_string(sites[i]);
	}

	S.erase(S.begin());  // clip off the leading separator
	S = "\"[" + S + "]\"";
	return S;
}

template<class T>
void CircularGenome<T>::printGenome() {
	cout << "alphabetSize: " << getAlphabetSize() << endl;
	cout << genomeToStr();
	cout << endl;
}

