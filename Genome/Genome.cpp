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

int& Genome::initialGenomeSize = Parameters::register_parameter("genomeSizeInitial", 5000, "starting size for genomes", "GENOME");
double& Genome::pointMutationRate = Parameters::register_parameter("pointMutationRate", 0.005, "per site mutation rate", "GENOME");
double& Genome::insertionRate = Parameters::register_parameter("mutationCopyRate", 0.02, "per genome insertion/deletion rate", "GENOME");
int& Genome::insertionMinSize = Parameters::register_parameter("mutationCopyMinSize", 10, "minimum size of insertion mutation", "GENOME");
int& Genome::insertionMaxSize = Parameters::register_parameter("mutationCopyMaxSize", 200, "maximum size of insertion mutation", "GENOME");
double& Genome::deletionRate = Parameters::register_parameter("mutationDeletionRate", 0.02, "insertion rate per 1000 genome sites", "GENOME");
int& Genome::deletionMinSize = Parameters::register_parameter("mutationDeletionMinSize", 10, "minimum size of insertion mutation", "GENOME");
int& Genome::deletionMaxSize = Parameters::register_parameter("mutationDeletionMaxSize", 200, "maximum size of insertion mutation", "GENOME");
int& Genome::minGenomeSize = Parameters::register_parameter("genomeSizeMin", 5000, "if the genome is smaller then this, mutations will only increse genome size", "GENOME");
int& Genome::maxGenomeSize = Parameters::register_parameter("genomeSizeMax", 20000, "if the genome is larger then this, mutations will only decrease genome size", "GENOME");
int& Genome::crossCount = Parameters::register_parameter("genomecrossCount", 3, "number of crosses when performing crossover", "GENOME");

//vector<vector<int>> Genome::extractTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code, int gateID) {
//	vector<vector<int>> table;
//	int x = 0;
//	int y = 0;
//	int Y = tableSize[0];
//	int X = tableSize[1];
//	int maxY = tableMaxSize[0];
//	int maxX = tableMaxSize[1];
//
//	table.resize(Y);  // set the number of rows in the table
//
//	for (; y < (Y); y++) {
//		table[y].resize(X);  // set the number of columns in this row
//		for (x = 0; x < X; x++) {
//			//table[y][x] = (Type) (sites[index]);
//			table[y][x] = extractValue(index, valueRange, code, gateID);
//		}
//		for (; x < maxX; x++) {
//			extractValue(index, valueRange);  // advance genomeIndex to account for unused entries in the max sized table for this row
//		}
//	}
//	for (; y < (maxY); y++) {
//		for (x = 0; x < maxX; x++) {
//			extractValue(index, valueRange);  // advance to account for unused rows
//		}
//	}
//	return table;
//}

//	shared_ptr<AbstractGenomeHandler> Genome::newHandler(shared_ptr<AbstractGenome> _genome) {
//		cout << " in Genome::newHandler(shared_ptr<Genome> _genome)\n";
//		shared_ptr<AbstractGenomeHandler> GH = make_shared<GenomeHandler>(_genome);
//
//		return GH;
//	}

