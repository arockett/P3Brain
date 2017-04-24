/******************************************************************************
* file: LogicalWorld.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "LogicalWorld.h"

#include "Tools.h"


LogicalWorld::LogicalWorld()
{
    logic = {
        LogicalWorld::Logic::AND,
        LogicalWorld::Logic::XOR,
        LogicalWorld::Logic::NXOR
    };

    fitnessIncrement = 1.0 / 4 / logic.size();
}


LogicalWorld::~LogicalWorld()
{
}

void LogicalWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug)
{
    org->score = 0.0;
    auto &brain = org->brain;
    // If there aren't enough non-input nodes to check then return 0 fitness
    // because the bit string is not long enough
    if( (int)logic.size() > brain->nrOfBrainNodes - 2 )
    {
        return;
    }

    double fitness = 0.0;

    /*
     * Test the brain with each combination of two bit binary inputs
     */
    // 0 0
    brain->resetBrain();
    brain->nodes[0] = 0.0;
    brain->nodes[1] = 0.0;
    for( int i = 0; i < brain->nrOfBrainNodes - 2; i++ )
    {
        brain->nodes[0] = 0.0;
        brain->nodes[1] = 0.0;
        brain->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( brain->nodes[brain->nrOfBrainNodes - ( logic.size() - i )] == (double)getBit( logic[i], 3 ) )
            fitness += fitnessIncrement;
    }

    // 0 1
    brain->resetBrain();
    brain->nodes[0] = 0.0;
    brain->nodes[1] = 1.0;
    for( int i = 0; i < brain->nrOfBrainNodes - 2; i++ )
    {
        brain->nodes[0] = 0.0;
        brain->nodes[1] = 1.0;
        brain->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( brain->nodes[brain->nrOfBrainNodes - ( logic.size() - i )] == (double)getBit( logic[i], 2 ) )
            fitness += fitnessIncrement;
    }

    // 1 0
    brain->resetBrain();
    brain->nodes[0] = 1.0;
    brain->nodes[1] = 0.0;
    for( int i = 0; i < brain->nrOfBrainNodes - 2; i++ )
    {
        brain->nodes[0] = 1.0;
        brain->nodes[1] = 0.0;
        brain->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( brain->nodes[brain->nrOfBrainNodes - ( logic.size() - i )] == (double)getBit( logic[i], 1 ) )
            fitness += fitnessIncrement;
    }

    // 1 1
    brain->resetBrain();
    brain->nodes[0] = 1.0;
    brain->nodes[1] = 1.0;
    for( int i = 0; i < brain->nrOfBrainNodes - 2; i++ )
    {
        brain->nodes[0] = 1.0;
        brain->nodes[1] = 1.0;
        brain->update();
    }
    for( unsigned i = 0; i < logic.size(); i++ )
    {
        if( brain->nodes[brain->nrOfBrainNodes - ( logic.size() - i )] == (double)getBit( logic[i], 0 ) )
            fitness += fitnessIncrement;
    }

    org->score = fitness;
}
