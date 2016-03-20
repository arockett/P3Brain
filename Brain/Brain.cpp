//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "Brain.h"


string& AbstractBrain::brainTypeStr = Parameters::register_parameter("brainType", (string) "Markov", "brain to be used in evolution loop, [Markov, Wire]", "BRAIN");  // string parameter for outputMethod;
int& AbstractBrain::hiddenNodes = Parameters::register_parameter("hiddenNodes", 8, "number of hidden nodes, if brain type supports hiden nodes", "BRAIN");  // string parameter for outputMethod;
