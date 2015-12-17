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
  virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<Genome> genome, int nrOfBrainStates){
    vector<shared_ptr<Gate>> gates;
    return gates;
  }

};

class Classic_GateListBuilder : public Base_GateListBuilder {
 public:

  Classic_GateListBuilder() = default;
  virtual ~Classic_GateListBuilder() = default;

  virtual vector<shared_ptr<Gate>> buildGateList(shared_ptr<Genome> genome,int nrOfBrainStates){
    vector<shared_ptr<Gate>> gates;
    bool translation_Complete = false;
    if (genome->getSize() == 0){
      translation_Complete = true;
    }
    int genomeIndex = 0;
    int saveIndex = 0;
    int testIndex;
    while (!translation_Complete) {  // while there are sites in the genome
      testIndex = genomeIndex;  // get to values from genome to test for start codns
      const int testSite1Value = genome->extractValue(testIndex, { 0, 255 });  // extract first 1/2 of startcodon
      saveIndex = testIndex;  // save this index, this is where we pick up when we come back from building a gate.
      const int testSite2Value = genome->extractValue(testIndex, { 0, 255 });  // extract second 1/2 of startcodon
      if (genomeIndex > testIndex) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
        translation_Complete = true;
      } else if (testSite1Value + testSite2Value == 255) {  // if we found a start codon
        if (Gate_Builder::makeGate[testSite1Value] != nullptr) {  // and that start codon codes to an in use gate class
          genome->extractValue(genomeIndex, { 0, 255 }, Genome::START_CODE);  // mark start codon in genomes coding region
          genome->extractValue(genomeIndex, { 0, 255 }, Genome::START_CODE);  // mark start codon in genomes coding region
          gates.push_back(Gate_Builder::makeGate[testSite1Value](genome, genomeIndex));  // make a gate of the type associated with the value in testSite1Value
        }
      }
      genomeIndex = saveIndex;
    }
    return gates;
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__GateListBuilder__) */
