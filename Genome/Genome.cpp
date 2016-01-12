//
//  sites.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "ByteGenome.h"
#include "../Global.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"


vector<vector<int>> Genome::extractTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code, int gateID) {
	vector<vector<int>> table;
	int x = 0;
	int y = 0;
	int Y = tableSize[0];
	int X = tableSize[1];
	int maxY = tableMaxSize[0];
	int maxX = tableMaxSize[1];

	table.resize(Y);  // set the number of rows in the table

	for (; y < (Y); y++) {
		table[y].resize(X);  // set the number of columns in this row
		for (x = 0; x < X; x++) {
			//table[y][x] = (Type) (sites[index]);
			table[y][x] = extractValue(index, valueRange, code, gateID);
		}
		for (; x < maxX; x++) {
			extractValue(index, valueRange);  // advance genomeIndex to account for unused entries in the max sized table for this row
		}
	}
	for (; y < (maxY); y++) {
		for (x = 0; x < maxX; x++) {
			extractValue(index, valueRange);  // advance to account for unused rows
		}
	}
	return table;
}

