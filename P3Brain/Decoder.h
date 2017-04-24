#pragma once
/******************************************************************************
* file: Decoder.h
*
* author: Aaron Beckett
* date: 03/24/2016
******************************************************************************/

#ifndef DECODER_H_
#define DECODER_H_

#include "Configuration.h"
#include "AbstractGate.h"

class Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int, vector<int>&, vector<int>& ) = 0;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) = 0;

    bool isValidated() { return validated; }

protected:
    void mapToBeginning( vector<int>& map, int length, int start = 0 );
    void mapToEnd( vector<int>& map, int length, int end = 0 );

    bool validated = false;
    int inputEncodingSize;
    int logicEncodingSize;
};


/**
 * Gates can connect to any node and have any internal logic.
 */
class UnstructuredDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int, vector<int>&, vector<int>& ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;

private:
    int numNodes;
};


/**
 * Gates can only connect to the previous N gates where N is the fixed number of inputs for a gate.
 * Gates can have any internal logic.
 */
class FixedInputDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int, vector<int>&, vector<int>& ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;
};


/**
 * Gates can connect to any node but all have a fixed internal logic.
 */
class FixedLogicDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int, vector<int>&, vector<int>& ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;

private:
    int numNodes;
};


/**
 * Gates are connected like a hypercube where each vertex is a gate and each edge is a possible connection.
 * Gates may only connect to one of their neighbors in the hypercube.
 * Gate may have any internal logic.
 */
class HypercubeDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int, vector<int>&, vector<int>& ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;

private:
    int cubeDimension;
};


// This mapping is used to convert a decoder's name into an instance
// of that Decoder object
namespace decoder
{
    using pointer = function<shared_ptr<Decoder>()>;
    static unordered_map<string, pointer> lookup( {
        { "Unstructured", make_shared<UnstructuredDecoder> },
        { "Fixed_Input", make_shared<FixedInputDecoder> },
        { "Fixed_Logic", make_shared<FixedLogicDecoder> },
        { "Hypercube", make_shared<HypercubeDecoder> }
    } );
}

#endif // !DECODER_H_


