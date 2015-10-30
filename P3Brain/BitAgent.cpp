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


// Define a custom assert function that prints out a message if the assert fails
#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            cerr << "Assertion '" #condition "' failed in " << __FILE__ \
                 << " line " << __LINE__ << ": " << message << endl; \
            exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do {} while (false)
#endif


BitAgent::BitAgent()
{
}

BitAgent::BitAgent( const vector<bool>& startGenome, int numInputStates, BitAgent::Decoder decoder, int gateComplexity)
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
        DecodeHypercubeGenome( startGenome, numInputStates, gateComplexity );
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
        vector<int> inStates;
        for( int j = gateIns; j > 0; j-- )
        {
            inStates.push_back( i - j );
        }

        vector<bool> gateBits;
        for( int j = 0; j < logicSize; j++ )
        {
            gateBits.push_back( genome[( i - numInputStates ) * logicSize + j] );
        }

        gates.push_back( shared_ptr<Gate>( new BitGate( inStates, i, gateBits ) ) );
    }
}

void BitAgent::DecodeFixedLogicGenome( const vector<bool>& genome, int numInputStates, int gateIns)
{
    // NOT IMPLEMENTED
}

void BitAgent::DecodeHypercubeGenome( const vector<bool>& genome, int numInputStates, int gateIns )
{
    /********************************************************
     * Check the genome size to ensure it's the right length
     ********************************************************/
    
    // Calculate number of bits needed for gate logic
    int gateLogicEncodingSize = pow( 2.0, gateIns );

    //
    // Calculate cube dimension necessary for given number of input states
    //
    // Minimum dimension possible
    int cubeDimension = ceil( log2( numInputStates ) ) + 1;
    // If the minimum dimension is not a power of 2, make it the next power of 2 greater
    // than the minimum possible dimension. If the hypercube dimension is not a power
    // of 2, the bits encoding each gate input will be underutilized.
    if( log2( cubeDimension ) / 1.0 != 0.0 )
    {
        cubeDimension = pow(2.0, ceil( log2( cubeDimension ) ));
    }

    ASSERT( gateIns <= cubeDimension, "Too many gate inputs. For " << numInputStates
        << " you should have <= " << cubeDimension << " inputs per gate." );

    // Calculate number of bits needed to encode each gate input
    int inputEncodingSize = log2( cubeDimension );

    int gateEncodingSize = gateIns * inputEncodingSize + gateLogicEncodingSize;
    double numGates = (double)genome.size() / (double)gateEncodingSize;
    ASSERT( numGates == pow( 2.0, cubeDimension ) - numInputStates,
        "Invalid P3 bit string length. Based on the number of input states and gate complexity, length should be"
        << (pow( 2.0, cubeDimension ) - numInputStates) * gateEncodingSize );


    /********************************************************
    * Decode the genome bit string
    ********************************************************/
    nrOfBrainStates = numGates + numInputStates;
    states.resize(nrOfBrainStates);
    nextStates.resize(nrOfBrainStates);

    this->genome = make_shared<vector<bool>>( genome );
    gates.clear();
    for( int i = numInputStates; i < nrOfBrainStates; i++ )
    {
        vector<int> inStates;
        // TODO: Fill inStates

        vector<bool> gateBits;
        // TODO: Fill gateBits

        gates.push_back( shared_ptr<Gate>( new BitGate(inStates, i, gateBits) ) );
    }
}