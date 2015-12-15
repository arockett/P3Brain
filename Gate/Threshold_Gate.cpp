//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"
#include "Threshold_Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

Thresholdgate::Thresholdgate(shared_ptr<Genome> genome, int genomeIndex) {
  int i;
  inputs.clear();
  outputs.clear();
  int _xDim;


  genome->advanceIndexPastStartCodon(genomeIndex);

  //The dimensions are different here, the first inputs is the trigger
  //the next one are the current state Counter
  //the outputSize is the same, the first bit of the output is the ping
  //the others are the current state counter, they are the same as the inputs!
  //get the dimensions of the table
  // was : _xDim = 1 + (genome->sites[(k++) % genome->sites.size()] & 7);
  _xDim = genome->extractValue(genomeIndex, {1,8}, Genome::DATA_CODE);
  //prepare the containers for the inputs and outputs addresses
  inputs.resize(_xDim);
  outputs.resize(_xDim);

  // was : inputs[0] = genome->sites[k % genome->sites.size()];
  inputs[0] = genome->extractValue(genomeIndex, {0,255}, Genome::DATA_CODE);

  for (i = 1; i < _xDim; i++) {
    // was::
    //inputs[i] = genome->sites[(k + i) % genome->sites.size()];
    //outputs[i] = genome->sites[(k + i) % genome->sites.size()];
    inputs[i] = genome->extractValue(genomeIndex, {0,255}, Genome::DATA_CODE);
    outputs[i] = inputs[i];
  }

  //keep calm, i is at the right value
  //was : outputs[0] = genome->sites[(k + i) % genome->sites.size()];
  outputs[0] = outputs[i];

  //yes the threshold can be higher than the maximal number countable to by this threshold counter
  // was : threshold = genome->sites[(k + i + 1) % genome->sites.size()];
  threshold = genome->extractValue(genomeIndex, {0,255}, Genome::DATA_CODE);
}

void Thresholdgate::update(vector<double> & states, vector<double> & nextStates) {
  int theCount = 0;
  for (size_t i = 1; i < inputs.size(); i++)
    theCount = (theCount << 1) + Bit(states[inputs[i]]);
  theCount += Bit(states[inputs[0]]);
  if (theCount > threshold)
    states[outputs[0]] += 1.0;
  for (size_t i = 1; i < outputs.size(); i++)
    nextStates[outputs[i]] += 1.0 * ((theCount >> (i - 1)) & 1);
}

string Thresholdgate::description() {
  return "ThresholdgateGate: " + to_string(threshold) + "\n" + Gate::description();;
}

