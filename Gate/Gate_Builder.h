#ifndef __BasicMarkovBrainTemplate__Gate_Builder__
#define __BasicMarkovBrainTemplate__Gate_Builder__

#include "Gate.h"
#include "Feedback_Gate.h"
#include "GP_Gate.h"
#include "Threshold_Gate.h"

class Gate_Builder {  // abstact class. Assumes that a standard genome is being used.
 public:

  static bool& usingProbGate;
  static bool& usingDetGate;
  static bool& usingEpsiGate;
  static bool& usingVoidGate;

  static bool& usingFBGate;
  static bool& usingGPGate;
  static bool& usingThGate;


  static void AddGate(int ID, function<shared_ptr<Gate>(shared_ptr<Genome>, int)> theFunction);
  static void setupGates();
  static function<shared_ptr<Gate>(shared_ptr<Genome>, int)> makeGate[256];

  Gate_Builder();
  ~Gate_Builder();
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate_Builder__) */

