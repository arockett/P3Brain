#include "Gate_Builder.h"

bool& Gate_Builder::usingProbGate = Parameters::register_parameter("probabilisticGate", false, "set to true to enable probabilistic gates", "GATE TYPES");
bool& Gate_Builder::usingDetGate = Parameters::register_parameter("deterministicGate", true, "set to true to enable deterministic gates?", "GATE TYPES");
bool& Gate_Builder::usingEpsiGate = Parameters::register_parameter("fixedEpsilonGate", false, "set to true to enable epsilon gates", "GATE TYPES");
bool& Gate_Builder::usingVoidGate = Parameters::register_parameter("voidGate", false, "set to true to enable void gates", "GATE TYPES");

bool& Gate_Builder::usingFBGate = Parameters::register_parameter("feedBackGate", false, "set to true to enable feedback gates", "GATE TYPES");
bool& Gate_Builder::usingGPGate = Parameters::register_parameter("geneticProgramingGate", false, "set to true to enable GP (what?) gates", "GATE TYPES");
bool& Gate_Builder::usingThGate = Parameters::register_parameter("thresholdGate", false, "set to true to enable threshold gates", "GATE TYPES");

//setupGates() populates Gate::makeGate (a structure containing functions) with the constructors for various types of gates.
//there are 256 possible gates identified each by a pair of codons (n followed by 256-n)
//after initializing Gate::MakeGate, Gate::AddGate() adds values and the associated constructor function to Gate::MakeGate
void Gate_Builder::setupGates() {
  for (int i = 0; i < 256; i++) {
    AddGate(i, nullptr);
  }
  if (usingProbGate) {
    AddGate(42, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<ProbabilisticGate>(genome,index);});
    Global::inUseGateTypes.insert(42);
  }
  if (usingDetGate) {
    AddGate(43, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<DeterministicGate>(genome,index);});
    Global::inUseGateTypes.insert(43);
  }
  if (usingEpsiGate) {
    AddGate(47, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<FixedEpsilonGate>(genome,index);});
    Global::inUseGateTypes.insert(47);
  }
  if (usingVoidGate) {
    AddGate(48, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<VoidGate>(genome,index);});
    Global::inUseGateTypes.insert(47);
  }
  if (usingFBGate) {
    AddGate(44, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<FeedbackGate>(genome,index);});
    Global::inUseGateTypes.insert(44);
  }
  if (usingGPGate) {
    AddGate(45, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<GPGate>(genome,index);});
    Global::inUseGateTypes.insert(45);
  }
  if (usingThGate) {
    AddGate(46, [](shared_ptr<Genome> genome,shared_ptr<Genome::Index> index) {return make_shared<Thresholdgate>(genome,index);});
    Global::inUseGateTypes.insert(46);
  }

}

/* *** some c++ 11 magic to speed up translation from genome to gates *** */
function<shared_ptr<Gate>(shared_ptr<Genome>, shared_ptr<Genome::Index>)> Gate_Builder::makeGate[256];

void Gate_Builder::AddGate(int ID, function<shared_ptr<Gate>(shared_ptr<Genome>, shared_ptr<Genome::Index> index)> theFunction) {
  makeGate[ID] = theFunction;
}
/* *** end - some c++ 11 magic to speed up translation from genome to gates *** */
