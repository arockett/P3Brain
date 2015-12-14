#pragma once
/******************************************************************************
* file: BitBrain.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#ifndef BITAGENT_H_
#define BITAGENT_H_

#include "Brain.h"

#include <memory>

/*
 * The BitBrain class is a type of Agent that uses a bit string to represent
 * a Markov Brain. It can be used in conjunction with P3 and the MarkovWorld
 * evaluator to evolve MarkovBrains with bit strings and the P3 algorithm.
 */
class BitBrain : public Brain
{
public:
    enum Decoder {
        Unstructured,
        FixedInput,
        FixedLogic,
        Hypercube
    };

    BitBrain();
    BitBrain( const vector<bool>& startGenome, int numInputStates, int numHiddenStates, int numOutStates, int gateComplexity );
    BitBrain( const vector<bool>& startGenome, int numInputStates, int gateComplexity, Decoder decoder = Unstructured );
    virtual ~BitBrain();

private:
    void DecodeUnstructuredGenome( const vector<bool>&, int, int, int, int );
    void DecodeFixedInputGenome( const vector<bool>&, int, int );
    void DecodeFixedLogicGenome( const vector<bool>&, int, int );
    void DecodeHypercubeGenome( const vector<bool>&, int, int );
};

#endif /* BITAGENT_H_ */
