/******************************************************************************
* file: LogicalWorld.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "LogicalWorld.h"

unsigned input1( unsigned );
unsigned input2( unsigned );
unsigned input3( unsigned );
unsigned input4( unsigned );


LogicalWorld::LogicalWorld( const vector<Logic>& outputLogics )
{
    if( outputLogics.size() > 0 )
    {
        for( auto l : outputLogics )
        {
            logic.push_back( l );
        }
    }
    else
    {
        logic.push_back( AND );
    }

    fitnessIncrement = 1.0 / 4 / logic.size();
}


LogicalWorld::~LogicalWorld()
{
}


vector<double> LogicalWorld::evaluateFitness(vector<Agent*> agents, bool analyse) {
    vector<double> fitnesses;
    for (int i = 0; i<agents.size(); i++)
        fitnesses.push_back(testIndividual(agents[i], analyse));
    return fitnesses;
}

double LogicalWorld::testIndividual(Agent *agent, bool analyse) {
    // If there aren't enough non-input nodes to check then return 0 fitness
    // because the bit string is not long enough
    if( logic.size() > agent->nrOfBrainStates - 2 )
        return 0.0;

    double fitness = 0.0;

    /*
     * Test the agent to make them develop an "and" gate
     */
    // 0 0
    agent->resetBrain();
    agent->states[0] = 0.0;
    agent->states[1] = 0.0;
    for( int i = 0; i < agent->nrOfBrainStates - 2; i++ )
    {
        agent->states[0] = 0.0;
        agent->states[1] = 0.0;
        agent->updateStates();
    }
    for( int i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - (logic.size() - i)] == (double)input1(logic[i]) )
            fitness += fitnessIncrement;
    }

    // 0 1
    agent->resetBrain();
    agent->states[0] = 0.0;
    agent->states[1] = 1.0;
    for( int i = 0; i < agent->nrOfBrainStates - 2; i++ )
    {
        agent->states[0] = 0.0;
        agent->states[1] = 1.0;
        agent->updateStates();
    }
    for( int i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)input2( logic[i] ) )
            fitness += fitnessIncrement;
    }
    
    // 1 0
    agent->resetBrain();
    agent->states[0] = 1.0;
    agent->states[1] = 0.0;
    for( int i = 0; i < agent->nrOfBrainStates - 2; i++ )
    {
        agent->states[0] = 1.0;
        agent->states[1] = 0.0;
        agent->updateStates();
    }
    for( int i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)input3( logic[i] ) )
            fitness += fitnessIncrement;
    }

    // 1 1
    agent->resetBrain();
    agent->states[0] = 1.0;
    agent->states[1] = 1.0;
    for( int i = 0; i < agent->nrOfBrainStates - 2; i++ )
    {
        agent->states[0] = 1.0;
        agent->states[1] = 1.0;
        agent->updateStates();
    }
    for( int i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)input4( logic[i] ) )
            fitness += fitnessIncrement;
    }

    return fitness;
}

/*
 * Some utility functions
 */
unsigned input1( unsigned l )
{
    return ( l & 8 ) >> 3;
}

unsigned input2( unsigned l )
{
    return ( l & 4 ) >> 2;
}

unsigned input3( unsigned l )
{
    return ( l & 2 ) >> 1;
}

unsigned input4( unsigned l )
{
    return l & 1;
}
