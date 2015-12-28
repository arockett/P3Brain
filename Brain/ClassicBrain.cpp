//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "ClassicBrain.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

vector<int> ClassicBrain::defaultNodeMap;

int& ClassicBrain::defaultNrOfBrainStates = Parameters::register_parameter("brainSize", 15, "number of Brain Values", "BRAIN");

bool& ClassicBrain::serialProcessing = Parameters::register_parameter("serialProcessing", false, "sets brains to overwrite... right?", "BRAIN");

ClassicBrain::ClassicBrain(shared_ptr<Base_GateListBuilder> _GLB, int _nrOfStates) {
	GLB = _GLB;
	nrOfBrainStates = _nrOfStates;
}

ClassicBrain::ClassicBrain(shared_ptr<Base_GateListBuilder> _GLB, shared_ptr<Genome> genome, int _nrOfBrainStates) {  //this is a constructor. it is run whenever a new brain is created.
	nrOfBrainStates = _nrOfBrainStates;
	states.resize(nrOfBrainStates);
	nextStates.resize(nrOfBrainStates);

	GLB = _GLB;

	gates = GLB->buildGateList(genome, nrOfBrainStates);

//  bool translation_Complete = false;
//  if (genome->getSize() == 0){
//    translation_Complete = true;
//  }
//  int genomeIndex = 0;
//  int saveIndex = 0;
//  int testIndex;
//  while (!translation_Complete) {  // while there are sites in the genome
//    testIndex = genomeIndex;  // get to values from genome to test for start codns
//    const int testSite1Value = genome->extractValue(testIndex, { 0, 255 });  // extract first 1/2 of startcodon
//    saveIndex = testIndex;  // save this index, this is where we pick up when we come back from building a gate.
//    const int testSite2Value = genome->extractValue(testIndex, { 0, 255 });  // extract second 1/2 of startcodon
//    if (genomeIndex > testIndex) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
//      translation_Complete = true;
//    } else if (testSite1Value + testSite2Value == 255) {  // if we found a start codon
//      if (Gate_Builder::makeGate[testSite1Value] != nullptr) {  // and that start codon codes to an in use gate class
//        genome->extractValue(genomeIndex, { 0, 255 }, Genome::START_CODE);  // mark start codon in genomes coding region
//        genome->extractValue(genomeIndex, { 0, 255 }, Genome::START_CODE);  // mark start codon in genomes coding region
//        gates.push_back(Gate_Builder::makeGate[testSite1Value](genome, genomeIndex));  // make a gate of the type associated with the value in testSite1Value
//      }
//    }
//    genomeIndex = saveIndex;
//  }
	inOutReMap();  // map ins and outs from genome values to brain states
}

shared_ptr<ClassicBrain> ClassicBrain::makeBrainFromGenome(shared_ptr<Genome> _genome) {
	shared_ptr<ClassicBrain> newBrain = make_shared<ClassicBrain>(GLB, _genome, ClassicBrain::defaultNrOfBrainStates);
	return newBrain;
}

void ClassicBrain::resetBrain() {
	for (int i = 0; i < nrOfBrainStates; i++)
		states[i] = 0.0;
	for (size_t i = 0; i < gates.size(); i++)
		gates[i]->resetGate();
}

void ClassicBrain::update() {
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
	nextStates.assign(nrOfBrainStates, 0.0);
	for (size_t i = 0; i < gates.size(); i++) {  //update each gate
		gates[i]->update(states, nextStates);
	}
	swap(states, nextStates);
}

void ClassicBrain::inOutReMap() {  // remaps genome site values to valid brain state addresses
	for (size_t i = 0; i < gates.size(); i++) {
		gates[i]->applyNodeMap(defaultNodeMap, nrOfBrainStates);
	}

}

int ClassicBrain::IntFromState(vector<int> I) {
	int r = 0;
	for (size_t i = 0; i < I.size(); i++)
		r = (r << 1) + Bit(states[I[i]]);
	return r;
}

int ClassicBrain::IntFromAllStates() {
	int r = 0;
	for (int i = 0; i < nrOfBrainStates; i++)
		r = (r << 1) + Bit(states[i]);
	return r;

}

string ClassicBrain::description() {
	string S = "Markov Briain\n" + gateList();
	return S;
}

vector<string> ClassicBrain::getStats() {
	vector<string> dataPairs;
	dataPairs.push_back("gates");
	dataPairs.push_back(to_string(gates.size()));
	return (dataPairs);
}

string ClassicBrain::gateList() {
	string S = "";
	for (size_t i = 0; i < gates.size(); i++) {
		S = S + gates[i]->description();
	}
	return S;
}

vector<vector<int>> ClassicBrain::getConnectivityMatrix() {
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

//set<int> Brain::findCodingRegions(int mask) {  //if mask=0, all coding regions are returned. if mask=1, return everything except start codon. if mask=2, return everything except start codon and wiring.
//  set<int> allCodingRegions;
//  for (auto gateIterator = gates.begin(); gateIterator != gates.end(); gateIterator++) {
//    for (auto iterator = (*gateIterator)->codingRegions.begin(); iterator != (*gateIterator)->codingRegions.end(); iterator++) {
//      if (iterator->second >= mask) {
//        allCodingRegions.insert(iterator->first);
//      }
//    }
//  }
//  return allCodingRegions;
//}

int ClassicBrain::brainSize() {
	return (int) gates.size();
}

int ClassicBrain::numGates() {
	return brainSize();
}

