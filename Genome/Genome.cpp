//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "../Global.h"
#include "Genome.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

int& Genome::initialChromosomeSize = Parameters::register_parameter("chromosomeSizeInitial", 1000, "starting size for all chromosomes in genome (genome size will be chromosomeSizeInitial * number of chromosomes * ploidy)", "GENOME");
double& Genome::pointMutationRate = Parameters::register_parameter("pointMutationRate", 0.005, "per site mutation rate", "GENOME");
double& Genome::insertionRate = Parameters::register_parameter("mutationCopyRate", 0.00002, "per genome insertion/deletion rate", "GENOME");
int& Genome::insertionMinSize = Parameters::register_parameter("mutationCopyMinSize", 10, "minimum size of insertion mutation", "GENOME");
int& Genome::insertionMaxSize = Parameters::register_parameter("mutationCopyMaxSize", 200, "maximum size of insertion mutation", "GENOME");
double& Genome::deletionRate = Parameters::register_parameter("mutationDeletionRate", 0.00002, "insertion rate per 1000 genome sites", "GENOME");
int& Genome::deletionMinSize = Parameters::register_parameter("mutationDeletionMinSize", 10, "minimum size of insertion mutation", "GENOME");
int& Genome::deletionMaxSize = Parameters::register_parameter("mutationDeletionMaxSize", 200, "maximum size of insertion mutation", "GENOME");
int& Genome::maxChromosomeSize = Parameters::register_parameter("chromosomeSizeMin", Genome::deletionMaxSize * 2, "if a chromosome is smaller then this, mutations will only increase chromosome size", "GENOME");
int& Genome::minChromosomeSize = Parameters::register_parameter("chromosomeSizeMax", 20000, "if a chromosome is larger then this, mutations will only decrease chromosome size", "GENOME");

int& Genome::crossCount = Parameters::register_parameter("genomecrossCount", 3, "number of crosses when performing crossover", "GENOME");

vector<string> Genome::genomeFileColumns = { "update", "ID", "sitesCount", "chromosomeCount", "alphabetSize", "ploidy" , "chromosomeLengths", "sites"};

