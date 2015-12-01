/******************************************************************************
* file: LogicalWorld.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "LogicalWorld.h"

#include "Tools.h"


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

double LogicalWorld::testIndividual(shared_ptr<Organism> organism, bool analyse) {
    auto& agent = organism->brain;
    // If there aren't enough non-input nodes to check then return 0 fitness
    // because the bit string is not long enough
    if( (int)logic.size() > agent->nrOfBrainStates - 2 )
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
        agent->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)getBit( logic[i], 3 ) )
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
        agent->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)getBit( logic[i], 2 ) )
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
        agent->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)getBit( logic[i], 1 ) )
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
        agent->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( agent->states[agent->nrOfBrainStates - ( logic.size() - i )] == (double)getBit( logic[i], 0 ) )
            fitness += fitnessIncrement;
    }

    return fitness;
}
