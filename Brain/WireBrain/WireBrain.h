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

#include "../../Genome/AbstractGenome.h"

#include "../../Utilities/Random.h"

#include "../AbstractBrain.h"

// define the wire states
using namespace std;

class WireBrain: public AbstractBrain {

	static shared_ptr<ParameterLink<bool>> allowNegativeChargePL;
	static shared_ptr<ParameterLink<int>> defaultWidthPL;
	static shared_ptr<ParameterLink<int>> defaultHeightPL;
	static shared_ptr<ParameterLink<int>> defaultDepthPL;
	static shared_ptr<ParameterLink<int>> worldConnectionsSeparationPL;
	static shared_ptr<ParameterLink<int>> overchargeThresholdPL;
	static shared_ptr<ParameterLink<int>> decayDurationPL;
	static shared_ptr<ParameterLink<int>> chargeUpdatesPerUpdatePL;
	static shared_ptr<ParameterLink<bool>> constantInputsPL;
	static shared_ptr<ParameterLink<bool>> cacheResultsPL;
	static shared_ptr<ParameterLink<int>> cacheResultsCountPL;

	static shared_ptr<ParameterLink<string>> genomeDecodingMethodPL;  // "bitmap" = convert genome directly, "wiregenes" = genes defined by start codeons, location, direction and location
	static shared_ptr<ParameterLink<int>> wiregenesInitialGeneCountPL;
	static shared_ptr<ParameterLink<double>> bitmapInitialFillRatioPL;

	static shared_ptr<ParameterLink<bool>> wiregenesAllowSimpleWiresPL;
	static shared_ptr<ParameterLink<int>> wiregenesSimpleWireMaxLengthPL;
	static shared_ptr<ParameterLink<string>> wiregenesSimpleWireDirectionsPL;

	static shared_ptr<ParameterLink<bool>> wiregenesAllowWormholesPL;
	static shared_ptr<ParameterLink<int>> wiregenesWormholesBidirectionalPL;

	static shared_ptr<ParameterLink<bool>> wiregenesAllowSquiggleWiresPL;
	static shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMinLengthPL;
	static shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMaxLengthPL;
	static shared_ptr<ParameterLink<string>> wiregenesSquiggleWireDirectionsPL;

	shared_ptr<ParameterLink<bool>> allowNegativeChargeLPL;
	shared_ptr<ParameterLink<int>> defaultWidthLPL;
	shared_ptr<ParameterLink<int>> defaultHeightLPL;
	shared_ptr<ParameterLink<int>> defaultDepthLPL;
	shared_ptr<ParameterLink<int>> worldConnectionsSeparationLPL;
	shared_ptr<ParameterLink<int>> overchargeThresholdLPL;
	shared_ptr<ParameterLink<int>> decayDurationLPL;
	shared_ptr<ParameterLink<int>> chargeUpdatesPerUpdateLPL;
	shared_ptr<ParameterLink<bool>> constantInputsLPL;
	shared_ptr<ParameterLink<bool>> cacheResultsLPL;
	shared_ptr<ParameterLink<int>> cacheResultsCountLPL;

	shared_ptr<ParameterLink<string>> genomeDecodingMethodLPL;  // "bitmap" = convert genome directly, "wiregenes" = genes defined by start codeons, location, direction and location
	shared_ptr<ParameterLink<int>> wiregenesInitialGeneCountLPL;
	shared_ptr<ParameterLink<double>> bitmapInitialFillRatioLPL;

	shared_ptr<ParameterLink<bool>> wiregenesAllowSimpleWiresLPL;
	shared_ptr<ParameterLink<int>> wiregenesSimpleWireMaxLengthLPL;
	shared_ptr<ParameterLink<string>> wiregenesSimpleWireDirectionsLPL;

	shared_ptr<ParameterLink<bool>> wiregenesAllowWormholesLPL;
	shared_ptr<ParameterLink<int>> wiregenesWormholesBidirectionalLPL;

	shared_ptr<ParameterLink<bool>> wiregenesAllowSquiggleWiresLPL;
	shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMinLengthLPL;
	shared_ptr<ParameterLink<int>> wiregenesSquiggleWireMaxLengthLPL;
	shared_ptr<ParameterLink<string>> wiregenesSquiggleWireDirectionsLPL;

	int CHARGE;  // = 2 + *decayDuration;
	int NEGCHARGE;  // = CHARGE * -1;

	const int WIRE = 1;
	const int HOLLOW = 0;

	const int START_CODE = 0;
	const int LOCATION_CODE = 1;
	const int DIRECTION_CODE = 2;
	const int LENGTH_CODE = 3;
	const int DESTINATION_CODE = 4;

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
};

#endif /* defined(__BasicMarkovBrainTemplate__WireBrain__) */
