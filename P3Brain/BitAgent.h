#pragma once
/******************************************************************************
* file: BitAgent.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "Agent.h"

#include <memory>

/*
 * The BitAgent class is a type of Agent that uses a bit string to represent
 * a Markov Brain. It can be used in conjunction with P3 and the MarkovWorld
 * evaluator to evolve MarkovBrains with bit strings and the P3 algorithm.
 */
class BitAgent : public Agent
{
public:
    enum Decoder {
        FixedInput,
        FixedLogic,
        Hypercube
    };

    BitAgent();
    BitAgent( const vector<bool>& startGenome, int numInputStates, Decoder decoder = FixedInput, int gateComplexity = 2 );
    virtual ~BitAgent();

private:
    shared_ptr<vector<bool>> genome;

    void DecodeFixedInputGenome( const vector<bool>&, int, int );
    void DecodeFixedLogicGenome( const vector<bool>&, int, int );
    void DecodeHypercubeGenome( const vector<bool>&, int, int );
};

