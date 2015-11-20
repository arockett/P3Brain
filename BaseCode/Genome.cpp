//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Genome.h"
#include "Global.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

int& Genome::initialGenomeSize = Parameters::register_parameter("genomeSizeInitial", 5000, "starting size for genomes", "GENOME");
double& Genome::pointMutationRate = Parameters::register_parameter("pointMutationRate", 0.005, "per site mutation rate", "GENOME");
double& Genome::insertionRate = Parameters::register_parameter("insertionRate", 0.02, "per genome insertion/deletion rate", "GENOME");
double& Genome::deletionRate = Parameters::register_parameter("deletionRate", 0.02, "insertion rate per 1000 genome sites", "GENOME");
int& Genome::minGenomeSize = Parameters::register_parameter("genomeSizeMin", 5000, "if the genome is smaller then this, mutations will only increse genome size", "GENOME");
int& Genome::maxGenomeSize = Parameters::register_parameter("GenomeSizeMax", 20000, "if the genome is larger then this, mutations will only decrease genome size", "GENOME");

//////////////////////////////////////////
// required code to support abstract class
//double Genome::insertionDeletionP=0.02;

//vector<string> AbstactGenome::getStats(){
//    vector<string> dataPairs;
//    return(dataPairs);
//}
// required code to support abstract class
//////////////////////////////////////////

vector<string> Genome::getStats() {
    vector<string> dataPairs;
    dataPairs.push_back("genomeSize");
    dataPairs.push_back(to_string(sites.size()));
    return (dataPairs);
}

/*
 * converts the sites vector to a FileManager::separator separated list in string format.
 */
string Genome::convert_to_string() {
    string dataString;
    if (sites.size() > 0) { // convert the genome into a string of int
        for (auto site : sites) {
            dataString += to_string((int) site) + FileManager::separator;
        }
        dataString.pop_back(); // remove extra separator at end
    }
    return dataString;
}

Genome::Genome(shared_ptr<Genome> from) {
    copyGenome(from);
}

void Genome::copyGenome(shared_ptr<Genome> from) {
    shared_ptr<Genome> who = from;
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
                w = 128 + Random::getIndex(512 - 128); // w is between 128 and 512 (size of the chunk to be duplicated)
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

shared_ptr<Genome> Genome::makeMutatedGenome(double mutationRate) {
    shared_ptr<Genome> G(new Genome(shared_from_this()));
    G->applyMutations(mutationRate);
    return G;
}

void Genome::makePointMutation() {
    sites[Random::getIndex(sites.size())] = (unsigned char) Random::getIndex(256);
}
