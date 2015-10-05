//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Agent.h"

#define SKIPGATE 1 // if 0, remove the skipgate code - FOR SPEED UP!

vector<int> Agent::defaultNodeMap;

int& Agent::defaultNrOfBrainStates = Parameters::register_parameter("brainSize", 16, "number of Brain Values", "AGENT");

double& Agent::skipGate = Parameters::register_parameter("skipGate", 0.0, "chance that a gate will not fire", "AGENT");

bool& Agent::serialProcessing = Parameters::register_parameter("serialProcessing", false,
		"sets agents to overwrite... right?", "AGENT");

Agent::Agent() {
}

Agent::Agent(Genome* startGenome, int _nrOfBrainStates) { //this is a constructor. it is run whenever a new agent is created.
	//cout << "in agent contructor\n";
	nrOfBrainStates = _nrOfBrainStates;
	states.resize(nrOfBrainStates);
	nextStates.resize(nrOfBrainStates);
	genome = startGenome;
	gates.clear();
	for (size_t i = 0; i < genome->sites.size() - 1; i++) {
		const int S = genome->sites[i];
		if ((S + genome->sites[i + 1]) == 256) {
			if (Gate::makeGate[S] != NULL) {
				gates.push_back(Gate::makeGate[S](genome, i));
			}
		}
	}

	inOutReMap();
	/*
	 for (auto gate : gates) {
	 cout << "remapped inputs: ";
	 for (int x = 0; x < gate->inputs.size(); x++) {
	 cout << gate->inputs[x] << " ";
	 }
	 cout << "\n";
	 cout << "remapped outputs: ";
	 for (int x = 0; x < gate->outputs.size(); x++) {
	 cout << gate->outputs[x] << " ";
	 }
	 cout << "\n";
	 }
	 */
	//cout << "leaving agent contructor\n";

}

Agent::~Agent() {
	cout << "HERE?\n";
	cout << gates.size() << "--\n";
}

void Agent::resetBrain() {
	for (int i = 0; i < nrOfBrainStates; i++)
		states[i] = 0.0;
	for (size_t i = 0; i < gates.size(); i++)
		gates[i]->resetGate();
}

void Agent::updateStates() {
	/*
	 if(Agent::serialProcessing){
	 //this is a special way of serialized updating
	 //experimental feature, should be default off!
	 //only change if you really know what you are doing!
	 for(int i=0;i<gates.size();i++){
	 gates[i]->update(states, states);
	 }
	 } else {*/
	//this is the default way of parallel updating
	for (int i = 0; i < nrOfBrainStates; i++) { //reset all states to 0
		nextStates[i] = 0.0;
	}
	for (size_t i = 0; i < gates.size(); i++) { //update each gate
#if SKIPGATE==1
		if (((double) rand() / (double) RAND_MAX) > Agent::skipGate) {
			gates[i]->update(states, nextStates);
		}
#else
		gates[i]->update(&states, &nextStates);
#endif // SKIPGATE
	}
	for (int i = 0; i < nrOfBrainStates; i++) { //sets states for t+1 to states just updated in the last line
		states[i] = nextStates[i];
		//cout << states[i] << " ";
	}
	//cout << "\n";
}

void Agent::inOutReMap() { // remaps genome site values to valid brain state addresses
	//cout << "in Agent::inOutReMap\n";
	for (size_t i = 0; i < gates.size(); i++) {
		gates[i]->applyNodeMap(defaultNodeMap, nrOfBrainStates);
	}
	//cout << "leaving Agent::inOutReMap\n";

}

int Agent::IntFromState(vector<int> I) {
	int r = 0;
	for (size_t i = 0; i < I.size(); i++)
		r = (r << 1) + Bit(states[I[i]]);
	return r;
}

int Agent::IntFromAllStates() {
	int r = 0;
	for (int i = 0; i < nrOfBrainStates; i++)
		r = (r << 1) + Bit(states[i]);
	return r;

}

string Agent::gateList() {
	string S = "";
	for (size_t i = 0; i < gates.size(); i++) {
		S = S + gates[i]->description();
	}
	return S;
}

vector<vector<int>> Agent::getConnectivityMatrix() {
	vector<vector<int>> M;
	M.resize(nrOfBrainStates);
	for (int i = 0; i < nrOfBrainStates; i++) {
		M[i].resize(nrOfBrainStates);
		for (int j = 0; j < nrOfBrainStates; j++)
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

set<int> Agent::findCodingRegions(int mask) { //if mask=0, all coding regions are returned. if mask=1, return everything except start codon. if mask=2, return everything except start codon and wiring.
	set<int> allCodingRegions;
	for (auto gateIterator = gates.begin(); gateIterator != gates.end(); gateIterator++) {
		for (auto iterator = (*gateIterator)->codingRegions.begin(); iterator != (*gateIterator)->codingRegions.end();
				iterator++) {
			if (iterator->second >= mask) {
				allCodingRegions.insert(iterator->first);
			}
		}
	}
	/*for(auto iterator = allCodingRegions.begin(); iterator != allCodingRegions.end(); iterator++){
	 cout << *iterator << "(" << (int)genome->genome[*iterator] << ") ";
	 }
	 cout << "\n";
	 */
	return allCodingRegions;
}

int Agent::brainSize() {
	return (int) gates.size();
}

int Agent::numGates() {
	return brainSize();
}

