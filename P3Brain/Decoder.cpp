/******************************************************************************
* file: Decoder.cpp
*
* author: Aaron Beckett
* date: 03/24/2016
******************************************************************************/

#include "Decoder.h"

#include "Tools.h"
#include "DeterministicGate.h"


// Template specialization to convert a string to a function pointer
// Used to all configurable decoder selection
template<>
decoder::pointer Configuration::get( const string key )
{
    return decoder::lookup[get<string>( key )];
}


void Decoder::mapToBeginning( vector<int>& map, int length, int start )
{
    map.resize( length );
    for( int i = 0; i < length; i++ )
    {
        map[i] = i + start;
    }
}

void Decoder::mapToEnd( vector<int>& map, int length, int end )
{
    map.resize( length );
    for( int i = 0; i < length; i++ )
    {
        map[i] = end - length + i;
    }
}


/***************************************************************************************/

void UnstructuredDecoder::validateParameters( int genomeLength, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns, vector<int>& iMap, vector<int>& oMap )
{
    /********************************************************
    * Calculate encoding sizes to ensure the genome is long enough
    ********************************************************/
    numNodes = numInputNodes + numOutputNodes + numHiddenNodes;
    int numGates = numOutputNodes + numHiddenNodes;

    logicEncodingSize = 1 << gateIns;
    inputEncodingSize = (int)ceil( log2( numNodes ) );
    int gateEncodingSize = gateIns * inputEncodingSize + logicEncodingSize;
    ASSERT( genomeLength / gateEncodingSize == numGates,
        "Invalid genome length, should be " << numGates * gateEncodingSize << "." );

    /*
     * Set the input and output mappings
     */
    mapToBeginning( iMap, numInputNodes );
    mapToBeginning( oMap, numOutputNodes, numInputNodes );

    validated = true;
}

vector<shared_ptr<AbstractGate>> UnstructuredDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    /********************************************************
    * Decode the genome bit string
    ********************************************************/
    vector<shared_ptr<AbstractGate>> gates;
    auto start = genome.begin();
    int nodeId = numInputNodes;
    while( start != genome.end() )
    {
        /* Create a gate: */

        // Get inputs
        vector<int> inNodes;
        for( int i = 0; i < gateIns; i++ )
        {
            auto end = start + inputEncodingSize;
            vector<bool> inNumber( start, end );
            inNodes.push_back( boolStringToInt( inNumber ) % numNodes );
            start = end;
        }

        // Set output
        vector<int> outNode = { nodeId };

        // Get logic
        auto end = start + logicEncodingSize;
        vector<vector<int>> logic;
        for( ; start != end; start++ )
        {
            logic.push_back( vector<int>( 1, (int)*start ) );
        }

        // Create and add gate
        gates.push_back( make_shared<DeterministicGate>( pair<vector<int>, vector<int>>( inNodes, outNode ), logic, nodeId - numInputNodes ) );

        nodeId++;
    }

    return gates;
}


/***************************************************************************************/
void FixedInputDecoder::validateParameters( int genomeLength, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns, vector<int>& iMap, vector<int>& oMap )
{
    // Calculate the number of bits needed to represent a gate with the given # of ins
    logicEncodingSize = 1 << gateIns;

    // How many gates does this genome size encode for? Assert that it is
    // a multiple of the number of bits needed to store one gate logic table
    int numGates = numOutputNodes + numHiddenNodes;
    float encodedGates = (float)genomeLength / (float)logicEncodingSize;
    ASSERT( encodedGates == (float)numGates,
        "Invalid P3 bit string length. With " << gateIns << " gate inputs, and "
        << numGates << " gates, the bit string length must be " << logicEncodingSize * numGates << "." );

    // Assert that there are enough input nodes for the given # of gate ins
    ASSERT( numInputNodes >= gateIns,
        "Invalid gate complexity. The number of gate inputs must be no more than the number of inpute nodes." );

    /*
    * Set the input and output mappings
    */
    mapToBeginning( iMap, numInputNodes );
    mapToEnd( oMap, numOutputNodes, numInputNodes + numGates );

    validated = true;
}

vector<shared_ptr<AbstractGate>> FixedInputDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    // TODO Layer the fixed gate inputs so that each Brain Input state can be accessed by a gate

    vector<shared_ptr<AbstractGate>> gates;
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
        for( ; start != end; start++ )
        {
            logic.push_back( vector<int>( 1, (int)*start ) );
        }

        // inStates will hold the index of each input to the gate, possible input indices
        // are any adjacent nodes (gates) to nodeId if all nodes were connected in a hypercube
        gates.push_back( make_shared<DeterministicGate>( pair<vector<int>, vector<int>>( inNodes, outNode ), logic, nodeId - numInputNodes ) );

        nodeId++;
    }

    return gates;
}


/***************************************************************************************/

void FixedLogicDecoder::validateParameters( int genomeLength, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns, vector<int>& iMap, vector<int>& oMap )
{

}

vector<shared_ptr<AbstractGate>> FixedLogicDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    // TODO
    return vector<shared_ptr<AbstractGate>>();
}


/***************************************************************************************/

void HypercubeDecoder::validateParameters( int genomeLength, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns, vector<int>& iMap, vector<int>& oMap )
{
    /********************************************************
    * Check the genome size to ensure it's the right length
    ********************************************************/

    // How many gates did the user ask for?
    int numGates = numOutputNodes + numHiddenNodes;
    // Based on this, calculate the cube dimension provided by the user
    cubeDimension = (int)ceil( log2( numInputNodes + numGates ) );
    // Calulate the required gates based on the implied cube dimension (round up)
    int requiredGates = (int)( pow( 2.0, cubeDimension ) - numInputNodes );

    // Calculate cube dimension necessary for number of world input states
    int minDimension = (int)ceil( log2( numInputNodes ) ) + 1;
    // Calculate the number of gates need for the minimum cube dimension
    int minGates = (int)( pow( 2.0, minDimension ) - numInputNodes );

    // Make sure the dimension is high enough for the number of inputs
    ASSERT( cubeDimension >= minDimension,
        "Not enough gates. For " << numInputNodes << " world inputs the minimum "
        << "hypercube dimension is " << minDimension << " which requires " << minGates << " gates.");

    // Make sure the user has asked for the correct number of gates
    ASSERT( numGates == requiredGates,
        "Invalid gate number. To have at least " << numGates << " gates using this world requires a hypercube "
        << "dimension of " << cubeDimension << " which requires " << requiredGates << " gates.");

    // Make sure the cube dimension is high enough for the gate complexity
    ASSERT( gateIns <= cubeDimension, "Too many gate inputs. For " << numGates
        << " gates and " << numInputNodes << " world inputs you should have <= "
        << cubeDimension << " inputs per gate." );

    // Calculate number of bits needed for gate logic
    logicEncodingSize = 1 << gateIns;
    // Calculate number of bits needed to encode each gate input
    inputEncodingSize = (int)ceil( log2( cubeDimension ) );

    // Use the number of bits per input and number of bits for gate logic to calculate
    // the total number of bits needed to represent a gate
    int gateEncodingSize = gateIns * inputEncodingSize + logicEncodingSize;

    // Find the total number of gates that can be represented by the given genome
    // and assert that it is the correct number
    float encodedGates = (float)genomeLength / (float)gateEncodingSize;
    ASSERT( encodedGates == (float)numGates,
        "Invalid P3 bit string length. Based on the number of input nodes and gate complexity, length should be "
        << numGates * gateEncodingSize << "." );

    /*
    * Set the input and output mappings
    */
    mapToBeginning( iMap, numInputNodes );
    mapToEnd( oMap, numOutputNodes, numInputNodes + numGates );

    validated = true;
}

vector<shared_ptr<AbstractGate>> HypercubeDecoder::decode( const vector<bool>& genome, int numInputNodes, int numOutputNodes, int numHiddenNodes, int gateIns )
{
    /********************************************************
    * Decode the genome bit string
    ********************************************************/

    vector<shared_ptr<AbstractGate>> gates;
    auto start = genome.begin();
    int nodeId = numInputNodes;
    while( start != genome.end() )
    {
        /* Create a gate: */

        // Get inputs
        vector<int> inNodes;
        for( int i = 0; i < gateIns; i++ )
        {
            auto end = start + inputEncodingSize;
            vector<bool> inNumber( start, end );
            inNodes.push_back( flipBit( nodeId, boolStringToInt( inNumber ) % cubeDimension ) );
            start = end;
        }

        // Set the output indices
        vector<int> outNode = { nodeId };

        // Get the gate logic
        auto end = start + logicEncodingSize;
        vector<vector<int>> logic;
        for( ; start != end; start++ )
        {
            logic.push_back( vector<int>( 1, (int)*start ) );
        }

        // inStates will hold the index of each input to the gate, possible input indices
        // are any adjacent nodes (gates) to nodeId if all nodes were connected in a hypercube
        gates.push_back( make_shared<DeterministicGate>( pair<vector<int>, vector<int>>( inNodes, outNode ), logic, nodeId - numInputNodes ) );

        nodeId++;
    }

    return gates;
}
