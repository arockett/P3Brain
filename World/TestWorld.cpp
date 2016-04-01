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
double TestWorld::testIndividual(shared_ptr<Organism> org, bool analyse, bool show) {
	org->brain->resetBrain();
	org->brain->setInput(0,1); // give the brain a constant 1 (for wire brain)
	org->brain->update();
	double w = 0.0;
	for (int i = 0; i < org->brain->nrOutNodes; i++) {
		w += Bit(org->brain->readOutput(i)) & 1;
	}
	return w * w;
}

int TestWorld::requiredInputs() {
	return 1;
}
int TestWorld::requiredOutputs() {
	return 100;
}
