/******************************************************************************
* file: Decoder.cpp
*
* author: Aaron Beckett
* date: 03/24/2016
******************************************************************************/

#include "Decoder.h"

#include "Tools.h"


// Template specialization to convert a string to a function pointer
// Used to all configurable decoder selection
template<>
decoder::pointer Configuration::get( const string key )
{
    return decoder::lookup[get<string>( key )];
}


vector<shared_ptr<Gate>> UnstructuredDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    /********************************************************
    * Calculate encoding sizes to ensure the genome is long enough
    ********************************************************/
    int numNodes = numInputNodes + numOutputNodes + numHiddenNodes;
    int numGates = numOutputNodes + numHiddenNodes;

    int logicEncodingSize = 1 << gateIns;
    int inputEncodingSize = ceil( log2( numNodes ) );
    int gateEncodingSize = gateIns * inputEncodingSize + logicEncodingSize;
    ASSERT( genome.size() / gateEncodingSize == numGates,
        "Invalid genome length, should be " << numGates * gateEncodingSize << "." );

    /********************************************************
    * Decode the genome bit string
    ********************************************************/
    vector<shared_ptr<Gate>> gates;
    auto start = genome.begin();
    int nodeId = numInputNodes;
    while( start != genome.end() )
    {
        /* Create a gate: */

        // Get inputs
        vector<int> inNodes;
        for( int k = 0; k < gateIns; k++ )
        {
            auto end = start + ( ( k + 1 ) * inputEncodingSize );
            vector<bool> inNumber( start, end );
            inNodes.push_back( boolStringToInt( inNumber ) % numNodes );
            start = end;
        }

        // Set output
        vector<int> outNode = { nodeId };

        // Get logic
        auto end = start + logicEncodingSize;
        vector<vector<int>> logic;
        for( start; start != end; start++ )
        {
            logic.push_back( vector<int>( 1, *start ) );
        }

        // Create and add gate
        gates.push_back( make_shared<DeterministicGate>( pair<vector<int>, vector<int>>( inNodes, outNode ), logic, nodeId - numInputNodes ) );

        nodeId++;
    }
}


vector<shared_ptr<Gate>> FixedInputDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    // TODO Layer the fixed gate inputs so that each Brain Input state can be accessed by a gate

    // Calculate the number of bits needed to represent a gate with the given # of ins
    int logicEncodingSize = 1 << gateIns;

    // How many gates does this genome size encode for? Assert that it is
    // a multiple of the number of bits needed to store one gate logic table
    int numGates = numOutputNodes + numHiddenNodes;
    float encodedGates = (float)genome.size() / (float)logicEncodingSize;
    ASSERT( encodedGates == (float)numGates,
        "Invalid P3 bit string length. With " << gateIns <<
        "gate inputs, the length must be a multiple of " << logicEncodingSize << "." );

    // Assert that there are enough input nodes for the given # of gate ins
    ASSERT( numInputNodes >= gateIns,
        "Invalid gate complexity. The number of gate inputs must be no more than the number of inpute nodes." );

    vector<shared_ptr<Gate>> gates;
    auto start = genome.begin();
    int nodeId = numInputNodes;
    while( start != genome.end() )
    {
        /* Create a gate: */

        // Hard code the input indices to x previous nodes where x is 
        // the number of gate inputs
        vector<int> inNodes;
        for( int j = gateIns; j > 0; j-- )
        {
            inNodes.push_back( nodeId - j );
        }

        // Set the output indices
        vector<int> outNode = { nodeId };

        // Get the gate logic
        auto end = start + logicEncodingSize;
        vector<vector<int>> logic;
        for( start; start != end; start++ )
        {
            logic.push_back( vector<int>( 1, *start ) );
        }

        // inStates will hold the index of each input to the gate, possible input indices
        // are any adjacent nodes (gates) to nodeId if all nodes were connected in a hypercube
        gates.push_back( make_shared<DeterministicGate>( pair<vector<int>, vector<int>>( inNodes, outNode ), logic, nodeId - numInputNodes ) );

        nodeId++;
    }
}


vector<shared_ptr<Gate>> FixedLogicDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    // TODO
}


vector<shared_ptr<Gate>> HypercubeDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    /********************************************************
    * Check the genome size to ensure it's the right length
    ********************************************************/

    // Calculate number of bits needed for gate logic
    int logicEncodingSize = 1 << gateIns;

    //
    // Calculate cube dimension necessary for given number of input states
    //
    // Minimum dimension possible
    int cubeDimension = (int)ceil( log2( numInputNodes ) ) + 1;

    ASSERT( gateIns <= cubeDimension, "Too many gate inputs. For " << numInputNodes
        << " you should have <= " << cubeDimension << " inputs per gate." );

    // Calculate number of bits needed to encode each gate input
    int inputEncodingSize = (int)ceil( log2( cubeDimension ) );

    // Use the number of bits per input and number of bits for gate logic to calculate
    // the total number of bits needed to represent a gate
    int gateEncodingSize = gateIns * inputEncodingSize + logicEncodingSize;

    // Make sure the user has asked for the correct number of gates
    int numGates = (int)( pow( 2.0, cubeDimension ) - numInputNodes );
    ASSERT( numOutputNodes + numHiddenNodes == numGates,
        "Invalid gate number. For a world with " << numInputNodes << " inputs, " << numGates
        << " are needed for a hypercube encoding." );

    // Find the total number of gates that can be represented by the given genome
    // and assert that it is the correct number
    float encodedGates = (float)genome.size() / (float)gateEncodingSize;
    ASSERT( encodedGates == (float)numGates,
        "Invalid P3 bit string length. Based on the number of input nodes and gate complexity, length should be "
        << numGates * gateEncodingSize << "." );


    /********************************************************
    * Decode the genome bit string
    ********************************************************/

    vector<shared_ptr<Gate>> gates;
    auto start = genome.begin();
    int nodeId = numInputNodes;
    while( start != genome.end() )
    {
        /* Create a gate: */

        // Get inputs
        vector<int> inNodes;
        for( int k = 0; k < gateIns; k++ )
        {
            auto end = start + ( ( k + 1 )*inputEncodingSize );
            vector<bool> inNumber( start, end );
            inNodes.push_back( flipBit( nodeId, boolStringToInt( inNumber ) % cubeDimension ) );
        }

        // Set the output indices
        vector<int> outNode = { nodeId };

        // Get the gate logic
        auto end = start + logicEncodingSize;
        vector<vector<int>> logic;
        for( start; start != end; start++ )
        {
            logic.push_back( vector<int>( 1, *start ) );
        }

        // inStates will hold the index of each input to the gate, possible input indices
        // are any adjacent nodes (gates) to nodeId if all nodes were connected in a hypercube
        gates.push_back( make_shared<DeterministicGate>( pair<vector<int>, vector<int>>( inNodes, outNode ), logic, nodeId - numInputNodes ) );

        nodeId++;
    }
}
