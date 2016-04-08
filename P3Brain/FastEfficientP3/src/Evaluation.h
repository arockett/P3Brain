// Brian Goldman

// File defining how to perform evaluation of a solution
// using multiple different evaluation functions.
// Each problem type defines its own class inherited from
// Evaluator and includes the create_evaluator macro.

#ifndef EVALUATION_H_
#define EVALUATION_H_
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <math.h>
#include <fstream>
#include <memory>
#include <array>
#include "Configuration.h"
#include "Util.h"

// Include objects from the MarkovBrain project
#include "AbstractWorld.h"
#include "Organism.h"
#include "Decoder.h"

#define PI 3.14159265

using std::vector;
using std::size_t;
using std::shared_ptr;

// This macro is used to define a factory function, simplifying the transition from
// configuration option to object capable of performing evaluation.
#define create_evaluator(name) static shared_ptr<Evaluator> create(Configuration& config, int run_number)\
{\
	return shared_ptr<Evaluator>(new name(config, run_number));\
}

// Base class to define the interface for evaluation functions
class Evaluator {
 public:
  Evaluator() = default;
  virtual ~Evaluator() = default;
  float virtual evaluate(const vector<bool> & solution) = 0;
};

/*
 * The 'MarkovWorld' evaluator essentially uses a World defined in the
 * MarkovBrain project to evaluate P3 bit strings and return a fitness
 * to the P3 algorithm.
 */
class MarkovWorld : public Evaluator {
public:
    MarkovWorld(Configuration& config, int run_number);
    float evaluate(const vector<bool>& solution) override;
    create_evaluator(MarkovWorld);

private:
    shared_ptr<AbstractWorld> trainingGround = nullptr;
    shared_ptr<Organism> gladiator = make_shared<Organism>();
    shared_ptr<Decoder> decoder = nullptr;
    int numInputNodes;
    int numOutputNodes;
    int numHiddenNodes;
    int gateComplexity;
    int trials;
    vector<int> inputMapping;
    vector<int> outputMapping;
};


#include "LogicalWorld.h"
#include "BerryWorld.h"
#include "UtilityWorld.h"
// This mapping is used to convert a problem's name into an instance
// of that Evaluator object
namespace evaluation {
using pointer=std::function<shared_ptr<AbstractWorld>()>;
static std::unordered_map<string, pointer> lookup( {
    { "Logical", make_shared<LogicalWorld> },
    { "Berry", make_shared<BerryWorld> },
    { "Utility", make_shared<UtilityWorld> },
});
}

#endif /* EVALUATION_H_ */
