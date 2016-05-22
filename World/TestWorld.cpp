//
//  World.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "TestWorld.h"

//#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"

TestWorld::TestWorld() {
	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
}

// score is number of outputs set to 1 (i.e. output > 0) squared
void TestWorld::runWorldSolo(shared_ptr<Organism> org, bool analyse, bool visualize, bool debug) {

	org->brain->resetBrain();
	org->brain->setInput(0,1); // give the brain a constant 1 (for wire brain)
	org->brain->update();
	double score = 0.0;
	for (int i = 0; i < org->brain->nrOutNodes; i++) {
		score += Bit(org->brain->readOutput(i)) & 1;
	}
	if (score < 0.0) {
		score= 0.0;
	}
	org->score = score;
	org->dataMap.Append("allscore", score);
}

int TestWorld::requiredInputs() {
	return 1;
}
int TestWorld::requiredOutputs() {
	return 100;
}
 int TestWorld::maxOrgsAllowed()  {
	return 1;
}
 int TestWorld::minOrgsAllowed()  {
	return 1;
}

