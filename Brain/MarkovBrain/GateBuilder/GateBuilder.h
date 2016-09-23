//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Gate_Builder__
#define __BasicMarkovBrainTemplate__Gate_Builder__

#include "../../../Utilities/Parameters.h"
#include "../Gate/DeterministicGate.h"
//#include "../Gate/FeedbackGate.h"
#include "../Gate/FixedEpsilonGate.h"
#include "../Gate/GPGate.h"
#include "../Gate/NeuronGate.h"
#include "../Gate/ProbabilisticGate.h"
//#include "../Gate/ThresholdGate.h"
#include "../Gate/TritDeterministicGate.h"
#include "../Gate/VoidGate.h"

class Gate_Builder {  // manages what kinds of gates can be built
public:

	static shared_ptr<ParameterLink<bool>> usingProbGatePL;
	static shared_ptr<ParameterLink<int>> probGateInitialCountPL;
	static shared_ptr<ParameterLink<bool>> usingDetGatePL;
	static shared_ptr<ParameterLink<int>> detGateInitialCountPL;
	static shared_ptr<ParameterLink<bool>> usingEpsiGatePL;
	static shared_ptr<ParameterLink<int>> epsiGateInitialCountPL;
	static shared_ptr<ParameterLink<bool>> usingVoidGatePL;
	static shared_ptr<ParameterLink<int>> voidGateInitialCountPL;

	static shared_ptr<ParameterLink<bool>> usingFBGatePL;
	static shared_ptr<ParameterLink<int>> fBGateInitialCountPL;
	static shared_ptr<ParameterLink<bool>> usingGPGatePL;
	static shared_ptr<ParameterLink<int>> gPGateInitialCountPL;
	static shared_ptr<ParameterLink<bool>> usingThGatePL;
	static shared_ptr<ParameterLink<int>> thGateInitialCountPL;

	static shared_ptr<ParameterLink<bool>> usingTritDeterministicGatePL;
	static shared_ptr<ParameterLink<int>> tritDeterministicGateInitialCountPL;

	static shared_ptr<ParameterLink<bool>> usingNeuronGatePL;
	static shared_ptr<ParameterLink<int>> neuronGateInitialCountPL;

	set<int> inUseGateTypes;
	set<string> inUseGateNames;
	vector<vector<int>> gateStartCodes;

	map<int, int> intialGateCounts;

	const shared_ptr<ParametersTable> PT;

	//Gate_Builder() = default;
	Gate_Builder(shared_ptr<ParametersTable> _PT = nullptr) : PT(_PT){
		setupGates();
	}

	~Gate_Builder() = default;

	void AddGate(int gateType, function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID, shared_ptr<ParametersTable> gatePT)> theFunction);
	void setupGates();
	vector<function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID, shared_ptr<ParametersTable> gatePT)>> makeGate;

	//int getIOAddress(shared_ptr<AbstractGenome::Handler> genomeHandler, shared_ptr<AbstractGenome> genome, int gateID);  // extracts one brain state value address from a genome
	static void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID);  // extracts many brain state value addresses from a genome
	static pair<vector<int>, vector<int>> getInputsAndOutputs(const pair<int, int> insRange, const pair<int, int>, shared_ptr<AbstractGenome::Handler> genomeHandle, int gateID);  // extracts the input and output brain state value addresses for this gate

	/* *** some c++ 11 magic to speed up translation from genome to gates *** */
	//function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID)> Gate_Builder::makeGate[256];
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate_Builder__) */

