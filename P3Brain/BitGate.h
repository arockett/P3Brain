#pragma once
/******************************************************************************
* file: BitAgent.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#ifndef BITGATE_H_
#define BITGATE_H_

#include "Gate.h"

class BitGate : public DeterministicGate
{
public:
    BitGate() = delete;
    BitGate(const vector<int>& ins, int out, const vector<bool>& logic);
    virtual ~BitGate();

    void update( vector<double>& states, vector<double>& nextStates ) override;
    string description() override;

private:
    int gateIndex;
    vector<bool> logic;
};

#endif /* BITGATE_H_ */
