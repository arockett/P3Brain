
#include "Evaluation.h"

#include "BitAgent.h"


/*
* Make sure you set trainingGround to an instance of the World you
* want to evaluate your BitAgents in.
*/
MarkovWorld::MarkovWorld(Configuration& config, int run_number)
{
    trainingGround = World();
}

/*
* The evaluate function is called by P3 every time it needs to evaluate the
* fitness of a bit string. If the evaluator is set to WorldEvaluator by a
* P3 config file or a command line parameter WHEN RUNNING P3, then this
* function will be called by P3, allowing us to translate the bit string to
* a Markov Brain and evaluate its fitness in a World.
*/
float MarkovWorld::evaluate(const vector<bool>& solution) {

    BitAgent agent = BitAgent(solution);

    return (float)trainingGround.testIndividual(&agent, false);
}
