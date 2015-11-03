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
#include "Tools.h"


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
    // Calculate the number of bits needed to represent a gate with the given # of ins
    int logicSize = pow( 2.0, gateIns );

    // How many gates does this genome size encode for? Assert that it is
    // a multiple of the number of bits needed to store one gate logic table
    float numGates = ( genome.size() / logicSize );
    ASSERT(fmod(numGates, 1) == 0,
        "Invalid P3 bit string length. With " << gateIns <<
        "gate inputs, the length must be a multiple of " << logicSize << "." );

    // How many states will be in this brain?
    nrOfBrainStates = numGates + numInputStates;
    states.resize(nrOfBrainStates);
    nextStates.resize(nrOfBrainStates);

    this->genome = make_shared<vector<bool>>( genome );
    gates.clear();

    // Starting with the first index after the input nodes, create each gate
    for( int i = numInputStates; i < nrOfBrainStates; i++ )
    {
        // Hard code the input indices to x previous nodes where x is 
        // the number of gate inputs
        vector<int> inStates;
        for( int j = gateIns; j > 0; j-- )
        {
            inStates.push_back( i - j );
        }

        // Get the gate logic table from the genome
        vector<bool> gateLogic;
        for( int j = 0; j < logicSize; j++ )
        {
            gateLogic.push_back( genome[( i - numInputStates ) * logicSize + j] );
        }

        gates.push_back( shared_ptr<Gate>( new BitGate( inStates, i, gateLogic ) ) );
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

    // Use the number of bits per input and number of bits for gate logic to calculate
    // the total number of bits needed to represent a gate
    int gateEncodingSize = gateIns * inputEncodingSize + gateLogicEncodingSize;

    // Find the total number of gates that can be represented by the given genome
    // and assert that it is the correct number
    double numGates = (double)genome.size() / (double)gateEncodingSize;
    ASSERT( fmod( numGates, 1 ) == 0 && numGates == (pow( 2.0, cubeDimension ) - numInputStates),
        "Invalid P3 bit string length. Based on the number of input states and gate complexity, length should be "
        << ( pow( 2.0, cubeDimension ) - numInputStates ) * gateEncodingSize << "." );


    /********************************************************
    * Decode the genome bit string
    ********************************************************/

    // How many states will be in this brain?
    nrOfBrainStates = numGates + numInputStates;
    states.resize(nrOfBrainStates);
    nextStates.resize(nrOfBrainStates);

    this->genome = make_shared<vector<bool>>( genome );
    gates.clear();

    // Starting with the first index after the input nodes, create each gate
    for( int i = numInputStates; i < nrOfBrainStates; i++ )
    {
        // Set the input indices
        vector<int> inStates;
        for( int k = 0; k < gateIns; k++ )
        {
            vector<bool> inNumber;
            for( int j = 0; j < inputEncodingSize; j++ )
            {
                inNumber.push_back( genome[( ( i - numInputStates ) * gateEncodingSize ) + ( k*inputEncodingSize ) + j] );
            }
            inStates.push_back( flipBit(i, boolStringToInt( inNumber )) );
        }

        // Get the gate logic
        vector<bool> gateLogic;
        for( int j = 0; j < gateLogicEncodingSize; j++ )
        {
            gateLogic.push_back( genome[( ( i - numInputStates ) * gateEncodingSize ) + ( gateIns*inputEncodingSize ) + j] );
        }

        // inStates will hold the index of each input to the gate, possible input indices
        // are any adjacent nodes (gates) to i if all brain states were connected in a hypercube
        gates.push_back( shared_ptr<Gate>( new BitGate(inStates, i, gateLogic) ) );
    }
}
