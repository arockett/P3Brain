//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Genome.h"
#include "Random.h"
#include "Data.h" // DELETE ME : HERE FOR klyphcount
#include <random>

int Genome::genomeIDCounter = 0;

int& Genome::initialGenomeSize = Parameters::register_parameter("genomeSizeInitial", 5000,
		"starting size for genomes", "GENOME");
double& Genome::pointMutationRate = Parameters::register_parameter("pointMutationRate", 0.005, "per site mutation rate",
		"GENOME");
double& Genome::insertionRate = Parameters::register_parameter("insertionRate", 0.02,
		"per genome insertion/deletion rate", "GENOME");
double& Genome::deletionRate = Parameters::register_parameter("deletionRate", 0.02,
		"insertion rate per 1000 genome sites", "GENOME");
int& Genome::minGenomeSize = Parameters::register_parameter("genomeSizeMin", 5000, "if the genome is smaller then this, mutations will only increse genome size",
		"GENOME");
int& Genome::maxGenomeSize = Parameters::register_parameter("GenomeSizeMax", 20000,
		"if the genome is larger then this, mutations will only decrease genome size", "GENOME");

//double Genome::insertionDeletionP=0.02;

Genome::Genome() {
	referenceCounter = 1;
	ancestor = NULL;
	ID = registerGenome();
	data["ID"] = to_string(ID);
}

Genome::Genome(Genome* from) {
	referenceCounter = 1;
	ancestor = from;
	from->referenceCounter++;
	copyGenome(from);
	ID = registerGenome();
	data["ID"] = to_string(ID);
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
