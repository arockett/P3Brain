#pragma once
/******************************************************************************
* file: LogicalWorld.h
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#ifndef LOGICALWORLD_H_
#define LOGICALWORLD_H_

#include "AbstractWorld.h"

class LogicalWorld : public AbstractWorld
{
public:
    enum Logic {
        FALSE,  // 0000
        AND,    // 0001
        ANB,    // 0010
        A,      // 0011
        BNA,    // 0100
        B,      // 0101
        XOR,    // 0110
        OR,     // 0111
        NOR,    // 1000
        NXOR,   // 1001
        NB,     // 1010
        NBNA,   // 1011
        NA,     // 1100
        NANB,   // 1101
        NAND,   // 1110
        TRUE    // 1111
    };

    LogicalWorld(const vector<Logic>& outputLogics = vector<Logic>());
    virtual ~LogicalWorld();

    virtual double testIndividual(shared_ptr<Organism> org, bool analyse, bool show = 0) override;

    virtual int requiredInputs() override
    {
        return 2;
    }

    virtual int requiredOutputs() override
    {
        return logic.size();
    }

private:
    vector<Logic> logic;
    double fitnessIncrement;
};

#endif /* LOGICALWORLD_H_ */
