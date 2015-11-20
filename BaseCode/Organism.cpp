//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <algorithm>

#include "Genome.h"
#include "Global.h"
#include "Organism.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

//////////////////////////////////////////
// required code to support abstract class
AbstractOrganism::AbstractOrganism() {
}
AbstractOrganism::~AbstractOrganism() {
}
// required code to support abstract class
//////////////////////////////////////////

/* Organism class (the one we expect to be used most of the time
 * has a genome, a brain, tools for lineage and ancestor tracking (for snapshot data saving method)
 */

int Organism::organismIDCounter = -1; // every organism will get a unique ID

// this is used to hold the most recent common ancestor

/*
 * create an empty organism - it must be filled somewhere else.
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism() {
    genome = nullptr;
    brain = nullptr;
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    genomeAncestors.push_back(ID); // it is it's own Ancestor for genome tracking purposes
    dataAncestors.push_back(ID); // it is it's own Ancestor for data tracking purposes
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
}

/*
 * create a new orginism given only a genome - since we do not know the type of brain we are using, we can not make the brain yet
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<Genome> _genome) {
    genome = _genome;
    brain = nullptr;
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    genomeAncestors.push_back(ID); // it is it's own Ancestor for genome tracking purposes
    dataAncestors.push_back(ID); // it is it's own Ancestor for data tracking purposes
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
    dataMap.SetMany(genome->getStats());
}

Organism::Organism(shared_ptr<Genome> _genome, shared_ptr<Brain> _brain) {
    genome = _genome;
    brain = _brain;
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    genomeAncestors.push_back(ID); // it is it's own Ancestor for genome tracking purposes
    dataAncestors.push_back(ID); // it is it's own Ancestor for data tracking purposes
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
    dataMap.SetMany(genome->getStats());
}

/*
 * create an organism with one parent
 * a brain is created with the assumption that the new brain should be of the same type as the parents brain
 *
 */
Organism::Organism(shared_ptr<Organism> from, shared_ptr<Genome> _genome) {
    genome = _genome;
    brain = from->brain->makeBrainFromGenome(genome);
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    parents.push_back(from); // add this parent to the parents set
    for (auto ancestorID : from->genomeAncestors) {
        genomeAncestors.push_back(ancestorID); // union all parents genomeAncestors into this organisms genomeAncestor set.
    }
    for (auto ancestorID : from->dataAncestors) {
        dataAncestors.push_back(ancestorID); // union all parents dataAncestors into this organisms dataAncestor set.
    }
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
    dataMap.SetMany(genome->getStats());
    dataMap.SetMany(brain->getStats());
}

/*
 * create an organism that has more than one parent
 * in this case the parent pointer is not used, and ancestor* sets are used instead to track lineage
 * a brain is created with the assumption that:
 * a) all organisms in from are the same
 * b) and the new brain should be of the same type as the parents brain
 */
Organism::Organism(const vector<shared_ptr<Organism>> from, shared_ptr<Genome> _genome) {
    genome = _genome;
    brain = from[0]->brain->makeBrainFromGenome(genome); // for now, just return parent[0] brain.
    for (auto i = 0; i < (int) from.size(); i++) {
        parents.push_back(from[i]); // add this parent to the parents set
        for (auto ancestorID : from[i]->genomeAncestors) {
            genomeAncestors.push_back(ancestorID); // union all parents genomeAncestors into this organisms genomeAncestor set.
        }
        for (auto ancestorID : from[i]->dataAncestors) {
            dataAncestors.push_back(ancestorID); // union all parents dataAncestors into this organisms dataAncestor set.
        }
    }
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
    dataMap.SetMany(genome->getStats());
    dataMap.SetMany(brain->getStats());
}

Organism::~Organism() {
    cout << ID << "\n";
}

int Organism::registerOrganism() {
    return organismIDCounter++;;
}

/*
 * called to kill an organism. Set alive to false
 */
void Organism::kill() {
    alive = false;
}

shared_ptr<Organism> Organism::makeMutatedOffspring(double pointMutationRate) {
    shared_ptr<Organism> parent1(this);
    shared_ptr<Organism> newOrg = make_shared<Organism>(parent1, genome->makeMutatedGenome(Genome::pointMutationRate));
    cout << "new offspring : " << newOrg->ID << " from parent: " << ID << "\n";
    return newOrg;
}

shared_ptr<Organism> Organism::makeMutatedOffspring(double pointMutationRate, shared_ptr<Organism> parent2) {
    shared_ptr<Organism> parent1(this);
    vector<shared_ptr<Organism>> _parents = { parent1, parent2 };
    shared_ptr<Organism> newOrg(new Organism(_parents, genome->makeMutatedGenome(Genome::pointMutationRate)));
    newOrg->gender = (Random::getInt(0, 1)) ? parent1->gender : parent2->gender; // assign a random gender to the new org
    return newOrg;
}

/*
 * Given a genome and a key(to data that has been saved into "dataMap"
 * return a list of the value for key for genome and all genomes ancestors ordered oldest first
 */
vector<string> Organism::GetLODItem(string key) {
    vector<string> list;
    shared_ptr<Organism> org(this);
    list.insert(list.begin(), org->dataMap.Get(key)); // add this organisms data to the front of the list
    while (org->parents.size() == 1) { // while the current org has one and only one parent
        org = org->parents[0]; // move to the next ancestor (since there is only one parent it is the element in the first position).
        list.insert(list.begin(), org->dataMap.Get(key)); // add that ancestors data to the front of the list
    }
    if (org->parents.size() > 1) { // if more than one parent we have a problem!
        cout << "  In Organism::GetLODItem :: ERROR! an Organism has more than one parent! Can not establish LOD (do not use getLOD with Sexual Populations).\n  If you are using LODwAP, try using SSwD instead.\n  Exiting...\n";
        exit(1);
    }
    return list;
}

/*
 * Given a genome return a list of Organisms containing this Organism and all if this Organisms ancestors ordered oldest first
 * it will fail if any organism in the LOD has more then one parent. (!not for sexual reproduction!)
 */
vector<shared_ptr<Organism>> Organism::getLOD() {
    vector<shared_ptr<Organism>> list;
    shared_ptr<Organism> org(this);
    list.insert(list.begin(), org); // add this organism to the front of the LOD list
    while (org->parents.size() == 1) { // while the current org has one and only one parent
        org = org->parents[0]; // move to the next ancestor (since there is only one parent it is the element in the first position).
        list.insert(list.begin(), org); // add that ancestor to the front of the LOD list
    }
    if (org->parents.size() > 1) { // if more than one parent we have a problem!
        cout << "  In Organism::getLOD :: ERROR! an Organism has more than one parent! Can not establish LOD (do not use getLOD with Sexual Populations).\n  If you are using LODwAP, try using SSwD instead.\n  Exiting...\n";
        exit(1);
    }
    return list;
}

/*
 * find the Most Recent Common Ancestor
 * uses getLOD to get a list of ancestors (oldest first). seaches the list for the first ancestor with a referenceCounter > 1
 * that is the first reference counter with more then one offspring.
 * If none are found, then return "from"
 * Note: a currently active genome has a referenceCounter = 1 (it has not reproduced yet, it only has 1 for it's self)
 *       a dead Organism with a referenceCounter = 0 will not be in the LOD (it has no offspring and will have been pruned)
 *       a dead Organism with a referenceCounter = 1 has only one offspring.
 *       a dead Organism with a referenceCounter > 1 has more then one spring with surviving lines of decent.
 */
shared_ptr<Organism> Organism::getMostRecentCommonAncestor() {
    vector<shared_ptr<Organism>> LOD = getLOD(); // get line of decent parent "parent"
    for (auto org : LOD) { // starting at the oldest parent, moving to the youngest
        if (org.use_count() > 1) // the first (oldest) ancestor with more then one surviving offspring is the oldest
            return org;
    }
    shared_ptr<Organism> _this(this);
    return _this; // a currently active genome will have referenceCounter = 1 but may be the Most Recent Common Ancestor
}

/*
 * Save the data from dataMap and genomes for one genomes ("from") LOD from the last save until the last convergance
 * this function assumes
 * a) all of the fields in dataMap that genomes will be writting to have been created
 * b) all genomes have the same fields
 * if a) and or b) are not true, the function will still work, but the output will be mislabeled garbage
 *
 * if requireConvergance then data will not be output for times were the LOD has not yet converged
 * else set current brain as MRCA and output all data (used for early termination) - THIS CAN ONLY BE DONE ONE TIME
 * !!!! AFTER requireConvergance = 0 has been run for a file name IT'S filesLastUpdate IS NOT ACCURATE !!!!
 */
//void Organism::saveDataOnLOD(int flush) {
//
//    if (Global::files.find("data.csv") == Global::files.end()) { // if file has not be initialized yet
//        Global::files[Global::DataFileName] = dataMap.getKeys();
//        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//        // this may need to be looked at once dataMap and snapshot have been addressed!
//        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    }
//
//    vector<Organism*> LOD = getLOD(); 		// get line of decent
//    Organism* effective_MRCA;
//    if (flush) {				// if flush then we don't care about convergance
//        effective_MRCA = *parents.begin(); // this assumes that a population was created, but not tested at the end of the evolution loop!
//    } else {						// find the convergance point in the LOD.
//        effective_MRCA = MRCA;
//    }
//
//    while ((effective_MRCA->birthDate >= Global::nextDataWrite) && (Global::nextDataWrite <= Global::updates)) { // if there is convergence before the next data interval
//        Organism* current = LOD[Global::nextDataWrite - Global::lastPrune];
//        for (auto file : Global::files) { // for each file in files
//            current->dataMap.writeToFile(file.first, file.second); // append new data to the file
//            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//            // this will need to be looked at once dataMap and snapshot have been addressed!
//            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//        }
//        Global::nextDataWrite += Global::dataInterval;
//    }
//
//    while ((effective_MRCA->birthDate >= Global::nextGenomeWrite) && (Global::nextGenomeWrite <= Global::updates)) { // if there is convergence before the next data interval
//        Organism* current = LOD[Global::nextGenomeWrite - Global::lastPrune];
//        string dataString;
//        if (current->genome->sites.size() > 0) { // convert the genome into a string of int
//            for (auto site : current->genome->sites) {
//                dataString += to_string((int) site) + FileManager::separator;
//            }
//            dataString.pop_back(); // remove extra separator at end
//        }
//        dataString = to_string(Global::nextGenomeWrite) + FileManager::separator + "\"[" + dataString + "]\""; // add write update and padding to genome string
//        FileManager::writeToFile(Global::GenomeFileName, dataString, "update,genome"); // write data to file
//        Global::nextGenomeWrite += Global::genomeInterval;
//    }
//
//}
//void Organism::flushDataOnLOD() {
//    cout << "Flushing remaining data to files!\n";
//    saveDataOnLOD(1);
//}
