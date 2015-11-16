//
//  Optimizer.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Optimizer__
#define __BasicMarkovBrainTemplate__Optimizer__

#include <iostream>
#include <stdlib.h>
#include <vector>

#include "Genome.h"
#include "Organism.h"

#include "../Utilities/Parameters.h"

using namespace std;

class Optimizer {
public:
    static int& elitism;
    static int& tournamentSize;

public:
    double maxFitness;
    Optimizer() = default;
    virtual ~Optimizer() = default;
    virtual void makeNextGeneration(vector<Organism*> &population);
};

class GA: Optimizer {
public:
    virtual void makeNextGeneration(vector<Organism*> &population);
};

class P3: Optimizer {

};

class Tournament: Optimizer {
public:
    virtual void makeNextGeneration(vector<Organism*> &population);
};

class Tournament2: Optimizer {
public:
    virtual void makeNextGeneration(vector<Organism*> &population);
};

#endif /* defined(__BasicMarkovBrainTemplate__Optimizer__) */
