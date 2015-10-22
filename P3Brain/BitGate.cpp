/******************************************************************************
* file: BitAgent.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "BitGate.h"


BitGate::BitGate( const vector<int>& ins, int id, const vector<bool>& logic ) :
    mGateIndex(id),
    mLogic(logic)
{
    I = ins;
    O.clear();
    O.push_back( mGateIndex );
}


BitGate::~BitGate()
{
}


void BitGate::update( double *states, double *nextStates )
{
    int outIndex = 0;
    if( (bool)states[I[0]] )
    {
        outIndex += 2;
    }
    if( (bool)states[I[1]] )
    {
        ++outIndex;
    }

    nextStates[mGateIndex] = (double)mLogic[outIndex];
}