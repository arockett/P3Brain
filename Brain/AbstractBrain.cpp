//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "AbstractBrain.h"

shared_ptr<string> AbstractBrain::brainTypeStr = Parameters::register_parameter("BRAIN-brainType", (string) "Markov", "brain to be used in evolution loop, [Markov, Wire, Human]");  // string parameter for outputMethod;
shared_ptr<int> AbstractBrain::hiddenNodes = Parameters::register_parameter("BRAIN-hiddenNodes", 8, "number of hidden nodes, if brain type supports hiden nodes");  // string parameter for outputMethod;
shared_ptr<bool> AbstractBrain::serialProcessing = Parameters::register_parameter("BRAIN-serialProcessing", false, "outputs from units will write to nodes, not nodesNext");
