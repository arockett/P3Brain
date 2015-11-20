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
    referenceCount = 1; // because it's alive;
    dataCount = 0;
    genomeAncestors.push_back(ID); // it is it's own Ancestor for genome tracking purposes
    dataAncestors.push_back(ID); // it is it's own Ancestor for data tracking purposes
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
}

/*
 * create a new organism given only a genome - since we do not know the type of brain we are using, we can not make the brain yet
 * parents is left empty (this is organism has no parents!)
 */
Organism::Organism(shared_ptr<Genome> _genome) {
    genome = _genome;
    brain = nullptr;
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    referenceCount = 1; // because it's alive;
    dataCount = 0;
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
    dataCount = 0;
    referenceCount = 1; // because it's alive;
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
 */
Organism::Organism(shared_ptr<Organism> from, shared_ptr<Genome> _genome) {
    genome = _genome;
    brain = from->brain->makeBrainFromGenome(genome);
    ID = registerOrganism();
    alive = true;
    gender = 0; // by default all orgs are female.
    referenceCount = 1; // because it's alive;
    dataCount = 0;
    parents.push_back(from);
    from->referenceCount++; // this parent has an(other) offspring
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
    brain = from[0]->brain->makeBrainFromGenome(genome);
    ID = registerOrganism();
    alive = true;
    referenceCount = 1; // because it's alive;
    dataCount = 0;
    gender = 0; // by default all orgs are female.
    for (auto parent : from) {
        parents.push_back(parent); // add this parent to the parents set
        parent->referenceCount++; // this parent has an(other) offspring
        for (auto ancestorID : parent->genomeAncestors) {
            genomeAncestors.push_back(ancestorID); // union all parents genomeAncestors into this organisms genomeAncestor set.
        }
        for (auto ancestorID : parent->dataAncestors) {
            dataAncestors.push_back(ancestorID); // union all parents dataAncestors into this organisms dataAncestor set.
        }
    }
    timeOfBirth = Global::update; // happy birthday!
    timeOfDeath = -1; // still alive
    dataMap.Set("ID", ID);
    dataMap.Set("alive", alive);
    dataMap.Set("timeOfBirth", timeOfBirth);
    dataMap.SetMany(genome->getStats());
    dataMap.SetMany(brain->getStats());
}

int Organism::registerOrganism() {
    return organismIDCounter++;;
}

Organism::~Organism() {
    cout << "destruct: " << ID << "   with ref: " << referenceCount << "   and dataCount: " << dataCount << "\n";
    for (auto parent : parents) {
        parent->referenceCount--; // this parent has one less child in memory
    }
    parents.clear();
}

/*
 * called to kill an organism. Set alive to false
 */
void Organism::kill() {
    cout << "killing ID: " << ID << "\n";
    alive = false;
    referenceCount--; // remove reference for self
}

shared_ptr<Organism> Organism::makeMutatedOffspring(double pointMutationRate, shared_ptr<Organism> parent) {
    shared_ptr<Organism> newOrg = make_shared<Organism>(parent, genome->makeMutatedGenome(Genome::pointMutationRate));
    cout << "new offspring : " << newOrg->ID << " from parent: " << ID << "\n";
    return newOrg;
}

shared_ptr<Organism> Organism::makeMutatedOffspring(double pointMutationRate, shared_ptr<Organism> parent1, shared_ptr<Organism> parent2) {
    vector<shared_ptr<Organism>> _parents = { parent1, parent2 };
    shared_ptr<Organism> newOrg = make_shared<Organism>(_parents, genome->makeMutatedGenome(Genome::pointMutationRate));
    newOrg->gender = (Random::getInt(0, 1)) ? parent1->gender : parent2->gender; // assign a random gender to the new org
    return newOrg;
}

/*
 * Given a genome and a key(to data that has been saved into "dataMap"
 * return a list of the value for key for genome and all genomes ancestors ordered oldest first
 */
vector<string> Organism::GetLODItem(string key, shared_ptr<Organism> org) {
    vector<string> list;
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
vector<shared_ptr<Organism>> Organism::getLOD(shared_ptr<Organism> org) {
    vector<shared_ptr<Organism>> list;

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
shared_ptr<Organism> Organism::getMostRecentCommonAncestor(shared_ptr<Organism> org) {
    vector<shared_ptr<Organism>> LOD = getLOD(org); // get line of decent parent "parent"
    for (auto org : LOD) { // starting at the oldest parent, moving to the youngest
        if (org->referenceCount > 1) // the first (oldest) ancestor with more then one surviving offspring
            return org;
    }
    return org; // a currently active genome will have referenceCounter = 1 but may be the Most Recent Common Ancestor
}
