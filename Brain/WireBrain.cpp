//
//  Agent.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"
#include "WireBrain.h"

int& WireBrain::defaultWidth = Parameters::register_parameter("width", 10, "width of the wire brain cube", "BRAIN - WIRE");
int& WireBrain::defaultHeight = Parameters::register_parameter("height", 10, "height of the wire brain cube", "BRAIN - WIRE");
int& WireBrain::defaultDepth = Parameters::register_parameter("depth", 10, "depth of the wire brain cube", "BRAIN - WIRE");
int& WireBrain::defaultNrOfBrainNodes = Parameters::register_parameter("NrOfBrainNodes", 15, "number of nodes (I/O)", "BRAIN - WIRE");
int& WireBrain::worldConnectionsSeparation = Parameters::register_parameter("worldConnectionsSeparation", 3, "distance between nodes input and output connections (I/O)", "BRAIN - WIRE");
double& WireBrain::initialFillRatio = Parameters::register_parameter("initialFillRatio", .25, "amount of the brain that will initially be wire vs. empty", "BRAIN - WIRE");
int& WireBrain::overchargeThreshold = Parameters::register_parameter("overchargeThreshold", 3, "If a cell receives this much charge or more, it will not charge", "BRAIN - WIRE");
int& WireBrain::chargeUpdatesPerUpdate = Parameters::register_parameter("chargeUpdatesPerUpdate", 100, "Number of charge updates per brain update", "BRAIN - WIRE");
bool& WireBrain::constantInputs = Parameters::register_parameter("constantInputs", true, "if true, input values are reset every charge update, if not, input values are set on first charge update only.", "BRAIN - WIRE");


int& WireBrain2::defaultWidth = Parameters::register_parameter("width2", 10, "width of the wire brain cube", "BRAIN - WIRE2");
int& WireBrain2::defaultHeight = Parameters::register_parameter("height2", 10, "height of the wire brain cube", "BRAIN - WIRE2");
int& WireBrain2::defaultDepth = Parameters::register_parameter("depth2", 10, "depth of the wire brain cube", "BRAIN - WIRE2");
int& WireBrain2::defaultNrOfBrainNodes = Parameters::register_parameter("NrOfBrainNodes2", 15, "number of nodes (I/O)", "BRAIN - WIRE2");
int& WireBrain2::worldConnectionsSeparation = Parameters::register_parameter("worldConnectionsSeparation2", 3, "distance between nodes input and output connections (I/O)", "BRAIN - WIRE2");
double& WireBrain2::initialFillRatio = Parameters::register_parameter("initialFillRatio2", .25, "amount of the brain that will initially be wire vs. empty", "BRAIN - WIRE2");
int& WireBrain2::overchargeThreshold = Parameters::register_parameter("overchargeThreshold2", 3, "If a cell receives this much charge or more, it will not charge", "BRAIN - WIRE2");
int& WireBrain2::chargeUpdatesPerUpdate = Parameters::register_parameter("chargeUpdatesPerUpdate2", 100, "Number of charge updates per brain update", "BRAIN - WIRE2");
bool& WireBrain2::constantInputs = Parameters::register_parameter("constantInputs2", true, "if true, input values are reset every charge update, if not, input values are set on first charge update only.", "BRAIN - WIRE2");

