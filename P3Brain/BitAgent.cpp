/******************************************************************************
* file: BitAgent.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "BitAgent.h"

#include <cmath>
#include <cassert>
#include "BitGate.h"


BitAgent::BitAgent()
{
}

BitAgent::BitAgent( const vector<bool>& startGenome, int numInputStates, BitAgent::Decoder decoder, int gateComplexity, int cubeDimension)
{
    switch( decoder )
    {
    case FixedInput:
        DecodeFixedInputGenome( startGenome, numInputStates, gateComplexity );
        break;
    case FixedLogic:
        DecodeFixedLogicGenome( startGenome, numInputStates, gateComplexity );
        break;
    case Hypercube:
        DecodeHypercubeGenome( startGenome, numInputStates, gateComplexity, cubeDimension );
        break;
    }
}

BitAgent::~BitAgent()
{
}


/*
 *************************   Decoders ****************************
 */

void BitAgent::DecodeFixedInputGenome( const vector<bool>& genome, int numInputStates, int gateIns )
{
    int logicSize = pow( 2.0, gateIns );
    float numGates = ( genome.size() / logicSize );
    assert(fmod(numGates, 1) == 0);
    nrOfBrainStates = numGates + numInputStates;

    states.resize(nrOfBrainStates);
    nextStates.resize(nrOfBrainStates);

    this->genome = make_shared<vector<bool>>( genome );
    gates.clear();
    for( int i = numInputStates; i < nrOfBrainStates; i++ )
    {
        vector<bool> gateBits;
        for( int j = 0; j < logicSize; j++ )
        {
            gateBits.push_back( genome[( i - numInputStates ) * logicSize + j] );
        }

        vector<int> inStates;
        for( int j = gateIns; j > 0; j-- )
        {
            inStates.push_back( i - j );
        }

        gates.push_back( shared_ptr<Gate>( new BitGate( inStates, i, gateBits ) ) );
    }
}

void BitAgent::DecodeFixedLogicGenome( const vector<bool>& genome, int numInputStates, int gateIns)
{
    // NOT IMPLEMENTED
}

void BitAgent::DecodeHypercubeGenome( const vector<bool>& genome, int numInputStates, int gateIns, int cubeDimension )
{
    // TODO:
    /*
     * Check the genome size to ensure it's the right length
     */
    
    // Calculate number of bits needed for gate logic
    int gateLogic = pow( 2.0, gateIns );

    int numGates = 0; // TODO: calculate gate size

    nrOfBrainStates = numGates + numInputStates;
    states.resize(nrOfBrainStates);
    nextStates.resize(nrOfBrainStates);

    this->genome = make_shared<vector<bool>>( genome );
    gates.clear();
    for( int i = numInputStates; i < nrOfBrainStates; i++ )
    {
        vector<bool> gateBits;
        // TODO: Fill gateBits

        vector<int> inStates;
        // TODO: Fill inStates

        gates.push_back( shared_ptr<Gate>( new BitGate(inStates, i, gateBits) ) );
    }
}