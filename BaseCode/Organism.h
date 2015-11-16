//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Organism__
#define __BasicMarkovBrainTemplate__Organism__

#include <stdlib.h>
#include <vector>
#include "Genome.h"

#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"
#include "Brain.h"

using namespace std;

class AbstractOrganism {
public:
    AbstractOrganism();
    virtual ~AbstractOrganism() = 0;
};

class Organism: AbstractOrganism {
private:
    static int organismIDCounter; // used to issue unique ids to Genomes
    int registerOrganism(); // get an Organism_id (uses organismIDCounter)

public:
    static Organism* MRCA;
    int gender; // generally 0=female, 1=male, *=?

public:
    Genome* genome;
    Brain* brain;

    double score;

    set<Organism*> parents; // parents are pointers to parents of this organism. In asexual populations this will have one element
    set<int> genomeAncestors; // list of the IDs of organisms in the last genome file who are ancestors of this organism (genomes saved on genome interval)
    set<int> dataAncestors; // list of the IDs of organisms in the last data files who are ancestors of this organism (i.e. all files saved on data interval)

    int ID;
    int birthDate; // the time this organism was made
    bool alive; // is this organism alive (1) or dead (0)
    int referenceCounter; // how many living organisms are looking at this organism (including this organism while it's alive)
    DataMap dataMap; // holds all data (genome size, score, world data, etc.)
    map<int, DataMap> checkPointDataMap; // Used only with SnapShot with Delay (SSwD) stores contents of dataMap when an ouput interval is reached so that
                                         // after the delay we have the correct data for the given time. key is 'update'.
    Organism(); // make an empty organism
    Organism(Genome* _genome); // make a parentless organism with a genome, and a nullptr to brain
    Organism(Genome* _genome, Brain* _brain); // make a parentless organism with a genome, and a brain
    Organism(Organism* from, Genome* _genome); // make an organism with one parent, a genome and a brain determined from the parents brain type.
    Organism(const vector<Organism*>& from, Genome* _genome); // make a organism with many parents, a genome, and a brain determined from the parents brain type.

    virtual ~Organism();

    virtual void unFollow(); // this decrements referenceCounter and if referenceCounter = 0, deletes this
    virtual void addFollow(); // this increments referenceCounter

    virtual void kill(); // sets alive = 0 (on org and in dataMap) and calls deReference()

    virtual vector<string> GetLODItem(string key);
    vector<Organism*> getLOD();
    virtual Organism* getMostRecentCommonAncestor();
    virtual Organism* makeMutatedOffspring(double pointMutationRate);
    virtual Organism* makeMutatedOffspring(double pointMutationRate, Organism* parent2);

    virtual void saveDataOnLOD(int flush = 0); // for Organism, save to file data for this genome and it's LOD
    virtual void flushDataOnLOD(); // used at the end of a run to save data newer then the MRCA / convergance point
};

#endif /* defined(__BasicMarkovBrainTemplate__Organism__) */

