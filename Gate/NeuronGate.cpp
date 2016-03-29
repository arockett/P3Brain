//
//  Gate.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

//#include <iostream>
//
#include "NeuronGate.h"


const int& NeuronGate::defaultNumInputsMin = Parameters::register_parameter("neuronGate_numInputsMin", 3, "min number of inputs to neuronGate", "GATES - NEURON");
const int& NeuronGate::defaultNumInputsMax = Parameters::register_parameter("neuronGate_numInputsMax", 3, "max number of inputs to neuronGate", "GATES - NEURON");
const int& NeuronGate::defaultDischargeBehavior = Parameters::register_parameter("dischargeBehavior", 0, "what happens when gate fires (-1: let genome decide, 0: clear charge, 1: 'reduce' by delivery charge, 2: 1/2 charge)", "GATES - NEURON");
const double& NeuronGate::defaultThresholdMin = Parameters::register_parameter("neuronGate_thresholdMin", -1.0, "lowest possible value for threshold", "GATES - NEURON");
const double& NeuronGate::defaultThresholdMax = Parameters::register_parameter("neuronGate_thresholdMax", 1.0, "highest possible value for threshold", "GATES - NEURON");
const bool& NeuronGate::defaultAllowRepression = Parameters::register_parameter("neuronGate_allowRepression", false, "if false, neuronGates only fire if charge exceeds threshold; if true, neuronGates can also exist which always fire, except when charge exceeds threshold", "GATES - NEURON");
const double& NeuronGate::defaultDecayRateMin = Parameters::register_parameter("neuronGate_decayRateMin", 0.0, "amount charge will decay (towards 0) per update", "GATES - NEURON");
const double& NeuronGate::defaultDecayRateMax = Parameters::register_parameter("neuronGate_decayRateMax", 0.0, "amount charge will decay (towards 0) per update", "GATES - NEURON");
const double& NeuronGate::defaultDeliveryChargeMin = Parameters::register_parameter("neuronGate_deliveryChargeMin", -1.0, "output charge", "GATES - NEURON");
const double& NeuronGate::defaultDeliveryChargeMax = Parameters::register_parameter("neuronGate_deliveryChargeMax", 1.0, "output charge", "GATES - NEURON");
const double& NeuronGate::defaultDeliveryError = Parameters::register_parameter("neuronGate_deliveryError", 0.0, "error in deliveryCharge... charge -= random[0,deliveryError)", "GATES - NEURON");


const bool& NeuronGate::defaultThresholdFromNode = Parameters::register_parameter("neuronGate_thresholdFromNode", false, "if true, gate will have additional input, which will be used as threshold", "GATES - NEURON");
const bool& NeuronGate::defaultDeliveryChargeFromNode = Parameters::register_parameter("neuronGate_deliveryChargeFromNode", false, "if true, gate will have additional input, which will be used as deliveryCharge", "GATES - NEURON");
