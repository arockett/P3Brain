//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Brain__
#define __BasicMarkovBrainTemplate__Brain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <vector>

#include "../Gate/Gate_Builder.h"
#include "../GateListBuilder/GateListBuilder.h"
#include "../Genome/Genome.h"

#include "../Utilities/Parameters.h"

using namespace std;
class AbstractBrain {
 public:
  AbstractBrain() = default;
  virtual ~AbstractBrain() = default;
  virtual void update() = 0;
  virtual string description() = 0;  // returns a desription of this brain in it's current state
  virtual vector<string> getStats() =0;  // returns a vector of string pairs of any stats that can then be used for data tracking (etc.)
};

class ClassicBrain : AbstractBrain {
 protected:
  vector<shared_ptr<Gate>> gates;
  shared_ptr<Base_GateListBuilder> GLB;

 public:
  static int& defaultNrOfBrainStates;  // default value for number of states in all brains
  int nrOfBrainStates;  // the number of states in THIS brain
  static bool& serialProcessing;  // write directly states (overwrite) - do not use nextStates

  static void initializeParameters();
  static vector<int> defaultNodeMap;

  /*
   * Builds a look up table to convert genome site values into brain state addresses - this is only used when there is a fixed number of brain states
   * if there is a variable number of brain states, then the node map must be rebuilt.
   */
  static int makeNodeMap(vector<int> & nodeMap, int sizeInBits, int defaultNrOfBrainStates) {
    for (int i = 0; i < pow(2, (sizeInBits)); i++) {  // each site in the genome has 8 bits so we need to count though (  2 to the (8 * number of sites)  )
      nodeMap.push_back(i % defaultNrOfBrainStates);
    }

    return 1;
  }

  //Genome *genome;
  vector<double> states;
  vector<double> nextStates;

  ClassicBrain() = delete;

  ClassicBrain(shared_ptr<Base_GateListBuilder> _GLB, int _nrOfStates = defaultNrOfBrainStates);
  ClassicBrain(shared_ptr<Base_GateListBuilder> _GLB, shared_ptr<Genome> startGenome, int _nrOfStates = defaultNrOfBrainStates);
  virtual ~ClassicBrain() = default;

  virtual void update();

  void inOutReMap();

  virtual shared_ptr<ClassicBrain> makeBrainFromGenome(shared_ptr<Genome> _genome);

  virtual string description();
  virtual vector<string> getStats();

  virtual int IntFromState(vector<int> I);
  virtual int IntFromAllStates();
  virtual void resetBrain();
  virtual string gateList();
  virtual vector<vector<int>> getConnectivityMatrix();
  virtual int brainSize();
  //set<int> findCodingRegions(int mask);
  int numGates();

 public:
  inline void setState(const int& state, const double& value) {
    if (state < (int) states.size()) {
      states[state] = value;
    } else {
      cout << "Writing to invalid brain state - this brain needs more states!\nExiting\n";
      exit(1);
    }
  }
  inline double getState(const int& state) {
    if (state < (int) states.size()) {
      return states[state];
    } else {
      cout << "Reading from invalid brain state - this brain needs more states!\nExiting\n";
      exit(1);
    }
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__Brain__) */
