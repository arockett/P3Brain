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
    skipGate = *Data::parameterDouble["skipGate"];
    nrOfStates = startGenome.size() / 4 + 2;
    states = new double[nrOfStates];
    nextStates = new double[nrOfStates];
    
    mGenome = make_shared<vector<bool>>(startGenome);
    gates.clear();
    for( int i = 2; i < nrOfStates; i++ )
    {
        vector<bool> gateBits;
        for( int j = 0; j < 4; j++ )
        {
            gateBits.push_back( startGenome[i * 4 + j] );
        }

        vector<int> inStates = {
            ( i - 2 ),
            ( i - 1 )
        };

        gates.push_back( &BitGate( inStates, i, gateBits ) );
    }
}


BitAgent::~BitAgent()
{
    Agent::~Agent();
}
