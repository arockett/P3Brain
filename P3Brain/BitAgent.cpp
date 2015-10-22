/******************************************************************************
* file: BitAgent.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "BitAgent.h"

#include "BitGate.h"


BitAgent::BitAgent()
{
}


BitAgent::BitAgent(const vector<bool>& startGenome)
{
    nrOfStates = startGenome.size() / 4 + 2;
    states = new double[nrOfStates];
    nextStates = new double[nrOfStates];

    // Must still initialize the nodeMap even though BitAgent doesn't
    // need to use it since the destructor of Agent calls delete on nodeMap
    nodeMap = new int[1];
    
    genome = make_shared<vector<bool>>(startGenome);
    gates.clear();
    for( int i = 2; i < nrOfStates; i++ )
    {
        vector<bool> gateBits;
        for( int j = 0; j < 4; j++ )
        {
            gateBits.push_back( startGenome[( i - 2 ) * 4 + j] );
        }

        vector<int> inStates = {
            ( i - 2 ),
            ( i - 1 )
        };

        gates.push_back( new BitGate( inStates, i, gateBits ) );
    }
}


BitAgent::~BitAgent()
{
}
