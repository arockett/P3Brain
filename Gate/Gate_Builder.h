#ifndef __BasicMarkovBrainTemplate__Gate_Builder__
#define __BasicMarkovBrainTemplate__Gate_Builder__

#include "Gate.h"
#include "Feedback_Gate.h"
#include "GP_Gate.h"
#include "Threshold_Gate.h"

class Gate_Builder {  // manages what kinds of gates can be built
 public:

	static bool& usingProbGate;
	static int& probGateInitialCount;
	static bool& usingDetGate;
	static int& detGateInitialCount;
	static bool& usingEpsiGate;
	static int& epsiGateInitialCount;
	static bool& usingVoidGate;
	static int& voidGateInitialCount;

	static bool& usingFBGate;
	static int& fBGateInitialCount;
	static bool& usingGPGate;
	static int& gPGateInitialCount;
	static bool& usingThGate;
	static int& thGateInitialCount;




	static set<int> inUseGateTypes;  // this is global - a list of the start codeons for gates being used
	static vector<vector<int>> gateStartCodes;
	static map<int,int> intialGateCounts;

	Gate_Builder() = delete;
	~Gate_Builder();

	static void AddGate(int gateType, function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler>, int gateID)> theFunction);
	static void setupGates();
	static vector<function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler>, int gateID)>> makeGate;


	//int getIOAddress(shared_ptr<AbstractGenome::Handler> genomeHandler, shared_ptr<AbstractGenome> genome, int gateID);  // extracts one brain state value address from a genome
	static void getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID);  // extracts many brain state value addresses from a genome
	static pair<vector<int>,vector<int>> getInputsAndOutputs(const pair<int,int> insRange, const pair<int,int>, shared_ptr<AbstractGenome::Handler> genomeHandle, int gateID);  // extracts the input and output brain state value addresses for this gate

};

#endif /* defined(__BasicMarkovBrainTemplate__Gate_Builder__) */

