//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "AbstractGenome.h"

shared_ptr<ParameterLink<string>> AbstractGenome::genomeTypeStrPL = Parameters::register_parameter("GENOME-type", (string) "Multi", "genome to be used in evolution loop, [Multi, Circular]");  // string parameter for outputMethod;
shared_ptr<ParameterLink<int>> AbstractGenome::alphabetSizePL = Parameters::register_parameter("GENOME-alphabetSize", 256, "alphabet size for genome");  // string parameter for outputMethod;
shared_ptr<ParameterLink<string>> AbstractGenome::genomeSitesTypePL = Parameters::register_parameter("GENOME-sitesType", (string) "char", "type for sites in genome [char, int, double, bool]");  // string parameter for outputMethod;






