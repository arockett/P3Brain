#include "GateBuilder.h"

const bool& Gate_Builder::usingProbGate = Parameters::register_parameter("probabilisticGate", false, "set to true to enable probabilistic gates", "GATE TYPES");
const int& Gate_Builder::probGateInitialCount = Parameters::register_parameter("probabilisticGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");
const bool& Gate_Builder::usingDetGate = Parameters::register_parameter("deterministicGate", true, "set to true to enable deterministic gates?", "GATE TYPES");
const int& Gate_Builder::detGateInitialCount = Parameters::register_parameter("deterministicGate_InitialCount", 6, "seed genome with this many start codons", "GATE TYPES");
const bool& Gate_Builder::usingEpsiGate = Parameters::register_parameter("fixedEpsilonGate", false, "set to true to enable epsilon gates", "GATE TYPES");
const int& Gate_Builder::epsiGateInitialCount = Parameters::register_parameter("fixedEpsilonGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");
const bool& Gate_Builder::usingVoidGate = Parameters::register_parameter("voidGate", false, "set to true to enable void gates", "GATE TYPES");
const int& Gate_Builder::voidGateInitialCount = Parameters::register_parameter("voidGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");

const bool& Gate_Builder::usingFBGate = Parameters::register_parameter("feedBackGate", false, "set to true to enable feedback gates", "GATE TYPES");
const int& Gate_Builder::fBGateInitialCount = Parameters::register_parameter("feedBackGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");
const bool& Gate_Builder::usingGPGate = Parameters::register_parameter("geneticProgramingGate", false, "set to true to enable GP (what?) gates", "GATE TYPES");
const int& Gate_Builder::gPGateInitialCount = Parameters::register_parameter("geneticProgramingGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");
const bool& Gate_Builder::usingThGate = Parameters::register_parameter("thresholdGate", false, "set to true to enable threshold gates", "GATE TYPES");
const int& Gate_Builder::thGateInitialCount = Parameters::register_parameter("thresholdGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");

const bool& Gate_Builder::usingTritDeterministicGate = Parameters::register_parameter("tritDeterministicGate", false, "set to true to enable tritDeterministic gates", "GATE TYPES");
const int& Gate_Builder::tritDeterministicGateInitialCount = Parameters::register_parameter("tritDeterministicGateGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");

const bool& Gate_Builder::usingNeuronGate = Parameters::register_parameter("neuronGate", false, "set to true to enable Neuron gates", "GATE TYPES");
const int& Gate_Builder::neuronGateInitialCount = Parameters::register_parameter("neuronGateGate_InitialCount", 3, "seed genome with this many start codons", "GATE TYPES");

set<int> Gate_Builder::inUseGateTypes;
vector<vector<int>> Gate_Builder::gateStartCodes;
map<int, int> Gate_Builder::intialGateCounts;
set<string> Gate_Builder::inUseGateNames;

// *** General tools for All Gates ***

// Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
void Gate_Builder::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<AbstractGenome::Handler> genomeHandler, int code, int gateID) {
	int i;
	for (i = 0; i < howMany; i++) {  // for the number of addresses we need
		addresses[i] = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddress) - 1, code, gateID);  // get an address
		//cout << addresses[i] << "\n";
	}
	while (i < howManyMax) {  // leave room in the genome in case this gate gets more IO later
		//cout << "skipping: " << i << "\n";
		genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddress) - 1);
		i++;
	}
}

// given a genome and a genomeIndex:
// pull out the number a number of inputs, number of outputs and then that many inputs and outputs
// if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
pair<vector<int>, vector<int>> Gate_Builder::getInputsAndOutputs(const pair<int, int> insRange, const pair<int, int> outsRange, shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {  // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

	int numInputs = genomeHandler->readInt(insRange.first, insRange.second, AbstractGate::IN_COUNT_CODE, gateID);
	//cout << "num_Inputs: " << numInputs << "\n";
	int numOutputs = genomeHandler->readInt(outsRange.first, outsRange.second, AbstractGate::OUT_COUNT_CODE, gateID);
	//cout << "num_Outputs: " << numOutputs << "\n";
	vector<int> inputs;
	vector<int> outputs;

	inputs.resize(numInputs);
	outputs.resize(numOutputs);

	if (insRange.second > 0) {
		getSomeBrainAddresses(numInputs, insRange.second, inputs, genomeHandler, AbstractGate::IN_ADDRESS_CODE, gateID);
	}
	if (outsRange.second > 0) {
		getSomeBrainAddresses(numOutputs, outsRange.second, outputs, genomeHandler, AbstractGate::OUT_ADDRESS_CODE, gateID);
	}
	return {inputs,outputs};
}

//setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
//there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
//after initializing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
void Gate_Builder::setupGates() {
	int gateIDCounter = 42;
	makeGate.resize(1 << Global::bitsPerCodon);
	for (int i = 0; i < (1 << Global::bitsPerCodon); i++) {
		AddGate(i, nullptr);
	}
	gateStartCodes.resize(1 << Global::bitsPerCodon);
	if (usingProbGate) {
		inUseGateNames.insert("Probabilistic");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = probGateInitialCount;
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);

			vector<vector<int>> rawTable = genomeHandler->readTable( {1 << addresses.first.size(), 1 << addresses.second.size()}, {16, 16}, {0, 255}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<ProbabilisticGate> nullObj = nullptr;
				return nullObj;
			}
			return make_shared<ProbabilisticGate>(addresses,rawTable,gateID);
		});
	}
	if (usingDetGate) {
		inUseGateNames.insert("Deterministic");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}

		intialGateCounts[codonOne] = detGateInitialCount;

		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {16, 4}, {0, 1}, AbstractGate::DATA_CODE, gateID);

			//vector<vector<int>> table = genomeHandler->readTable( {16,4}, {16, 4}, {0, 1}, Gate::DATA_CODE, gateID);
				if (genomeHandler->atEOC()) {
					shared_ptr<DeterministicGate> nullObj = nullptr;;
					return nullObj;
				}
				return make_shared<DeterministicGate>(addresses,table,gateID);
			});
	}
	if (usingEpsiGate) {
		inUseGateNames.insert("FixedEpsilon");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = epsiGateInitialCount;
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {16, 4}, {0, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<FixedEpsilonGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<FixedEpsilonGate>(addresses,table,gateID);
		});
	}
	if (usingVoidGate) {
		inUseGateNames.insert("Void");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = voidGateInitialCount;
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {1 << addresses.first.size(), addresses.second.size()}, {16, 4}, {0, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<VoidGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<VoidGate>(addresses,table,gateID);
		});
	}
//	if (usingFBGate) {
//		AddGate(44, [](shared_ptr<AbstractGenome> genome,shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {return make_shared<FeedbackGate>(genome,genomeHandler,gateID);});
//		Global::inUseGateTypes.insert(44);
//	}
	if (usingGPGate) {
		inUseGateNames.insert("GeneticPrograming");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = voidGateInitialCount;
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			int operation = genomeHandler->readInt(0, 8, AbstractGate::DATA_CODE, gateID);
			vector<double> constValues;
			for (int i = 0; i < 4; i++) {
				constValues.push_back(genomeHandler->readDouble(GPGate::constValueMin, GPGate::constValueMax, AbstractGate::DATA_CODE, gateID));
			}
			if (genomeHandler->atEOC()) {
				shared_ptr<GPGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<GPGate>(addresses,operation, constValues, gateID);
		});

		//GPGate(pair<vector<int>, vector<int>> _addresses, int _operation, vector<double> _constValues, int gateID);

	}
//	if (usingThGate) {
//		AddGate(46, [](shared_ptr<AbstractGenome> genome,shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {return make_shared<Thresholdgate>(genome,genomeHandler,gateID);});
//		Global::inUseGateTypes.insert(46);
//	}
	if (usingTritDeterministicGate) {
		inUseGateNames.insert("TritDeterministic");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			//gateStartCodes.insert(pair<int, vector<int> >(codonOne, vector<int>()));
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = tritDeterministicGateInitialCount;
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {
			pair<vector<int>,vector<int>> addresses = getInputsAndOutputs( {1, 4}, {1, 4}, genomeHandler, gateID);
			vector<vector<int>> table = genomeHandler->readTable( {pow(3,addresses.first.size()), addresses.second.size()}, {81, 4}, {-1, 1}, AbstractGate::DATA_CODE, gateID);
			if (genomeHandler->atEOC()) {
				shared_ptr<TritDeterministicGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<TritDeterministicGate>(addresses,table,gateID);
		});
	}
	if (usingNeuronGate) {
		inUseGateNames.insert("Neuron");
		int codonOne = gateIDCounter++;
		inUseGateTypes.insert(codonOne);
		{
			gateStartCodes[codonOne].push_back(codonOne);
			gateStartCodes[codonOne].push_back(((1 << Global::bitsPerCodon) - 1) - codonOne);
		}
		intialGateCounts[codonOne] = neuronGateInitialCount;
		AddGate(codonOne, [](shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID) {

			int numInputs = genomeHandler->readInt(NeuronGate::defaultNumInputsMin, NeuronGate::defaultNumInputsMax, AbstractGate::IN_COUNT_CODE, gateID);
			vector<int> inputs;
			inputs.resize(numInputs);
			getSomeBrainAddresses(numInputs, NeuronGate::defaultNumInputsMax, inputs, genomeHandler, AbstractGate::IN_ADDRESS_CODE, gateID);

			int output = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddress) - 1, AbstractGate::OUT_ADDRESS_CODE, gateID);

			int dischargeBehavior = NeuronGate::defaultDischargeBehavior;
			if (dischargeBehavior == -1) {
				dischargeBehavior = genomeHandler->readInt(0, 2, AbstractGate::DATA_CODE, gateID);
			}
			double thresholdValue = genomeHandler->readDouble(NeuronGate::defaultThresholdMin, NeuronGate::defaultThresholdMax, AbstractGate::DATA_CODE, gateID);

			bool thresholdActivates = 1;
			if (NeuronGate::defaultAllowRepression == 1) {
				thresholdActivates = genomeHandler->readInt(0, 1, AbstractGate::DATA_CODE, gateID);
			}

			double decayRate = genomeHandler->readDouble(NeuronGate::defaultDecayRateMin, NeuronGate::defaultDecayRateMax, AbstractGate::DATA_CODE, gateID);
			double deliveryCharge = genomeHandler->readDouble(NeuronGate::defaultDeliveryChargeMin, NeuronGate::defaultDeliveryChargeMax, AbstractGate::DATA_CODE, gateID);
			double deliveryError = NeuronGate::defaultDeliveryError;

			int ThresholdFromNode = -1;
			int DeliveryChargeFromNode = -1;
			if (NeuronGate::defaultThresholdFromNode) {
				ThresholdFromNode = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddress) - 1, AbstractGate::IN_ADDRESS_CODE, gateID);
			}
			if (NeuronGate::defaultDeliveryChargeFromNode) {
				DeliveryChargeFromNode = genomeHandler->readInt(0, (1 << Global::bitsPerBrainAddress) - 1, AbstractGate::IN_ADDRESS_CODE, gateID);
			}
			if (genomeHandler->atEOC()) {
				shared_ptr<NeuronGate> nullObj = nullptr;;
				return nullObj;
			}
			return make_shared<NeuronGate>(inputs, output, dischargeBehavior, thresholdValue, thresholdActivates, decayRate, deliveryCharge, deliveryError, ThresholdFromNode, DeliveryChargeFromNode, gateID);
		});
	}
}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
//function<shared_ptr<Gate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID)> Gate_Builder::makeGate[256];
vector<function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler>, int gateID)>> Gate_Builder::makeGate;

void Gate_Builder::AddGate(int gateType, function<shared_ptr<AbstractGate>(shared_ptr<AbstractGenome::Handler> genomeHandler, int gateID)> theFunction) {
	makeGate[gateType] = theFunction;
}
/* *** end - some c++ 11 magic to speed up translation from genome to gates *** */
