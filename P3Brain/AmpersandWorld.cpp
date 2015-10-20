/******************************************************************************
* file: AmpersandWorld.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "AmpersandWorld.h"


AmpersandWorld::AmpersandWorld()
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
    agent->resetBrain();

    /*
     * Test the agent to make them develop an "and" gate
     */
    // 0 0
    agent->states[0] = 0.0;
    agent->states[1] = 0.0;
    for( int i = 0; i < agent->nrOfStates-2; i++ )
    {
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == 0.0 ) fitness += .25;

    // 0 1
    agent->states[0] = 0.0;
    agent->states[1] = 1.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == 0.0 ) fitness += .25;
    
    // 1 0
    agent->states[0] = 1.0;
    agent->states[1] = 0.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] == 0.0 ) fitness += .25;

    // 1 1
    agent->states[0] = 1.0;
    agent->states[1] = 1.0;
    for( int i = 0; i < agent->nrOfStates - 2; i++ )
    {
        agent->updateStates();
    }
    if( agent->states[agent->nrOfStates - 1] != 0.0 ) fitness += .25;

    return fitness;
}
