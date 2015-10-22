/******************************************************************************
* file: AmpersandWorld.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "AmpersandWorld.h"


AmpersandWorld::AmpersandWorld( Logic l ) : logic(l)
{
}


AmpersandWorld::~AmpersandWorld()
{
}


vector<double> AmpersandWorld::evaluateFitness(vector<Agent*> agents, bool analyse) {
    vector<double> fitnesses;
    for (int i = 0; i<agents.size(); i++)
        fitnesses.push_back(testIndividual(agents[i], analyse));
    return fitnesses;
}

double AmpersandWorld::testIndividual(Agent *agent, bool analyse) {
    double fitness = 0.0;

    unsigned input1 = ( logic & 8 ) >> 3;
    unsigned input2 = ( logic & 4 ) >> 2;
    unsigned input3 = ( logic & 2 ) >> 1;
    unsigned input4 = logic & 1;

    /*
     * Test the agent to make them develop an "and" gate
     */
    // 0 0
    agent->resetBrain();
    agent->states[0] = 0.0;
    agent->states[1] = 0.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->states[0] = 0.0;
        agent->states[1] = 0.0;
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == (double)input1 ) fitness += .25;

    // 0 1
    agent->resetBrain();
    agent->states[0] = 0.0;
    agent->states[1] = 1.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->states[0] = 0.0;
        agent->states[1] = 1.0;
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == (double)input2 ) fitness += .25;
    
    // 1 0
    agent->resetBrain();
    agent->states[0] = 1.0;
    agent->states[1] = 0.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->states[0] = 1.0;
        agent->states[1] = 0.0;
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == (double)input3 ) fitness += .25;

    // 1 1
    agent->resetBrain();
    agent->states[0] = 1.0;
    agent->states[1] = 1.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->states[0] = 1.0;
        agent->states[1] = 1.0;
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == (double)input4 ) fitness += .25;

    return fitness;
}
