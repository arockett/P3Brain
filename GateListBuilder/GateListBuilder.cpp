//
//  GateListBuilder.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "GateListBuilder.h"

vector<shared_ptr<AbstractGate>> ClassicGateListBuilder::buildGateListAndGetAllValues(shared_ptr<AbstractGenome> genome, int nrOfBrainStates, int maxValue, vector<int> &genomeHeadValues, int genomeHeadValuesCount, vector<vector<int>> &genomePerGateValues, int genomePerGateValuesCount) {

	vector<shared_ptr<AbstractGate>> gates;
	int codonMax = (1 << *Global::bitsPerCodon) - 1;
	bool mustReadAll = codonMax > genome->getAlphabetSize();

	bool translation_Complete = false;

	if (genome->isEmpty()) {
		translation_Complete = true;
	} else {

		bool readForward = true;
		auto genomeHandler = genome->newHandler(genome, readForward);
		auto placeHolderGenomeHandler = genome->newHandler(genome, readForward);
		auto gateGenomeHandler = genome->newHandler(genome, readForward);

		// save start of genome info
		int i = 0;
		while (i < genomeHeadValuesCount && !genomeHandler->atEOC()) {
			genomeHeadValues.push_back(gateGenomeHandler->readInt(0, maxValue));
			i++;
		}

		int gateCount = 0;

		int testSite1Value, testSite2Value;
		testSite1Value = genomeHandler->readInt(0, codonMax);
		testSite2Value = genomeHandler->readInt(0, codonMax);
		while (!translation_Complete) {
			if (genomeHandler->atEOC()) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
				if (genomeHandler->atEOG()) {
					translation_Complete = true;
				}
				genomeHandler->resetHandlerOnChromosome();  // reset to start of this chromosome
				genomeHandler->copyTo(placeHolderGenomeHandler);  // move placeholder to the next chromosome aswell so mustReadAll method works
				testSite2Value = genomeHandler->readInt(0, codonMax);  // place first value in new chromosome in testSite2 so !mustReadAll method works
			} else if (Gate_Builder::gateStartCodes[testSite1Value].size() != 0 && Gate_Builder::gateStartCodes[testSite1Value][1] == testSite2Value) {  // if we found a start codon
					genomeHandler->copyTo(gateGenomeHandler);
					gateGenomeHandler->toggleReadDirection();
					gateGenomeHandler->readInt(0, codonMax);  // move back 2 start codon values
					gateGenomeHandler->readInt(0, codonMax);
					gateGenomeHandler->toggleReadDirection();  // reverse the read direction again
					gateGenomeHandler->readInt(0, codonMax, AbstractGate::START_CODE, gateCount);  // mark start codon in genomes coding region
					gateGenomeHandler->readInt(0, codonMax, AbstractGate::START_CODE, gateCount);
					shared_ptr<AbstractGate> newGate = Gate_Builder::makeGate[testSite1Value](gateGenomeHandler, gateCount);

					if (newGate != nullptr) {
						// now read perGate values from genome
						vector<int> thisGatesValues;
						int i = 0;
						while (i < genomePerGateValuesCount && !gateGenomeHandler->atEOC()) {
							thisGatesValues.push_back(gateGenomeHandler->readInt(0, maxValue));
							i++;
						}
						if (!gateGenomeHandler->atEOC()) {  // we may run out of space while reading the perGate sites...
							gates.push_back(newGate);
							genomePerGateValues.push_back(thisGatesValues);
						}
					}
					gateCount++;
			}
			if (mustReadAll) {  // if start codon values are bigger then the alphabetSize of the genome, we must step forward one genome site at a time (slow)
				placeHolderGenomeHandler->advanceIndex();
				placeHolderGenomeHandler->copyTo(genomeHandler);
				testSite1Value = genomeHandler->readInt(0, codonMax);
				testSite2Value = genomeHandler->readInt(0, codonMax);
			} else {  // we know that start codon values fit in a single site, so we can be clever
				testSite1Value = testSite2Value;
				testSite2Value = genomeHandler->readInt(0, codonMax);
			}
			//cout << testSite1Value << " + " << testSite2Value << " = " << testSite1Value + testSite2Value << endl;
		}
	}
//cout << "Leaving GLB\n";

	return gates;
}

