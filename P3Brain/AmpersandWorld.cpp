/******************************************************************************
* file: AmpersandWorld.cpp
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "AmpersandWorld.h"


AmpersandWorld::AmpersandWorld()
{
}


AmpersandWorld::~AmpersandWorld()
{
}


vector<double> AmpersandWorld::evaluateFitness(vector<Agent*> agents, bool analyse) {
    vector<double> W;
    for (int i = 0; i<agents.size(); i++)
        W.push_back(testIndividual(agents[i], analyse));
    return W;
}


double AmpersandWorld::testIndividual(Agent *agent, bool analyse) {
    // Test the agent to make them develop an "and" gate

    Data::Add(0.0, "W", agent->genome);
    return 1.0;
}
