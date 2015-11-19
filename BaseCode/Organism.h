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

#include "Brain.h"
#include "Genome.h"

#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

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

    Genome* genome;
    Brain* brain;

    double score;

    set<Organism*> parents; // parents are pointers to parents of this organism. In asexual populations this will have one element
    set<int> genomeAncestors; // list of the IDs of organisms in the last genome file who are ancestors of this organism (genomes saved on genome interval)
    set<int> dataAncestors; // list of the IDs of organisms in the last data files who are ancestors of this organism (i.e. all files saved on data interval)

    int ID;
    int timeOfBirth; // the time this organism was made
    int timeOfDeath; // the time this organism stopped being alive (this organism may be stored for archival reasons)

    bool alive; // is this organism alive (1) or dead (0)
    int referenceCounter; // how many offspring organisms are looking at this organism (including this organism while it's alive)
    DataMap dataMap; // holds all data (genome size, score, world data, etc.)
    map<int, DataMap> snapShotDataMaps; // Used only with SnapShot with Delay (SSwD) stores contents of dataMap when an ouput interval is reached so that
                                        // after the delay we have the correct data for the given time. key is 'update'.
    Organism(); // make an empty organism
    Organism(Genome* _genome); // make a parentless organism with a genome, and a nullptr to brain
    Organism(Genome* _genome, Brain* _brain); // make a parentless organism with a genome, and a brain
    Organism(Organism* from, Genome* _genome); // make an organism with one parent, a genome and a brain determined from the parents brain type.
    Organism(const vector<Organism*>& from, Genome* _genome); // make a organism with many parents, a genome, and a brain determined from the parents brain type.

    virtual ~Organism();

    virtual void unFollow(); // this decrements referenceCounter and if referenceCounter = 0 & !alive, deletes this
    virtual void addFollow(); // this increments referenceCounter

    virtual void kill(); // sets alive = 0 (on org and in dataMap) and if referenceCounter = 0, deletes this

    virtual vector<string> GetLODItem(string key);
    vector<Organism*> getLOD();
    virtual Organism* getMostRecentCommonAncestor();
    virtual Organism* makeMutatedOffspring(double pointMutationRate);
    virtual Organism* makeMutatedOffspring(double pointMutationRate, Organism* parent2);
};

#endif /* defined(__BasicMarkovBrainTemplate__Organism__) */

