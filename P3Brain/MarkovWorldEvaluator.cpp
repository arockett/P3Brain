
#include "Evaluation.h"

#include "BitAgent.h"

/* The evaluate function is called by P3 every time it needs to evaluate the
* fitness of a bit string. If the evaluator is set to WorldEvaluator by a
* P3 config file or a command line parameter WHEN RUNNING P3, then this
* function will be called by P3, allowing us to translate the bit string to
* a Markov Brain and evaluate its fitness in a World.
*/
float MarkovWorld::evaluate(const vector<bool>& solution) {

    BitAgent agent = BitAgent(solution);

    // If setWorld is not called on the MarkovWorld evaluator prior
    // calling this function, trainingGround will not have been set
    // and this statement will fail
    return (float)trainingGround.testIndividual(&agent, false);
}
