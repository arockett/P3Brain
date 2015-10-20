#pragma once
/******************************************************************************
* file: AmpersandWorld.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "World.h"

class AmpersandWorld : public World
{
public:
    AmpersandWorld();
    virtual ~AmpersandWorld();

    vector<double> evaluateFitness(vector<Agent*> agents, bool analyse) override;
    double testIndividual(Agent *agent, bool analyse) override;
};

