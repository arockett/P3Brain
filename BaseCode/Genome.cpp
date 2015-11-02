//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

//#include <random>

#include "Genome.h"
#include "Global.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

int Genome::genomeIDCounter = 0;
Genome* Genome::MRCA;

int& Genome::initialGenomeSize = Parameters::register_parameter("genomeSizeInitial", 5000, "starting size for genomes",
		"GENOME");
double& Genome::pointMutationRate = Parameters::register_parameter("pointMutationRate", 0.005, "per site mutation rate",
		"GENOME");
double& Genome::insertionRate = Parameters::register_parameter("insertionRate", 0.02,
		"per genome insertion/deletion rate", "GENOME");
double& Genome::deletionRate = Parameters::register_parameter("deletionRate", 0.02,
		"insertion rate per 1000 genome sites", "GENOME");
int& Genome::minGenomeSize = Parameters::register_parameter("genomeSizeMin", 5000,
		"if the genome is smaller then this, mutations will only increse genome size", "GENOME");
int& Genome::maxGenomeSize = Parameters::register_parameter("GenomeSizeMax", 20000,
		"if the genome is larger then this, mutations will only decrease genome size", "GENOME");

//double Genome::insertionDeletionP=0.02;

Genome::Genome() {
	referenceCounter = 1;
	ancestor = NULL;
	ID = registerGenome();
	birthDate = Global::update;
	dataMap.Set("ID", ID);
	dataMap.Set("birthDate", birthDate);
	dataMap.Set("update", Global::update);
}

Genome::Genome(Genome* from) {
	referenceCounter = 1;
	ancestor = from;
	from->referenceCounter++;
	copyGenome(from);
	ID = registerGenome();
	birthDate = Global::update;
	dataMap.Set("ID", ID);
	dataMap.Set("birthDate", birthDate);
	dataMap.Set("update", Global::update);
}

Genome::~Genome() {
	if (ancestor != NULL) {
		ancestor->kill();
	}
}

int Genome::registerGenome() {
	int I = genomeIDCounter;
	genomeIDCounter++;
	return I;
}

void Genome::copyGenome(Genome* from) {
	Genome* who = from;
	sites.resize(who->sites.size());
	for (size_t i = 0; i < sites.size(); i++)
		sites[i] = who->sites[i];
}

void Genome::applyMutations(double mutationRate) {
	if (mutationRate > 0.0) {
		int nucleotides = (int) sites.size();
		int i, s, o, w;
		vector<unsigned char> buffer;
		int localMutations = Random::getBinomial(nucleotides, mutationRate);
		for (i = 0; i < localMutations; i++) {
			sites[Random::getIndex(nucleotides)] = Random::getIndex(256);
		}
		int numInsertions = Random::getBinomial((int) sites.size(), (Genome::insertionRate / 1000));
		while (numInsertions > 0) {
			if ((int) nucleotides < maxGenomeSize) {
				//duplication
				w = 128 + Random::getIndex(512 - 128);	// w is between 128 and 512 (size of the chunk to be duplicated)
				if (w >= nucleotides) { // if w is >= the size of the genome, make w smaller!
					w = nucleotides - 1;
				}
				s = Random::getIndex(nucleotides - w); // s is where to start copying from.
				o = Random::getIndex(nucleotides); // o is where the chunk will be written
				buffer.clear();
				buffer.insert(buffer.begin(), sites.begin() + s, sites.begin() + s + w); // put s to (s+w) in buffer
				sites.insert(sites.begin() + o, buffer.begin(), buffer.end()); // insert buffer into genome
			}
			nucleotides = (int) sites.size();
			numInsertions--;
		}

		int numDels = Random::getBinomial(nucleotides, (Genome::deletionRate / 1000));
		while (numDels > 0) {

			if (nucleotides > minGenomeSize) {
				//deletion
				w = 128 + Random::getIndex(512 - 128); //  w is between 128 and 255 (size of the chunk to be deleted)
				if (w >= nucleotides) { // if w is >= the size of the genome, make w smaller!
					w = nucleotides - 1;
				}
				s = Random::getIndex(nucleotides - w); // s is where to start deleting from.
				sites.erase(sites.begin() + s, sites.begin() + s + w); // erase everything between s and (s+w)
			}
			nucleotides = (int) sites.size();
			numDels--;
		}
	}
}

void Genome::kill() {
	referenceCounter--;
	if (referenceCounter <= 0) {
		delete this;
	}
}

void Genome::fillRandom() {
	sites.resize(Genome::initialGenomeSize);
	for (size_t i = 0; i < sites.size(); i++) { // fill al sites with random values 0->255
		sites[i] = (unsigned char) Random::getIndex(256);
		//sites[i] = (unsigned char) 2; // uncomment to test genome with fixed number
	}
	for (int codon = 42; codon < 50; codon++) { // place gate start codeons
		for (int i = 0; i < 4; i++) {
			int j = Random::getIndex(sites.size() - 2);
			sites[j] = codon;
			sites[j + 1] = 256 - codon;
		}
	}
}

Genome* Genome::makeMutatedOffspring(double mutationRate) {
	Genome* G = new Genome(this);
	G->applyMutations(mutationRate);
	return G;
}

void Genome::makePointMutation() {
	sites[Random::getIndex(sites.size())] = (unsigned char) Random::getIndex(256);
}

/*
 * Given a genome and a key(to data that has been saved into "dataMap"
 * return a list of the value for key for genome and all genomes ancestors ordered oldest first
 */
vector<string> Genome::GetLODItem(string key) {
	vector<string> list;
	Genome* G = this;
	while (G != NULL) {
		list.insert(list.begin(), G->dataMap.Get(key));
		G = G->ancestor;
	}
	return list;
}

/*
 * Given a genome
 * return a list of genomes containing genome and all genomes ancestors ordered oldest first
 */
vector<Genome*> Genome::getLOD() {
	vector<Genome*> list;
	Genome * G = this;
	while (G != NULL) { // which G has an ancestor
		list.insert(list.begin(), G); // add that ancestor to the front of the LOD list
		G = G->ancestor; // move to the ancestor
	}
	return list;
}

/*
 * find the Most Recent Common Ancestor
 * uses getLOD to get a list of ancestors (oldest first). seaches the list for the first ancestor with a referenceCounter > 1
 * that is the first reference counter with more then one offspring.
 * If none are found, then return "from"
 * Note: a currently active genome has a referenceCounter = 1 (it has not reproduced yet, it only has 1 for it's self)
 *       a dead genome with a referenceCounter = 0 will not be in the LOD (it has no offspring and will have been pruned)
 *       a dead genome with a referenceCounter = 1 has only one offspring.
 *       a dead genome with a referenceCounter > 1 has more then one spring with surviving lines of decent.
 */
Genome* Genome::getMostRecentCommonAncestor() {
	vector<Genome*> LOD = this->getLOD(); // get line of decent from "from"
	for (auto G : LOD) { // starting at the oldest ancestor, moving to the youngest
		if (G->referenceCounter > 1) // the first (oldest) ancestor with more then one surviving offspring is the oldest
			return G;
	}
	return this; // a currently active genome will have referenceCounter = 1 but may be the Most Recent Common Ancestor
}

/*
 * Save the data from dataMap and genomes for one genomes ("from") LOD from the last save until the last convergance
 * this function assumes
 * a) all of the fields in dataMap that genomes will be writting to have been created
 * b) all genomes have the same fields
 * if a) and or b) are not true, the function will still work, but the output will be mislabeled garbage
 *
 * if requireConvergance then data will not be output for times were the LOD has not yet converged
 * else set current agent as MRCA and output all data (used for early termination) - THIS CAN ONLY BE DONE ONE TIME
 * !!!! AFTER requireConvergance = 0 has been run for a file name IT'S filesLastUpdate IS NOT ACCURATE !!!!
 */
void Genome::saveDataOnLOD(int flush) {

	if (Global::files.find("data.csv") == Global::files.end()) { // if file has not be initialized yet
		Global::files[Global::DataFileName] = dataMap.getKeys();
	}


	vector<Genome*> LOD = getLOD(); 		// get line of decent
	Genome* effective_MRCA;
	if (flush) {							// if flush then we don't care about convergance
		effective_MRCA = ancestor; // this assumes that a population was created, but not tested at the end of the evolution loop!
	} else {								// find the convergance point in the LOD.
		effective_MRCA = MRCA;
	}

	while ((effective_MRCA->birthDate >= Global::nextDataWrite) && (Global::nextDataWrite <= Global::updates)) { // if there is convergence before the next data interval
		for (auto file : Global::files) { // for each file in files
			LOD[Global::nextDataWrite - Global::lastPrune]->dataMap.writeToFile(file.first, file.second); // append new data to the file
		}
		Global::nextDataWrite += Global::dataInterval;
	}

	while ((effective_MRCA->birthDate >= Global::nextGenomeWrite) && (Global::nextGenomeWrite <= Global::updates)) { // if there is convergence before the next data interval
		string dataString;
		if (LOD[Global::nextGenomeWrite - Global::lastPrune]->sites.size() > 0) { // convert the genome into a string of int
			for (auto site : LOD[Global::nextGenomeWrite - Global::lastPrune]->sites) {
				dataString += to_string((int) site) + FileManager::separator;
			}
			dataString.pop_back(); // remove extra separator at end
		}
		dataString = to_string(Global::nextGenomeWrite) + FileManager::separator + "\"[" + dataString + "]\""; // add write update and padding to genome string
		FileManager::writeToFile(Global::GenomeFileName, dataString, "update,genome"); // write data to file
		Global::nextGenomeWrite += Global::genomeInterval;
	}

}

void Genome::flushDataOnLOD() {
	cout << "Flushing remaining data to files!\n";
	Genome::saveDataOnLOD(1);
}
