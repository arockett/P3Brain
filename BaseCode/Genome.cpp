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
		//WAS default_random_engine generator;
		//WAS binomial_distribution<int> distribution(nucleotides,mutationRate);
		//WAS int localMutations= distribution(generator);
		int localMutations = Random::getBinomial(nucleotides, mutationRate);
		for (i = 0; i < localMutations; i++) {
			//WAS sites[rand() % nucleotides] = rand() & 255;
			sites[Random::getIndex(nucleotides)] = Random::getIndex(256);
		}
		//if ((((double) rand() / (double) RAND_MAX) < Genome::insertionDeletionP) && (sites.size() < MaxGenomeSize)) {
		int numInsertions = Random::getBinomial((int) sites.size(), (Genome::insertionRate / 1000));
		while (numInsertions > 0) {
			//if (Random::getBinomial((int) sites.size(),Genome::insertionDeletionP/1000) && ((int) sites.size() < 20000)) {
			if ((int) nucleotides < maxGenomeSize) {
				//duplication
				//WAS w = 128 + rand() % (512 - 128);
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

			//if (Random::P(Genome::insertionDeletionP) && (nucleotides > 1000)) {
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
		//WAS sites[i] = (unsigned char) rand() & 255;
		sites[i] = (unsigned char) Random::getIndex(256);
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

void Genome::saveToFile(FILE *F) {
	for (size_t i = 0; i < sites.size(); i++)
		fprintf(F, "	%i", sites[i]);
	fprintf(F, "\n");
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
		//printf("IN getLOD - %i %i\n",G->ID,G->referenceCounter);
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
		//printf("IN getMRCA - %i %i\n",G->ID,G->referenceCounter);
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
void Genome::saveDataOnLOD(string fileName, int flush) {

	if (FileTracker::exists(fileName) == false) { // if file has not be initialized yet
		cout << "  saveDataOnLOD() :: \"" << fileName << "\" was not initialized. I better do that now!!";
		FileTracker::initFile(fileName, dataMap.getKeys(), Global::dataInterval);
	}

	vector<Genome*> LOD = getLOD(); 		// get line of decent
	Genome* effective_MRCA;
	if (flush) {							// if flush then we don't care about convergance
		effective_MRCA = this->ancestor; 	// this assumes that a population was created, but not tested at the end of the evolution loop!
	} else {								// find the convergance point in the LOD.
		effective_MRCA = MRCA;
	}

	int outputTime = 0; // this update, relative to the last save update, I.e. oldest genome on LOD is now LOD[0]

	// while we have not caught up with the current update
	// and we have not passed the MRCA
	// and we have not passed Global::updates (how long we wanted this to run)
	while ((outputTime + FileTracker::filesLastUpdate[fileName] <= Global::update) && (LOD[outputTime] != effective_MRCA)
			&& (outputTime + FileTracker::filesLastUpdate[fileName] <= Global::updates)) {
		if ((outputTime + FileTracker::filesLastUpdate[fileName]) == FileTracker::filesNextUpdate[fileName]) {
			// write data to file if this is a dataInterval
			LOD[outputTime]->dataMap.writeToFile(fileName, FileTracker::fileColumns[fileName]);
			FileTracker::filesNextUpdate[fileName] = nextInStepList(FileTracker::fileUpdateIntervals[fileName],
					outputTime + FileTracker::filesLastUpdate[fileName]);
		}
		outputTime++;
	}
	FileTracker::filesLastUpdate[fileName] += outputTime;
}

void Genome::flushDataOnLOD(string fileName) {
	Genome::saveDataOnLOD(fileName, 1);
}
