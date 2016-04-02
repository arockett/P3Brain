//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "MarkovBrain.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

//bool& MarkovBrain::cacheResults = Parameters::register_parameter("MarkovBrain_cacheResults", true, "if true, t+1 nodes will be cached. If the same input is seen, the cached node values will be used.", "BRAIN - MARKOV");
//int& MarkovBrain::cacheResultsCount = Parameters::register_parameter("MarkovBrain_cacheResultsCount", 1, "input combinations will be cached this many times, after this, repeats of a given input array will look up a random value from cached values", "BRAIN - MARKOV");

MarkovBrain::MarkovBrain(vector<shared_ptr<Gate>> _gates, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes) {
	GLB = nullptr;
	gates = _gates;
	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("gates");
	for (auto name : Gate_Builder::inUseGateNames) {
		aveFileColumns.push_back(name + "Gates");
	}
}

MarkovBrain::MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes) :
		AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes) {
	GLB = _GLB;
	//make a node map to handle genome value to brain state address look up.
	makeNodeMap(nodeMap, Global::bitsPerBrainAddress, nrOfBrainNodes);

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("gates");
	for (auto name : Gate_Builder::inUseGateNames) {
		aveFileColumns.push_back(name + "Gates");
	}
}

MarkovBrain::MarkovBrain(shared_ptr<AbstractGateListBuilder> _GLB, shared_ptr<AbstractGenome> genome, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes) :
		MarkovBrain(_GLB, _nrInNodes, _nrOutNodes, _nrHiddenNodes) {  //this is a constructor. it is run whenever a new brain is created.
	//cout << "in MarkovBrain::MarkovBrain(shared_ptr<Base_GateListBuilder> _GLB, shared_ptr<AbstractGenome> genome, int _nrOfBrainStates)\n\tabout to - gates = GLB->buildGateList(genome, nrOfBrainStates);" << endl;
	gates = GLB->buildGateList(genome, nrOfBrainNodes);
	inOutReMap();  // map ins and outs from genome values to brain states
}

shared_ptr<AbstractBrain> MarkovBrain::makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
	shared_ptr<MarkovBrain> newBrain = make_shared<MarkovBrain>(GLB, _genome, nrInNodes, nrOutNodes, nrHiddenNodes);
	return newBrain;
}

void MarkovBrain::resetBrain() {
	AbstractBrain::resetBrain();
	for (size_t i = 0; i < gates.size(); i++) {
		gates[i]->resetGate();
	}
}

void MarkovBrain::update() {
	nextNodes.assign(nrOfBrainNodes, 0.0);
	for (size_t i = 0; i < gates.size(); i++) {  //update each gate
		gates[i]->update(nodes, nextNodes);
	}
	swap(nodes, nextNodes);
}

void MarkovBrain::inOutReMap() {  // remaps genome site values to valid brain state addresses
	for (size_t i = 0; i < gates.size(); i++) {
		gates[i]->applyNodeMap(nodeMap, nrOfBrainNodes);
	}

}

string MarkovBrain::description() {
	string S = "Markov Briain\n" + gateList();
	return S;
}

vector<string> MarkovBrain::getStats() {
	vector<string> dataPairs;
	vector<int> nodesConnections, nextNodesConnections;
	nodesConnections.resize(nrOfBrainNodes);
	nextNodesConnections.resize(nrOfBrainNodes);

	dataPairs.push_back("gates");
	dataPairs.push_back(to_string(gates.size()));

	map<string, int> gatecounts;
	for (auto n : Gate_Builder::inUseGateNames) {
		gatecounts[n + "Gates"] = 0;
	}
	for (auto g : gates) {
		auto gateConnections = g->getConnectionsLists();
		for (auto c : gateConnections.first){
			nodesConnections[c]++;
		}
		for (auto c : gateConnections.second){
			nextNodesConnections[c]++;
		}
		gatecounts[g->gateType() + "Gates"]++;
	}

	string nodesConnectionsString = "\"[";
	string nextNodesConnectionsString = "\"[";

	for (int i = 0; i < nrOfBrainNodes; i++){
		nodesConnectionsString+=to_string(nodesConnections[i])+",";
		nextNodesConnectionsString+=to_string(nextNodesConnections[i])+",";
	}
	nodesConnectionsString.pop_back();
	nodesConnectionsString+="]\"";
	dataPairs.push_back("nodesConnections");
	dataPairs.push_back(nodesConnectionsString);
	nextNodesConnectionsString.pop_back();
	nextNodesConnectionsString+="]\"";
	dataPairs.push_back("nextNodesConnections");
	dataPairs.push_back(nextNodesConnectionsString);


	for (auto n : Gate_Builder::inUseGateNames) {
		dataPairs.push_back(n + "Gates");
		dataPairs.push_back(to_string(gatecounts[n + "Gates"]));
	}

	return (dataPairs);
}

string MarkovBrain::gateList() {
	string S = "";
	for (size_t i = 0; i < gates.size(); i++) {
		S = S + gates[i]->description();
	}
	return S;
}

vector<vector<int>> MarkovBrain::getConnectivityMatrix() {
	vector<vector<int>> M;
	M.resize(nrOfBrainNodes);
	for (int i = 0; i < nrOfBrainNodes; i++) {
		M[i].resize(nrOfBrainNodes);
		for (int j = 0; j < nrOfBrainNodes; j++)
			M[i][j] = 0;
	}
	for (auto G : gates) {
		vector<int> I = G->getIns();
		vector<int> O = G->getOuts();
		for (int i : I)
			for (int o : O)
				M[i][o]++;
	}
	return M;
}

int MarkovBrain::brainSize() {
	return (int) gates.size();
}

int MarkovBrain::numGates() {
	return brainSize();
}

void MarkovBrain::initalizeGenome(shared_ptr<AbstractGenome> _genome) {
	int codonMax = (1 << Global::bitsPerCodon) - 1;
	_genome->fillRandom();

	auto genomeHandler = _genome->newHandler(_genome);

	for (auto gateType : Gate_Builder::inUseGateTypes) {
		//cout << gateType << " : "<<Gate_Builder::intialGateCounts[gateType]<<endl;
		for (int i = 0; i < Gate_Builder::intialGateCounts[gateType]; i++) {
			//cout << "    " << i << " : ";
			genomeHandler->randomize();
			for (auto value : Gate_Builder::gateStartCodes[gateType]) {
				//cout << value << " ";
				genomeHandler->writeInt(value, 0, codonMax);
			}
			//cout << endl;
		}
	}
}

