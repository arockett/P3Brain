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

#include "Tools.h"
#include "BitAgent.h"
#include "LogicalWorld.h"
#include "BerryWorld.h"

/*
* Make sure you set trainingGround to an instance of the World you
* want to evaluate your BitAgents in.
*/
MarkovWorld::MarkovWorld( Configuration& config, int run_number )
{
    trainingGround = shared_ptr<World>( new BerryWorld() );

    string type = config.get<string>( "decoder" );
    if( type == "Unstructured" )
    {
        decoderType = BitAgent::Unstructured;
    }
    else if( type == "FixedInput" )
    {
        decoderType = BitAgent::FixedInput;
    }
    else if( type == "FixedLogic" )
    {
        decoderType = BitAgent::FixedLogic;
    }
    else if( type == "Hypercube" )
    {
        decoderType = BitAgent::Hypercube;
    }
    else
    {
        ASSERT( false, "Invalid decoder type given in P3 config file." );
    }
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
    BitAgent agent;
    if( decoderType != BitAgent::Unstructured )
    {
        agent = BitAgent( solution, 11, decoderType, 2 );
    }
    else
    {
        agent = BitAgent( solution, 11, 10, 3 );
    }

    return (float)trainingGround->testIndividual(&agent, false);
}
