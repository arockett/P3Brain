#pragma once
/******************************************************************************
* file: BitAgent.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#ifndef BITAGENT_H_
#define BITAGENT_H_

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
        Unstructured,
        FixedInput,
        FixedLogic,
        Hypercube
    };

    BitAgent();
    BitAgent( const vector<bool>& startGenome, int numInputStates, int numHiddenStates, int numOutStates, int gateComplexity );
    BitAgent( const vector<bool>& startGenome, int numInputStates, int gateComplexity, Decoder decoder = Unstructured );
    virtual ~BitAgent();

private:
    void DecodeUnstructuredGenome( const vector<bool>&, int, int, int, int );
    void DecodeFixedInputGenome( const vector<bool>&, int, int );
    void DecodeFixedLogicGenome( const vector<bool>&, int, int );
    void DecodeHypercubeGenome( const vector<bool>&, int, int );
};

#endif /* BITAGENT_H_ */
