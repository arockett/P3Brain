//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "AbstractGenome.h"

const string& AbstractGenome::genomeTypeStr = Parameters::register_parameter("type", (string) "Multi", "genome to be used in evolution loop, [Multi, Circular]", "GENOME");  // string parameter for outputMethod;
const int& AbstractGenome::alphabetSize = Parameters::register_parameter("alphabetSize", 256, "alphabet size for genome", "GENOME");  // string parameter for outputMethod;
const string& AbstractGenome::genomeSitesType = Parameters::register_parameter("sitesType", (string) "char", "type for sites in genome [char, int, double, bool]", "GENOME");  // string parameter for outputMethod;






