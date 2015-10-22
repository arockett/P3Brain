/******************************************************************************
* file: MarkovWorldEvaluator.cpp
*
* This file defines the constructor and evaluate function for the P3 Evaluator
* named 'MarkovWorld'. 'MarkovWorld' allows you to use P3 to evolve Markov Brains
* by using the custom 'BitAgent' to translate bit strings into Markov Brains and 
* then using a normal Markov Brain World to calculate a fitness for the translated
* bit string.
*
* Look at BitAgent.h and BitAgent.cpp for more information on translating bit
* strings into Markov Brains.
*
* author: Aaron Beckett
* date: 10/15/2015
******************************************************************************/

#include "Evaluation.h"

#include "BitAgent.h"
#include "AmpersandWorld.h"

/*
* Make sure you set trainingGround to an instance of the World you
* want to evaluate your BitAgents in.
*/
MarkovWorld::MarkovWorld(Configuration& config, int run_number)
{
    trainingGround = shared_ptr<World>( new AmpersandWorld( AmpersandWorld::Logic::XOR ) );
}

/*
* The evaluate function is called by P3 every time it needs to evaluate the
* fitness of a bit string. If the evaluator is set to "MarkovWorld" by a
* P3 config file or a command line parameter WHEN RUNNING P3, then this
* function will be called by P3, allowing us to translate the bit string to
* a Markov Brain and evaluate its fitness in the 'trainingGround'.
*/
float MarkovWorld::evaluate(const vector<bool>& solution)
{
    BitAgent agent = BitAgent(solution);

    return trainingGround->testIndividual(&agent, false);
}
