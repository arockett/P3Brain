//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>

#include "Gate.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

double& FixedEpsilonGate::FixedEpsilonGate_Probability = Parameters::register_parameter("fixedEpsilonGate_Probability", 0.05, "chance that an output from a FixedEpsilonGate gate will be randomized", "GATES - FIXED EPSILON");
double& VoidGate::voidGate_Probability = Parameters::register_parameter("voidGate_Probability", 0.05, "chance that an output from a void gate will be set to 0", "GATES - VOID");

// *** General tools for All Gates ***

// Gets "howMany" addresses, advances the genome_index buy "howManyMax" addresses and updates "codingRegions" with the addresses being used.
void Gate::getSomeBrainAddresses(const int& howMany, const int& howManyMax, vector<int>& addresses, shared_ptr<Genome::Index> genome_index, shared_ptr<Genome> genome, int code) {
  int i;
  for (i = 0; i < howMany; i++) {  // for the number of addresses we need
    addresses[i] = genome->extractValue(genome_index, { 0, (1 << Global::bitsPerBrainAddress) - 1 }, code);  // get an address
    //cout << addresses[i] << "\n";
  }
  while (i < howManyMax) {  // leave room in the genome in case this gate gets more IO later
    //cout << "skipping: " << i << "\n";
    genome->extractValue(genome_index, { 0, (1 << Global::bitsPerBrainAddress) - 1 }, -1);
    i++;
  }
}

// given a genome and a genomeIndex:
// pull out the number a number of inputs, number of outputs and then that many inputs and outputs
// if number of inputs or outputs is less then the max possible inputs or outputs skip the unused sites in the genome
void Gate::getInputsAndOutputs(const vector<int> insRange, vector<int> outsRange, shared_ptr<Genome::Index> genomeIndex, shared_ptr<Genome> genome) {  // (max#in, max#out,currentIndexInGenome,genome,codingRegions)

  int numInputs = genome->extractValue(genomeIndex, { insRange[0], insRange[1] }, Genome::IN_COUNT_CODE);
  //cout << "num_Inputs: " << numInputs << "\n";
  int numOutputs = genome->extractValue(genomeIndex, { outsRange[0], outsRange[1] }, Genome::OUT_COUNT_CODE);
  //cout << "num_Outputs: " << numOutputs << "\n";

  inputs.resize(numInputs);
  outputs.resize(numOutputs);
  if (insRange[1] > 0) {
    getSomeBrainAddresses(numInputs, insRange[1], inputs, genomeIndex, genome, Genome::IN_ADDRESS_CODE);
  }
  if (outsRange[1] > 0) {
    getSomeBrainAddresses(numOutputs, outsRange[1], outputs, genomeIndex, genome, Genome::OUT_ADDRESS_CODE);
  }
}

// converts values attained from genome for inputs and outputs to vaild brain state ids
// uses nodeMap to accomplish the remaping
void Gate::applyNodeMap(vector<int> nodeMap, int maxNodes) {
  for (size_t i = 0; i < inputs.size(); i++) {
    inputs[i] = nodeMap[inputs[i]] % maxNodes;
  }
  for (size_t i = 0; i < outputs.size(); i++) {
    outputs[i] = nodeMap[outputs[i]] % maxNodes;
  }
}

/* useful function if you need to look up values from a table during update.
 * convert the inputs to an index by reading them one at a time and bitshifting the result
 * the first input is read last so that 1, 10 , 100 (etc.) all return 1. (01)
 * and 01, 010, 0100 (etc.) all return 2 (10)
 * and 11, 110, 1100 (etc.) all return 3 (11)
 * etc... hopefully you see the pattern
 * This, btw, will maintain output consistency for these lower values even if a gate gains or loses inputs.
 */
int Gate::getIndexFromInputs(vector<double> & brainState) {
  int index = 0;

  for (int i = (int) inputs.size() - 1; i >= 0; i--) {
    index = (index << 1) + Bit(brainState[inputs[i]]);
  }
  return index;
}

void Gate::resetGate(void) {
  //nothing to reset here!
}

vector<int> Gate::getIns() {
  return inputs;
}

vector<int> Gate::getOuts() {
  return outputs;
}

string Gate::description() {
  string S = " Gate\n";
  S = S + "IN:";
  for (size_t i = 0; i < inputs.size(); i++)
    S = S + " " + to_string(inputs[i]);
  S = S + "\n";
  S = S + "OUT:";
  for (size_t i = 0; i < outputs.size(); i++)
    S = S + " " + to_string(outputs[i]);
  S = S + "\n";
  //S = S + getCodingRegions();
  return S;
}

/* *** ProbilisticGate implementation *** */

ProbabilisticGate::ProbabilisticGate(shared_ptr<Genome> genome, shared_ptr<Genome::Index> genomeIndex) {
  int i, j;
  inputs.clear();
  outputs.clear();
  int numOutputs, numInputs;

  // get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
  getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome);  // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
  numInputs = inputs.size();
  numOutputs = outputs.size();

  // get a table filled with values from the genome that has
  // rows = (the number of possible combinations of input values) and columns = (the number of possible combinations of output values)
  vector<vector<int>> rawTable = genome->extractTable(genomeIndex, { 1 << numInputs, 1 << numOutputs }, { 16, 16 }, { 0, 255 });

  table.resize(1 << numInputs);
  //normalize each row
  for (i = 0; i < (1 << numInputs); i++) {  //for each row (each possible input bit string)
    table[i].resize(1 << numOutputs);
    // first sum the row
    double S = 0;
    for (j = 0; j < (1 << numOutputs); j++) {
      S += (double)rawTable[i][j];
    }
    // now normalize the row
    if (S == 0.0) {  //if all the inputs on this row are 0, then give them all a probability of 1/(2^(number of outputs))
      for (j = 0; j < (1 << numOutputs); j++)
        table[i][j] = 1.0 / (double) (1 << numOutputs);
    } else {  //otherwise divide all values in a row by the sum of the row
      for (j = 0; j < (1 << numOutputs); j++)
        table[i][j] = (double)rawTable[i][j] / S;
    }
  }

}

void ProbabilisticGate::update(vector<double> & states, vector<double> & nextStates) {  //this translates the input bits of the current states to the output bits of the next states
  int input = getIndexFromInputs(states);  // converts the input values into an index
  int outputColumn = 0;
  double r = Random::getDouble(1);  // r will determine with set of outputs will be chosen
  while (r > table[input][outputColumn]) {
    r -= table[input][outputColumn];  // this goes across the probability table in row for the given input and subtracts each
    // value in the table from r until r is less than a value it reaches
    outputColumn++;  // we have not found the correct output so move to the next output
  }
  for (size_t i = 0; i < outputs.size(); i++)  //for each output...
    nextStates[outputs[i]] += 1.0 * ((outputColumn >> (outputs.size() - 1 - i)) & 1);  // convert output (the column number) to bits and pack into next states
                                                                                       // but always put the last bit in the first input (to maintain consistancy)
}

string ProbabilisticGate::description() {
  string S = "ProbilisticGate" + Gate::description();
  return S;
}

/* *** Determistic Gate Implementation *** */

DeterministicGate::DeterministicGate(shared_ptr<Genome> genome, shared_ptr<Genome::Index> genomeIndex) {
  inputs.clear();
  outputs.clear();
  int numOutputs, numInputs;

  // get numInputs inputs and numOutputs outputs, advance k (genomeIndex) as if we had gotten 4 of each and record this in codingRegions
  getInputsAndOutputs( { 1, 4 }, { 1, 4 }, genomeIndex, genome);  // (insRange, outsRange,currentIndexInGenome,genome,codingRegions)
  numInputs = inputs.size();
  numOutputs = outputs.size();
  //cout << " after getIO index: " << genomeIndex << "\n";

  // get a table filled with values from the genome that has
  // rows = (the number of possible combinations of input values) and columns = (the number of possible combinations of output values)
  table = genome->extractTable(genomeIndex, { 1 << numInputs, numOutputs }, { 16, 4 }, { 0, 1 });
}

void DeterministicGate::setupForBits(int* Ins, int nrOfIns, int Out, int logic) {
  inputs.resize(nrOfIns);
  for (int i = 0; i < nrOfIns; i++)
    inputs[i] = Ins[i];
  outputs.resize(1);
  outputs[0] = Out;
  table.resize(1 << nrOfIns);
  for (int i = 0; i < (1 << nrOfIns); i++) {
    table[i].resize(1);
    table[i][0] = (logic >> i) & 1;
  }
}

void DeterministicGate::update(vector<double> & states, vector<double> & nextStates) {
  int input = getIndexFromInputs(states);  // converts the input values into an index
  for (size_t i = 0; i < outputs.size(); i++) {
    nextStates[outputs[i]] += table[input][i];
  }
}

string DeterministicGate::description() {
  return "Deterministic " + Gate::description();
}

/* *** Fixed Epison Gate *** */
/* this gate behaves like a deterministic gate with a constant externally set error which may cause the outputs to scramble */

FixedEpsilonGate::FixedEpsilonGate(shared_ptr<Genome> genome, shared_ptr<Genome::Index> genomeIndex)
    : DeterministicGate(genome, genomeIndex) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
  epsilon = FixedEpsilonGate_Probability;  // in case you want to have different epsilon for different gates (who am I to judge?)

  // now to the specifics of this gate - we convert the table to a list of numbers (i.e. bitstrings) so we can do fast comparisons in the update
  defaultOutput.clear();  // clear the defaultOutput vector... the list of bitstrings
  for (size_t i = 0; i < table.size(); i++) {  //for every row of the table (which is 2^inputs)
    int D = 0;
    for (size_t j = 0; j < table[i].size(); j++) {  //for every space in the row
      D |= (Bit(table[i][j]) << j);  //translate the value in that space to a bit (0 or 1), then bitshift it [position in row] times, then take D, bit-or it with that number,
      //and make that the new D
    }
    defaultOutput.push_back(D);  //add D (which is a bitstring of length [2^outputs]) to defaultOutput. defaultoutputs is now of length (2^number of inputs)
  }
}

void FixedEpsilonGate::update(vector<double> & states, vector<double> & nextStates) {
  int input = getIndexFromInputs(states);  // converts the input values into an index
  if (Random::P(epsilon)) {  //if there will be an error
    //do a random output excluding the one otherwise given
    int output = 0;
    do {
      output = Random::getIndex(table.size());  // pick a bitstring (with the correct number of bits) i.e. one bit for each output
    } while (output == defaultOutput[input]);  // if you happen to pick the a bit string which happens to match the correct output, pick again
    for (size_t i = 0; i < outputs.size(); i++) {
      nextStates[outputs[i]] += 1.0 * (double) ((output >> i) & 1);
    }
  } else {
    //do the default output
    for (size_t i = 0; i < outputs.size(); i++) {
      nextStates[outputs[i]] += table[input][i];
    }
  }
}

string FixedEpsilonGate::description() {
  return "Fixed Epsilon " + to_string(epsilon) + "\n " + Gate::description();
}

/* *** VoidGate *** */
/* this gate behaves like a deterministic gate with a constant externally set error which may set a single output to 0 */

VoidGate::VoidGate(shared_ptr<Genome> genome, shared_ptr<Genome::Index> genomeIndex)
    : DeterministicGate(genome, genomeIndex) {  // use DeterministicGate constructor to build set up everything (including a table of 0s and 1s)
  epsilon = voidGate_Probability;  // in case you want to have different epsilon for different gates (who am I to judge?)
}

void VoidGate::update(vector<double> & states, vector<double> & nextStates) {
  int input = getIndexFromInputs(states);  // converts the input values into an index
  vector<int> outputRow = table[input];

  if (Random::P(epsilon)) {
    outputRow[Random::getIndex(outputs.size())] = 0;  // pick one output randomly and set it to 0
  }
  for (size_t i = 0; i < outputs.size(); i++) {
    nextStates[outputs[i]] += outputRow[i];
  }
}

string VoidGate::description() {
  return "Void Gate " + to_string(epsilon) + "\n " + Gate::description();
}

