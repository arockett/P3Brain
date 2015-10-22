#pragma once
/******************************************************************************
* file: BitAgent.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "Gate.h"

class BitGate : public Gate
{
public:
    BitGate() = delete;
    BitGate(const vector<int>& ins, int out, const vector<bool>& logic);
    virtual ~BitGate();

    void update( double *states, double *nextStates ) override;

private:
    int gateIndex;
    vector<bool> logic;
};

