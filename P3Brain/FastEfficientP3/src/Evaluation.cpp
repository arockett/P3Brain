// Brian Goldman

// Each evaluation object defines the "evaluate" function for how
// to convert the vector of bool solution into a floating point fitness.

#include "Evaluation.h"
#include "MarkovBrain.h"
#include "Tools.h"

using namespace std;

// Template specialization to convert a string to a function pointer
// Used to allow configurable problem choice
template<>
evaluation::pointer Configuration::get(const string key) {
  return evaluation::lookup[get<string>(key)];
}

/*
* For this integration of P3 with Markov Brains, a MarkovWorld is the only
* evaluator that is possible and the specific world to test in is a 
* configurable option.
*/
MarkovWorld::MarkovWorld( Configuration& config, int run_number )
{
    trainingGround = config.get<evaluation::pointer>( "world" )();
    decoder = config.get<decoder::pointer>( "decoder" )();
    numInputNodes = trainingGround->requiredInputs();
    numOutputNodes = trainingGround->requiredOutputs();
    numHiddenNodes = config.get<int>( "gates" ) - numOutputNodes;
    ASSERT( numHiddenNodes >= 0,
        "Not enough gates, world requires at least " << numOutputNodes << " gates." );
    gateComplexity = config.get<int>( "gate_complexity" );
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
    gladiator->dataMap.ClearMap();
    auto gates = decoder->decode( solution, numInputNodes, numOutputNodes, numHiddenNodes, gateComplexity );
    gladiator->brain = make_shared<MarkovBrain>( gates, numInputNodes, numOutputNodes, numHiddenNodes );
    return (float)trainingGround->testIndividual(gladiator, false);
}
