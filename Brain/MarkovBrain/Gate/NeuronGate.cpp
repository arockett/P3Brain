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

shared_ptr<int> NeuronGate::defaultNumInputsMin = Parameters::register_parameter("GATES_NEURON-numInputsMin", 3, "min number of inputs to neuronGate");
shared_ptr<int> NeuronGate::defaultNumInputsMax = Parameters::register_parameter("GATES_NEURON-numInputsMax", 3, "max number of inputs to neuronGate");
shared_ptr<int> NeuronGate::defaultDischargeBehavior = Parameters::register_parameter("GATES_NEURON-dischargeBehavior", 0, "what happens when gate fires (-1: let genome decide, 0: clear charge, 1: 'reduce' by delivery charge, 2: 1/2 charge)");
shared_ptr<double> NeuronGate::defaultThresholdMin = Parameters::register_parameter("GATES_NEURON-thresholdMin", -1.0, "lowest possible value for threshold");
shared_ptr<double> NeuronGate::defaultThresholdMax = Parameters::register_parameter("GATES_NEURON-thresholdMax", 1.0, "highest possible value for threshold");
shared_ptr<bool> NeuronGate::defaultAllowRepression = Parameters::register_parameter("GATES_NEURON-allowRepression", false, "if false, neuronGates only fire if charge exceeds threshold; if true, neuronGates can also exist which always fire, except when charge exceeds threshold");
shared_ptr<double> NeuronGate::defaultDecayRateMin = Parameters::register_parameter("GATES_NEURON-decayRateMin", 0.0, "amount charge will decay (towards 0) per update");
shared_ptr<double> NeuronGate::defaultDecayRateMax = Parameters::register_parameter("GATES_NEURON-decayRateMax", 0.0, "amount charge will decay (towards 0) per update");
shared_ptr<double> NeuronGate::defaultDeliveryChargeMin = Parameters::register_parameter("GATES_NEURON-deliveryChargeMin", -1.0, "output charge");
shared_ptr<double> NeuronGate::defaultDeliveryChargeMax = Parameters::register_parameter("GATES_NEURON-deliveryChargeMax", 1.0, "output charge");
shared_ptr<double> NeuronGate::defaultDeliveryError = Parameters::register_parameter("GATES_NEURON-deliveryError", 0.0, "error in deliveryCharge... charge -= random[0,deliveryError)");


shared_ptr<bool> NeuronGate::defaultThresholdFromNode = Parameters::register_parameter("GATES_NEURON-thresholdFromNode", false, "if true, gate will have additional input, which will be used as threshold");
shared_ptr<bool> NeuronGate::defaultDeliveryChargeFromNode = Parameters::register_parameter("GATES_NEURON-deliveryChargeFromNode", false, "if true, gate will have additional input, which will be used as deliveryCharge");
