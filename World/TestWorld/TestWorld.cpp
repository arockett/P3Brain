//
//  World.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "TestWorld.h"

shared_ptr<ParameterLink<int>> TestWorld::modePL = Parameters::register_parameter("WORLD_TEST-mode", 0, "0 = bit outputs before adding, 1 = add outputs");
shared_ptr<ParameterLink<int>> TestWorld::numberOfOutputsPL = Parameters::register_parameter("WORLD_TEST-numberOfOutputs", 10, "number of outputs in this world");

TestWorld::TestWorld() {
	mode = (PT == nullptr) ? modePL->lookup() : PT->lookupDouble("WORLD_TEST-mode");
	numberOfOutputs = (PT == nullptr) ? numberOfOutputsPL->lookup() : PT->lookupDouble("WORLD_TEST-numberOfOutputs");

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void TestWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {

	org->brain->resetBrain();
	org->brain->setInput(0, 1);  // give the brain a constant 1 (for wire brain)
	org->brain->update();
	double score = 0.0;
	for (int i = 0; i < org->brain->nrOutNodes; i++) {
		if (mode == 0) {
			score += Bit(org->brain->readOutput(i));
		} else {
			score += org->brain->readOutput(i);
		}
	}
	if (score < 0.0) {
		score = 0.0;
	}
	org->score = score;
	org->dataMap.Append("allscore", score);
}

int TestWorld::requiredInputs() {
	return 1;
}
int TestWorld::requiredOutputs() {
	return numberOfOutputs;
}
int TestWorld::maxOrgsAllowed() {
	return 1;
}
int TestWorld::minOrgsAllowed() {
	return 1;
}

