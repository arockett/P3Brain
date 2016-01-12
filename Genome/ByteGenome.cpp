//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "ByteGenome.h"
#include "../Global.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

int& ByteGenome::initialGenomeSize = Parameters::register_parameter("genomeSizeInitial", 5000, "starting size for genomes", "BYTE_GENOME");
double& ByteGenome::pointMutationRate = Parameters::register_parameter("pointMutationRate", 0.005, "per site mutation rate", "BYTE_GENOME");
double& ByteGenome::insertionRate = Parameters::register_parameter("insertionRate", 0.02, "per genome insertion/deletion rate", "BYTE_GENOME");
double& ByteGenome::deletionRate = Parameters::register_parameter("deletionRate", 0.02, "insertion rate per 1000 genome sites", "BYTE_GENOME");
int& ByteGenome::minGenomeSize = Parameters::register_parameter("genomeSizeMin", 5000, "if the genome is smaller then this, mutations will only increse genome size", "BYTE_GENOME");
int& ByteGenome::maxGenomeSize = Parameters::register_parameter("genomeSizeMax", 20000, "if the genome is larger then this, mutations will only decrease genome size", "BYTE_GENOME");

vector<string> ByteGenome::getStats() {
	vector<string> dataPairs;
	dataPairs.push_back("genomeSize");
	dataPairs.push_back(to_string(sites.size()));
	return (dataPairs);
}

/*
 * converts the sites vector to a FileManager::separator separated list in string format.
 */
string ByteGenome::convert_to_string() {
	string dataString;
	if (sites.size() > 0) {  // convert the genome into a string of int
		for (auto site : sites) {
			dataString += to_string((int) site) + FileManager::separator;
		}
		dataString.pop_back();  // remove extra separator at end
	}
	return dataString;
}

ByteGenome::ByteGenome(shared_ptr<ByteGenome> from) {
	copyGenome(from);
}

// load a genome from file - will look for genome with key keyvalue pair
void ByteGenome::loadGenome(string fileName, string key, string keyValue) {
	cout << "In loadGenome\n";

	map<string, vector<string>> fileContents;

	// load all data from file into fileContents
	fileContents = readFromCSVFile(fileName);

	// if update matters, find genome in this file at this update
	string sites_string = CSVLookUp(fileContents, "key", keyValue, "genome");

	sites.clear();
	vector<int> _sites;
	convertCSVListToVector(sites_string, _sites);
	for (auto site : _sites) {
		sites.push_back((unsigned char) site);
	}
}

void ByteGenome::copyGenome(shared_ptr<ByteGenome> from) {
	sites = from->sites;
}

void ByteGenome::applyMutations(double _pointMutationRate, double _insertionRate, double _deletionRate, int _minGenomeSize, int _maxGenomeSize) {
	if (_pointMutationRate > 0.0) {
		int nucleotides = (int) sites.size();
		int i, s, o, w;
		vector<unsigned char> buffer;
		int localMutations = Random::getBinomial(nucleotides, _pointMutationRate);
		for (i = 0; i < localMutations; i++) {
			sites[Random::getIndex(nucleotides)] = Random::getIndex(256);
		}
		int numInsertions = Random::getBinomial((int) sites.size(), (_insertionRate / 1000));
		while (numInsertions > 0) {
			if ((int) nucleotides < _maxGenomeSize) {
				//duplication
				w = 128 + Random::getIndex(512 - 128);  // w is between 128 and 512 (size of the chunk to be duplicated)
				if (w >= nucleotides) {  // if w is >= the size of the genome, make w smaller!
					w = nucleotides - 1;
				}
				s = Random::getIndex(nucleotides - w);  // s is where to start copying from.
				o = Random::getIndex(nucleotides);  // o is where the chunk will be written
				buffer.clear();
				buffer.insert(buffer.begin(), sites.begin() + s, sites.begin() + s + w);  // put s to (s+w) in buffer
				sites.insert(sites.begin() + o, buffer.begin(), buffer.end());  // insert buffer into genome
			}
			nucleotides = (int) sites.size();
			numInsertions--;
		}

		int numDels = Random::getBinomial(nucleotides, (_deletionRate / 1000));
		while (numDels > 0) {

			if (nucleotides > _minGenomeSize) {
				//deletion
				w = 128 + Random::getIndex(512 - 128);  //  w is between 128 and 255 (size of the chunk to be deleted)
				if (w >= nucleotides) {  // if w is >= the size of the genome, make w smaller!
					w = nucleotides - 1;
				}
				s = Random::getIndex(nucleotides - w);  // s is where to start deleting from.
				sites.erase(sites.begin() + s, sites.begin() + s + w);  // erase everything between s and (s+w)
			}
			nucleotides = (int) sites.size();
			numDels--;
		}
	}
}

void ByteGenome::mutate() {
	applyMutations(pointMutationRate, insertionRate, deletionRate, minGenomeSize, maxGenomeSize);
}

void ByteGenome::fillRandom(int size) {
	sites.resize(size);
	for (size_t i = 0; i < sites.size(); i++) {  // fill al sites with random values 0->255
		sites[i] = (unsigned char) Random::getIndex(256);
		//sites[i] = (unsigned char) 2; // uncomment to test genome with fixed number
	}
//  for (int codon = 42; codon < 50; codon++) {  // place gate start codeons
//    for (int i = 0; i < 4; i++) {
//      int j = Random::getIndex(sites.size() - 2);
//      sites[j] = codon;
//      sites[j + 1] = 256 - codon;
//    }
//  }
}

shared_ptr<Genome> ByteGenome::makeMutatedGenome() {
	shared_ptr<ByteGenome> G = make_shared<ByteGenome>();
	G->sites = sites;
	G->mutate();
	return G;
}

shared_ptr<Genome> ByteGenome::makeMutatedGenome(vector<shared_ptr<ByteGenome>> from) {
	int shortestGenome = from[0]->sites.size();
	for (auto g : from) {
		if ((int) g->sites.size() < shortestGenome) {
			shortestGenome = g->sites.size();
		}
	}
	int numberOfCuts = Random::getInt(10);  // between 0 and 10 cuts
	vector<int> cutLocations;
	for (int i = 0; i < numberOfCuts; i++) {
		cutLocations.push_back(Random::getIndex(shortestGenome));
	}
	cutLocations.push_back(-1);  // add a number at the end to avoid seg fault
	sort(cutLocations.begin(), cutLocations.end());
	shared_ptr<ByteGenome> currSource = from[Random::getIndex(from.size())];
	int currCutIndex = 0;
	vector<unsigned char> newSites;
	for (int i = 0; i < (int) currSource->sites.size(); i++) {
		newSites.push_back(currSource->sites[i]);
		if (i == cutLocations[currCutIndex]) {
			currCutIndex++;
			shared_ptr<ByteGenome> currSource = from[Random::getIndex(from.size())];
		}
	}
	shared_ptr<ByteGenome> G = make_shared<ByteGenome>();  // make a blank genome  G->sites = sites;
	G->sites = newSites;
	G->mutate();
	return G;
}

void ByteGenome::makePointMutation() {
	sites[Random::getIndex(sites.size())] = (unsigned char) Random::getInt(255);
}

int ByteGenome::extractValue(shared_ptr<Index> index, vector<int> valueRange, int code, int gateID) {
	if (valueRange[0] > valueRange[1]) {
		int temp = valueRange[0];
		valueRange[1] = valueRange[0];
		valueRange[0] = temp;
	}
	int currentMax = 256;
	int genomeValue = (int) sites[index->current()[0]];
	assignCodingRegionValue(index, code, gateID);
	index->advance();
	while ((valueRange[1] - valueRange[0] + 1) > currentMax) {  // we don't have enough bits of information
		genomeValue = (genomeValue << 8) & (int) sites[index->current()[0]];  // get 8 more bits
		assignCodingRegionValue(index, code, gateID);
		index->advance();
		currentMax = currentMax * 256;
	}
	return (genomeValue % (valueRange[1] - valueRange[0] + 1)) + valueRange[0];
}

