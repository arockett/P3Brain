//
//  Agent.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__WireBrain__
#define __BasicMarkovBrainTemplate__WireBrain__

#include <math.h>
#include <memory>
#include <iostream>
#include <set>
#include <unordered_set>
#include <vector>

#include "../Genome/AbstractGenome.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"
#include "AbstractBrain.h"

// define the wire states

#define WIRE 1
#define HOLLOW 0

#define START_CODE 0
#define LOCATION_CODE 1
#define DIRECTION_CODE 2
#define LENGTH_CODE 3
#define DESTINATION_CODE 4

using namespace std;

class WireBrain: public AbstractBrain {

	static const bool& allowNegativeCharge;
	static const int& defaultWidth;
	static const int& defaultHeight;
	static const int& defaultDepth;
	static const int& worldConnectionsSeparation;
	static const int& overchargeThreshold;
	static const int& decayDuration;
	static const int& chargeUpdatesPerUpdate;
	static const bool& constantInputs;
	static const bool& cacheResults;
	static const int& cacheResultsCount;

	static const string& genomeDecodingMethod;  // "bitmap" = convert genome directly, "wiregenes" = genes defined by start codeons, location, direction and location
	static const int& wiregenesInitialGeneCount;
	static const double& bitmapInitialFillRatio;

	static const bool& wiregenesAllowSimpleWires;
	static const int& wiregenesSimpleWireMaxLength;
	static const string& wiregenesSimpleWireDirections;

	static const bool& wiregenesAllowWormholes;
	static const int& wiregenesWormholesBidirectional;

	static const bool& wiregenesAllowSquiggleWires;
	static const int& wiregenesSquiggleWireMinLength;
	static const int& wiregenesSquiggleWireMaxLength;
	static const string& wiregenesSquiggleWireDirections;

	const int CHARGE = 2 + decayDuration;
	const int NEGCHARGE = CHARGE * -1;

public:

	int width, depth, height;

	vector<int> nodesAddresses, nodesNextAddresses;  // where the nodes connect to the brain

	vector<int> allCells, nextAllCells;  // list of all cells in this brain
	vector<vector<int>> neighbors;  // for every cell list of wired neighbors (most will be empty)
	vector<int> wireAddresses;  // list of addresses for all cells which are wireAddresses (uncharged, charged and decay)

	vector<vector<long>> inputLookUpTable;  // table that contains output for a given input
	vector<int> inputCount;  // table that contains a count of the number of times we have seen a given input

	int connectionsCount;

	WireBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes);
	WireBrain(shared_ptr<AbstractGenome> genome, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes);
	WireBrain(const vector<bool> &genome, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes);

	virtual void initalize();
	virtual void connectPruneAndSetAveColumns(vector<pair<int, int>> wormholeList);
	virtual ~WireBrain() = default;
	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override;
	virtual void chargeUpdate();
	virtual void chargeUpdateTrit();
	virtual void update() override;
	virtual void SaveBrainState(string fileName);
	virtual void displayBrainState();
	virtual string description() override;
	virtual vector<string> getStats() override;
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome) override;
}
;

#endif /* defined(__BasicMarkovBrainTemplate__WireBrain__) */
