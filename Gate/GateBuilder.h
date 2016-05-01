#ifndef __BasicMarkovBrainTemplate__Gate_Builder__
#define __BasicMarkovBrainTemplate__Gate_Builder__

#include <math.h>

#include "../Utilities/Parameters.h"
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

	static shared_ptr< bool> usingProbGate;
	static shared_ptr<int> probGateInitialCount;
	static shared_ptr<bool> usingDetGate;
	static shared_ptr<int> detGateInitialCount;
	static shared_ptr<bool> usingEpsiGate;
	static shared_ptr<int> epsiGateInitialCount;
	static shared_ptr<bool> usingVoidGate;
	static shared_ptr<int> voidGateInitialCount;

	static shared_ptr<bool> usingFBGate;
	static shared_ptr<int> fBGateInitialCount;
	static shared_ptr<bool> usingGPGate;
	static shared_ptr<int> gPGateInitialCount;
	static shared_ptr<bool> usingThGate;
	static shared_ptr<int> thGateInitialCount;

	static shared_ptr<bool> usingTritDeterministicGate;
	static shared_ptr<int> tritDeterministicGateInitialCount;

	static shared_ptr<bool> usingNeuronGate;
	static shared_ptr<int> neuronGateInitialCount;



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

