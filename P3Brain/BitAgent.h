#pragma once
/******************************************************************************
* file: BitAgent.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "Agent.h"

/*
 * The BitAgent class is a type of Agent that uses a bit string to represent
 * a Markov Brain. It can be used in conjunction with P3 and the MarkovWorld
 * evaluator to evolve MarkovBrains with bit strings and the P3 algorithm.
 */
class BitAgent : public Agent
{
public:
    BitAgent();
    BitAgent(const vector<bool>& genome);
    virtual ~BitAgent();
};

