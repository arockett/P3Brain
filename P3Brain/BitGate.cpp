/******************************************************************************
* file: BitAgent.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "BitGate.h"

#include "Tools.h"


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
    // First make sure the size of our logic table is correct
    // for this number of inputs. It should never be incorrect
    // if the BitAgent uses a valid decoder
    ASSERT( logic.size() == (int)pow( 2.0, inputs.size() ),
        "Invalid logic table in gate " << gateIndex <<
        ". This should never happen... check your BitAgent decoder." );

    // the outIndex starts at zero so if all inputs are zero, it won't
    // ever be increased and the first output will be returned since
    // all inputs are zero
    int outIndex = 0;

    // Loop over the inputs and increment the outIndex based on there
    // location starting from the highest place going to the one's place
    //      -> For example:
    //              If there are four inputs and the second input to be
    //              processed is a 1 (true), then add 4 to the outIndex
    //              because it's in the 4's place
    for( unsigned i = 0; i < inputs.size(); i++ )
    {
        if( (bool)states[inputs[i]] )
        {
            outIndex += (int)pow( 2.0, (inputs.size() - ( 1 + i )) );
        }
    }

    nextStates[gateIndex] = (double)logic[outIndex];
}