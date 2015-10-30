/******************************************************************************
* file: BitAgent.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "BitGate.h"


BitGate::BitGate( const vector<int>& ins, int id, const vector<bool>& logic ) :
    gateIndex(id),
    logic(logic)
{
    inputs = ins;
    outputs.clear();
    outputs.push_back( gateIndex );
}


BitGate::~BitGate()
{
}


void BitGate::update( vector<double>& states, vector<double>& nextStates )
{
    // TODO: make this more flexible for different number of inputs
    int outIndex = 0;
    if( (bool)states[inputs[0]] )
    {
        outIndex += 2;
    }
    if( (bool)states[inputs[1]] )
    {
        ++outIndex;
    }

    nextStates[gateIndex] = (double)logic[outIndex];
}