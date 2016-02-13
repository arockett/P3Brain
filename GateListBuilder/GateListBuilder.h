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
		auto genomeHandler = genome->newHandler(genome,readForward);
		auto saveHandler = genome->newHandler(genome,readForward);
		auto testHandler = genome->newHandler(genome,readForward);
		int gateCount = 0;
		while (!translation_Complete) {  // while there are sites in the genome
			//cout << "\tstart translation loop"<<endl;
			genomeHandler->copyTo(testHandler);  // get to values from genome to test for start codns
			const int testSite1Value = testHandler->readInt(0, 255);  // extract first 1/2 of startcodon
			testHandler->copyTo(saveHandler);  // save this index, this is where we pick up when we come back from building a gate.
			//saveHandler->printIndex();
			const int testSite2Value = testHandler->readInt(0, 255);  // extract second 1/2 of startcodon
			//cout << "/tfinished read testSite values:" << testSite1Value << "   " << testSite2Value << endl;
			//cout << "testValues: " << testSite1Value << "  " << testSite2Value << "\n";
			if (saveHandler->atEOG()) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
				translation_Complete = true;
				//cout << "\treached EOG"<<endl;
			} else if (testSite1Value + testSite2Value == 255) {  // if we found a start codon
				//cout << "\tfound start codon" << endl;
				if (Gate_Builder::makeGate[testSite1Value] != nullptr) {  // and that start codon codes to an in use gate class
					//cout << "\tfound valid gate" << endl;
					//cout << "found gate : " << testSite1Value << " " << testSite2Value << endl;
					genomeHandler->readInt(0, 255, Gate::START_CODE, gateCount);  // mark start codon in genomes coding region
					genomeHandler->readInt(0, 255, Gate::START_CODE, gateCount);  // mark start codon in genomes coding region
					//cout << "\tcalling gate constructor"<<endl;
					gates.push_back(Gate_Builder::makeGate[testSite1Value](genome, genomeHandler, gateCount));  // make a gate of the type associated with the value in testSite1Value
					//cout << "\tback from gate constructor"<<endl;
					gateCount++;
				}
			}
			saveHandler->copyTo(genomeHandler);
			//genomeHandler->printIndex();
		}
		return gates;
	}

};

#endif /* defined(__BasicMarkovBrainTemplate__GateListBuilder__) */
