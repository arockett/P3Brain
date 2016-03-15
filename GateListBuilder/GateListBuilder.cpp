//
//  GateListBuilder.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "GateListBuilder.h"

vector<shared_ptr<Gate>> Classic_GateListBuilder::buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates) {
	//cout << "In vector<shared_ptr<Gate>> buildGateList(shared_ptr<AbstractGenome> genome, int nrOfBrainStates)" << endl;
	vector<shared_ptr<Gate>> gates;
	int codonMax = (1 << Global::bitsPerCodon) - 1;
	bool translation_Complete = false;
	if (genome->isEmpty()) {
		translation_Complete = true;
	} else {

		bool readForward = true;
		auto genomeHandler = genome->newHandler(genome, readForward);
		auto gateGenomeHandler = genome->newHandler(genome, readForward);

		int gateCount = 0;

		int testSite1Value, testSite2Value;
		testSite1Value = genomeHandler->readInt(0, codonMax);
		testSite2Value = genomeHandler->readInt(0, codonMax);
		while (!translation_Complete) {
			if (genomeHandler->atEOG()) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
				translation_Complete = true;
			} else if (testSite1Value + testSite2Value == codonMax) {  // if we found a start codon
				if (Gate_Builder::makeGate[testSite1Value] != nullptr) {  // and that start codon codes to an in use gate class
					genomeHandler->copyTo(gateGenomeHandler);
					gateGenomeHandler->toggleReadDirection();
					gateGenomeHandler->readInt(0, codonMax);  // move back 2 start codon values
					gateGenomeHandler->readInt(0, codonMax);
					gateGenomeHandler->toggleReadDirection();  // reverse the read direction again
					gateGenomeHandler->readInt(0, codonMax, Gate::START_CODE, gateCount);  // mark start codon in genomes coding region
					gateGenomeHandler->readInt(0, codonMax, Gate::START_CODE, gateCount);
					shared_ptr<Gate> newGate = Gate_Builder::makeGate[testSite1Value](gateGenomeHandler, gateCount);
					if (newGate != nullptr) {
						gates.push_back(newGate);  // make a gate of the type associated with the value in testSite1Value
						gateCount++;
					}
				}
			}
//			testSite1Value = testSite2Value;
//			testSite2Value = genomeHandler->readInt(0, 255);
			genomeHandler->toggleReadDirection();
			genomeHandler->readInt(0, codonMax);  // move back 2 start codon values
			genomeHandler->readInt(0, codonMax);
			genomeHandler->toggleReadDirection();
			genomeHandler->advanceIndex();  // advance 1 index (might not be equal to a start codeon value (i.e. if we are reading from a bit genome)
			testSite1Value = genomeHandler->readInt(0, codonMax);
			testSite2Value = genomeHandler->readInt(0, codonMax);

			//cout << testSite1Value << " + " << testSite2Value << " = " << testSite1Value + testSite2Value << endl;

		}
	}
	//cout << "Leaving BLG\n";

	return gates;
}

