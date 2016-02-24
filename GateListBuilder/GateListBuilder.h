//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__GateListBuilder__
#define __BasicMarkovBrainTemplate__GateListBuilder__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../Gate/Gate_Builder.h"
#include "../Genome/Genome.h"

#include "../Utilities/Parameters.h"

using namespace std;
class Base_GateListBuilder {
 public:
	Base_GateListBuilder() = default;
	virtual ~Base_GateListBuilder() = default;
	virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates) {
		vector<shared_ptr<Gate>> gates;
		return gates;
	}

};

class Classic_GateListBuilder : public Base_GateListBuilder {
 public:

	Classic_GateListBuilder() = default;
	virtual ~Classic_GateListBuilder() = default;

	virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates) {
		//cout << "In virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates)"<<endl;
		vector<shared_ptr<Gate>> gates;

		bool translation_Complete = false;
		if (genome->isEmpty()) {
			translation_Complete = true;
		}

		bool readForward = true;
		auto genomeHandler = genome->newHandler(genome, readForward);
		auto gateGenomeHandler = genome->newHandler(genome, readForward);

		int gateCount = 0;

		int testSite1Value, testSite2Value;
		testSite1Value = genomeHandler->readInt(0, 255);
		testSite2Value = genomeHandler->readInt(0, 255);
		while (!translation_Complete) {
			if (genomeHandler->atEOG()) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
				translation_Complete = true;
			} else if (testSite1Value + testSite2Value == 255) {  // if we found a start codon
				if (Gate_Builder::makeGate[testSite1Value] != nullptr) {  // and that start codon codes to an in use gate class
					genomeHandler->copyTo(gateGenomeHandler);
					gateGenomeHandler->toggleReadDirection();
					gateGenomeHandler->readInt(0, 255);  // move back 2 start codon values
					gateGenomeHandler->readInt(0, 255);
					gateGenomeHandler->toggleReadDirection();  // reverse the read direction again
					gateGenomeHandler->readInt(0, 255, Gate::START_CODE, gateCount);  // mark start codon in genomes coding region
					gateGenomeHandler->readInt(0, 255, Gate::START_CODE, gateCount);

					gates.push_back(Gate_Builder::makeGate[testSite1Value](genome, gateGenomeHandler, gateCount));  // make a gate of the type associated with the value in testSite1Value
					gateCount++;
				}
				testSite1Value = testSite2Value;
			}
			testSite2Value = genomeHandler->readInt(0, 255);
		}
		return gates;
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__GateListBuilder__) */
