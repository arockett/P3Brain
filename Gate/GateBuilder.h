#ifndef __BasicMarkovBrainTemplate__Gate_Builder__
#define __BasicMarkovBrainTemplate__Gate_Builder__

#include <math.h>

#include "DeterministicGate.h"
#include "FeedbackGate.h"
#include "FixedEpsilonGate.h"
#include "GPGate.h"
#include "NeuronGate.h"
#include "ProbabilisticGate.h"
#include "ThresholdGate.h"
#include "TritDeterministicGate.h"
#include "VoidGate.h"

class Gate_Builder {  // manages what kinds of gates can be built
 public:

	static const bool& usingProbGate;
	static const int& probGateInitialCount;
	static const bool& usingDetGate;
	static const int& detGateInitialCount;
	static const bool& usingEpsiGate;
	static const int& epsiGateInitialCount;
	static const bool& usingVoidGate;
	static const int& voidGateInitialCount;

	static const bool& usingFBGate;
	static const int& fBGateInitialCount;
	static const bool& usingGPGate;
	static const int& gPGateInitialCount;
	static const bool& usingThGate;
	static const int& thGateInitialCount;

	static const bool& usingTritDeterministicGate;
	static const int& tritDeterministicGateInitialCount;

	static const bool& usingNeuronGate;
	static const int& neuronGateInitialCount;



	static set<int> inUseGateTypes;  // this is global - a list of the start codeons for gates being used
	static set<string> inUseGateNames;
	static vector<vector<int>> gateStartCodes;
	static map<int,int> intialGateCounts;

	Gate_Builder() = delete;
	~Gate_Builder();

	static void AddGate(int gateType, function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID)> theFunction);
	static void setupGates();
	static vector<function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID)>> makeGate;


	//int getIOAddress(shared_ptr<AbstractGenome::Handler> genomeHandler, shared_ptr<AbstractGenome> genome, int gateID);  // extracts one brain state value address from a genome
	static void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID);  // extracts many brain state value addresses from a genome
	static pair<vector<int>,vector<int>> getInputsAndOutputs(const pair<int,int> insRange, const pair<int,int>, shared_ptr<AbstractGenome::Handler> genomeHandle, int gateID);  // extracts the input and output brain state value addresses for this gate

};

#endif /* defined(__BasicMarkovBrainTemplate__Gate_Builder__) */

