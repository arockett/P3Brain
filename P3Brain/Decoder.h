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
    virtual void validateParameters( int, int, int, int, int ) = 0;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) = 0;

    bool isValidated() { return validated; }

protected:
    bool validated = false;
    int inputEncodingSize;
    int logicEncodingSize;
};


class UnstructuredDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;

private:
    int numNodes;
};


class FixedInputDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;
};


class FixedLogicDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int ) override;
    virtual vector<shared_ptr<AbstractGate>> decode( const vector<bool>&, int, int, int, int ) override;
};


class HypercubeDecoder : public Decoder
{
public:
    virtual void validateParameters( int, int, int, int, int ) override;
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


