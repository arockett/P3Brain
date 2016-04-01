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

	WireBrain(int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes) :
			AbstractBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes) {
		width = defaultWidth;
		height = defaultHeight;
		depth = defaultDepth;

		connectionsCount = 0;
	}

	WireBrain(shared_ptr<AbstractGenome> genome, int _nrInNodes, int _nrOutNodes, int _nrHiddenNodes) :
			WireBrain(_nrInNodes, _nrOutNodes, _nrHiddenNodes) {
		//cout << "in WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes)" << endl;
		allCells.resize(width * depth * height);
		nextAllCells.resize(width * depth * height);
		neighbors.resize(width * depth * height);

		nodesAddresses.resize(nrOfBrainNodes);
		nodesNextAddresses.resize(nrOfBrainNodes);

		// used in wiregenes decoding
		vector<vector<int>> simpleWireFeatures;
		vector<vector<int>> squiggleWireFeatures;
		vector<vector<int>> wormholeFeatures;

		if (cacheResults) {
			inputLookUpTable.resize(pow(2, nrOfBrainNodes));  // set lookup tables to be large enough to handle all possible input combinations
			inputCount.clear();  // insure that the input counts are all 0.
			inputCount.resize(pow(2, nrOfBrainNodes));
			for (int i = 0; i < pow(2, nrOfBrainNodes); i++) {
				inputCount[i] = 0;
			}

			if (cacheResultsCount < 1) {
				cout << "\n\nERROR! in WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) cacheResultsCount must be > 0!\n\nExiting.\n" << endl;
				exit(1);
			}
		}

		// establish I/O
		if ((width * depth * height) < (nrOfBrainNodes * worldConnectionsSeparation)) {
			cout << "ERROR: WireBrain requires a bigger brain width * depth * height must be >= (nrOfNodes * worldConnectionsSeparation)!\nExiting\n" << endl;
			exit(1);
		}
		for (int i = 0; i < nrOfBrainNodes; i++) {
			nodesAddresses[i] = worldConnectionsSeparation * i;
			nodesNextAddresses[i] = ((width * depth * height) - 1) - (worldConnectionsSeparation * i);
		}

		if (genomeDecodingMethod == "bitmap") {
			// load genome into allCells
			auto genomeHandler = genome->newHandler(genome, true);
			for (int l = 0; l < width * depth * height; l++) {
				allCells[l] = genomeHandler->readInt(0, 1);  // 1 (WIRE) will be assigned initialFillRatio % of the time
				if (allCells[l] == WIRE) {
					wireAddresses.push_back(l);
				}
			}
		} else if (genomeDecodingMethod == "wiregenes") {

			// features are:
			// simpleWire: 42,255-42,X,Y,Z,Length, Direction (if cardinalOnly 0->5 if diagonalsAlso 0->25)
			// squiggleWire: 44,255-44,X,Y,Z,Length,dX,dY,dZ,dX,dY,dZ,dX,dY,dZ,dX,dY,dZ... (length sets of dX,dY,dZ)
			// wormhole: 43,255-43,Direction,X,Y,Z,DestinationX,DestinationY,DestionationZ (Direction, 0 = forward, 1 = backwards 2 = both)
			// first, decode genome into features list
			// then place all wire
			// and build all connections
			// then add wormhole connections if X,Y,Z and DestinationX,DestinationY,DestionationZ are both wire
			bool translation_Complete = false;
			if (genome->isEmpty()) {
				translation_Complete = true;
			} else {

				bool readForward = true;
				auto genomeHandler = genome->newHandler(genome, readForward);
				auto featureGenomeHandler = genome->newHandler(genome, readForward);

				int featureCount = 0;

				int testSite1Value, testSite2Value;
				testSite1Value = genomeHandler->readInt(0, 255);
				testSite2Value = genomeHandler->readInt(0, 255);
				while (!translation_Complete) {
					if (genomeHandler->atEOG()) {  // if genomeIndex > testIndex, testIndex has wrapped and we are done translating
						translation_Complete = true;
					} else if (testSite1Value + testSite2Value == 255) {  // if we found a possible start codon...
						if (testSite1Value == 42 && wiregenesAllowSimpleWires) {  // record a wire feature

							int possibleDirections;  // if cardinalOnly, only 6 possible directions, if diagonalsAlso then 26 possible directions
							if (wiregenesSimpleWireDirections == "cardinalOnly") {
								possibleDirections = 5;
							} else if (wiregenesSimpleWireDirections == "diagonalsAlso") {
								possibleDirections = 25;
							} else {
								cout << "\nERROR: in WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) recived illegal value for wiregenesSimpleWireDirections \"" << wiregenesSimpleWireDirections << "\".\n\nExiting!\n\n" << endl;
								exit(1);
							}

							genomeHandler->copyTo(featureGenomeHandler);  // make a copy of the genome handler so we can remeber where we are
							featureGenomeHandler->toggleReadDirection();
							featureGenomeHandler->readInt(0, 255);  // move back 2 start codon values
							featureGenomeHandler->readInt(0, 255);
							featureGenomeHandler->toggleReadDirection();  // reverse the read direction again
							featureGenomeHandler->readInt(0, 255, START_CODE, featureCount);  // mark start codon in genomes coding region
							featureGenomeHandler->readInt(0, 255, START_CODE, featureCount);

							simpleWireFeatures.push_back( { featureGenomeHandler->readInt(0, width - 1, LOCATION_CODE),  // X
							featureGenomeHandler->readInt(0, height - 1, LOCATION_CODE),  // Y
							featureGenomeHandler->readInt(0, depth - 1, LOCATION_CODE),  // Z
							featureGenomeHandler->readInt(1, wiregenesSimpleWireMaxLength, LENGTH_CODE),  // length of wire
							featureGenomeHandler->readInt(0, possibleDirections, DIRECTION_CODE),  // Direction the wire will be built in
									});

							featureCount++;

						} else if (testSite1Value == 43 && wiregenesAllowWormholes) {  // record a wormhole
							genomeHandler->copyTo(featureGenomeHandler);  // make a copy of the genome handler so we can remeber where we are
							featureGenomeHandler->toggleReadDirection();
							featureGenomeHandler->readInt(0, 255);  // move back 2 start codon values
							featureGenomeHandler->readInt(0, 255);
							featureGenomeHandler->toggleReadDirection();  // reverse the read direction again
							featureGenomeHandler->readInt(0, 255, START_CODE, featureCount);  // mark start codon in genomes coding region
							featureGenomeHandler->readInt(0, 255, START_CODE, featureCount);

							wormholeFeatures.push_back( { featureGenomeHandler->readInt(0, width - 1, LOCATION_CODE),  // X
							featureGenomeHandler->readInt(0, height - 1, LOCATION_CODE),  // Y
							featureGenomeHandler->readInt(0, depth - 1, LOCATION_CODE),  // Z
							featureGenomeHandler->readInt(0, width - 1, DESTINATION_CODE),  // DestinationX
							featureGenomeHandler->readInt(0, height - 1, DESTINATION_CODE),  // DestinationY
							featureGenomeHandler->readInt(0, depth - 1, DESTINATION_CODE),  // DestinationZ
							featureGenomeHandler->readInt(0, 1, DESTINATION_CODE),  // Direction
									});

							featureCount++;
						} else if (testSite1Value == 44 && wiregenesAllowSquiggleWires) {  // record a squggleWire

							int possibleDirections;  // if cardinalOnly, only 6 possible directions, if diagonalsAlso then 26 possible directions
							if (wiregenesSquiggleWireDirections == "cardinalOnly") {
								possibleDirections = 5;
							} else if (wiregenesSquiggleWireDirections == "diagonalsAlso") {
								possibleDirections = 25;
							} else {
								cout << "\nERROR: in WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) recived illegal value for wiregenesSquiggleWireDirections \"" << wiregenesSquiggleWireDirections << "\".\n\nExiting!\n\n" << endl;
								exit(1);
							}

							genomeHandler->copyTo(featureGenomeHandler);  // make a copy of the genome handler so we can remeber where we are
							featureGenomeHandler->toggleReadDirection();
							featureGenomeHandler->readInt(0, 255);  // move back 2 start codon values
							featureGenomeHandler->readInt(0, 255);
							featureGenomeHandler->toggleReadDirection();  // reverse the read direction again
							featureGenomeHandler->readInt(0, 255, START_CODE, featureCount);  // mark start codon in genomes coding region
							featureGenomeHandler->readInt(0, 255, START_CODE, featureCount);

							squiggleWireFeatures.push_back( { featureGenomeHandler->readInt(0, width - 1, LOCATION_CODE),  // X
							featureGenomeHandler->readInt(0, height - 1, LOCATION_CODE),  // Y
							featureGenomeHandler->readInt(0, depth - 1, LOCATION_CODE),  // Z
							featureGenomeHandler->readInt(wiregenesSquiggleWireMinLength, wiregenesSquiggleWireMaxLength, LENGTH_CODE) });

							int index = squiggleWireFeatures.size() - 1;
							int length = squiggleWireFeatures[squiggleWireFeatures.size() - 1][3];

							for (int i = 1; i < length; i++) {  // for length of wire, add directions
								squiggleWireFeatures[index].push_back(featureGenomeHandler->readInt(0, possibleDirections, DIRECTION_CODE));
							}
							featureCount++;
						}
					}
					genomeHandler->toggleReadDirection();
					genomeHandler->readInt(0, 255);  // move back 2 start codon values
					genomeHandler->readInt(0, 255);
					genomeHandler->toggleReadDirection();
					genomeHandler->advanceIndex();  // advance 1 index (might not be equal to a start codeon value (i.e. if we are reading from a bit genome)
					testSite1Value = genomeHandler->readInt(0, 255);
					testSite2Value = genomeHandler->readInt(0, 255);
				}
			}

			int X, Y, Z, length, wireIndex;
			// dList contains all possible directions, the 6 cardinal directions are listed first, so if cardinalOnly we will only pick from that subset
			vector<vector<int>> dList = { { 1, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 }, { 1, 1, 1 }, { 1, 1, 0 }, { 1, 1, -1 }, { 1, 0, 1 }, { 1, 0, -1 }, { 1, -1, 1 }, { 1, -1, 0 }, { 1, -1, -1 }, { 0, 1, 1 }, { 0, 1, -1 }, { 0, -1, 1 }, { 0, -1, -1 }, { -1, 1, 1 }, { -1, 1, 0 }, { -1, 1, -1 }, { -1, 0, 1 }, { -1, 0, -1 }, { -1, -1, 1 }, { -1, -1, 0 }, { -1, -1, -1 } };

			for (auto feature : simpleWireFeatures) {  // for every wire feature
				//cout << "simpleWireFeature: " << feature[0] << "," << feature[1] << "," << feature[2] << " L:" << feature[3] << " D:" << feature[4] << endl;
				X = feature[0];
				Y = feature[1];
				Z = feature[2];
				length = feature[3];
				for (int i = 0; i < length; i++) {  // for
					if (((X >= 0) && (X < width)) && ((Y >= 0) && (Y < height)) && ((Z >= 0) && (Z < depth))) {  //if we are in the brain, make this cell wire
						wireIndex = X + (Y * width) + (Z * (width * height));
						//cout << "WI: " << wireIndex << endl;
						if (allCells[wireIndex] != WIRE) {  // if this cell is not already wire...
							allCells[wireIndex] = WIRE;  // make this cell wire
							wireAddresses.push_back(wireIndex);  // add this cell to wireAddresses
						}
						//cout << X << "," << Y << "," << Z << endl;
						X += dList[feature[4]][0];  // move in direction to the next cell in this wire
						Y += dList[feature[4]][1];
						Z += dList[feature[4]][2];

					} else {
						//cout << "stopped" <<  endl;
						i = wiregenesSimpleWireMaxLength;  // if we are outside of the brain, stop this wire
					}
				}
			}
			for (auto feature : squiggleWireFeatures) {  // for every wire feature
//				cout << "squiggleWireFeature: " << feature[0] << "," << feature[1] << "," << feature[2] << " L:" << feature[3];
//				for (int i = 4; i < (int)feature.size(); i++){
//					cout << " " << feature[i];
//				}
//				cout << endl;

				int index = 0;
				X = feature[index++];
				Y = feature[index++];
				Z = feature[index++];
				length = feature[index++];

				for (int i = 1; i < length; i++) {  // for
					if (((X >= 0) && (X < width)) && ((Y >= 0) && (Y < height)) && ((Z >= 0) && (Z < depth))) {  //if we are in the brain, make this cell wire
						wireIndex = X + (Y * width) + (Z * (width * height));
						//cout << "WI: " << wireIndex << endl;
						if (allCells[wireIndex] != WIRE) {  // if this cell is not already wire...
							allCells[wireIndex] = WIRE;  // make this cell wire
							wireAddresses.push_back(wireIndex);  // add this cell to wireAddresses
						}
						//cout << X << "," << Y << "," << Z << endl;
						X += dList[feature[index]][0];  // move in direction indicated by the next three values in feature
						Y += dList[feature[index]][1];
						Z += dList[feature[index++]][2];

					} else {
						//cout << "stopped" <<  endl;
						i = wiregenesSquiggleWireMaxLength;  // if we are outside of the brain, stop this wire
					}
				}
			}
		} else {
			cout << "\nERROR: in WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) received illegal value for genomeDecodingMethod \"" << genomeDecodingMethod << "\".\n\nExiting!\n\n" << endl;
			exit(1);
		}

		//displayBrainState();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following block creates a "manageable" brain for testing ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		if ((width * depth * height) < (nrOfBrainNodes * worldConnectionsSeparation)) {
//			cout << "ERROR: WireBrain requires a bigger brain width * depth * height must be >= (nrOfNodes * worldConnectionsSeparation)!\nExiting\n" << endl;
//			exit(1);
//		}
//		for (int i = 0; i < nrOfBrainNodes; i++) {
//			nodesAddresses[i] = worldConnectionsSeparation * i;
//			nodesNextAddresses[i] = ((width * depth * height) - 1) - (worldConnectionsSeparation * i);
//		}
//				width = 7;
//				height = 7;
//				depth = 3;
//				allCells.resize(0);
//				nextAllCells.resize(0);
//				neighbors.resize(0);
//
//				allCells.resize(width * depth * height);
//				nextAllCells.resize(width * depth * height);
//				neighbors.resize(width * depth * height);
//				wireAddresses.resize(0);
//
//				connectionsCount = 0;
//				wireCount = 0;
//				emptyCount = 0;
//
//				allCells = {
//					1,1,0,1,1,0,0,
//					0,0,1,0,0,1,0,
//					0,1,0,1,0,1,0,
//					0,1,1,1,0,0,0,
//					0,0,1,0,0,1,0,
//					0,1,0,0,1,1,1,
//					1,1,1,1,1,0,1,
//
//					1,0,0,0,0,0,0,
//					0,0,0,0,0,0,0,
//					0,0,0,0,0,0,0,
//					0,0,0,0,0,1,0,
//					0,0,0,0,0,0,0,
//					0,0,0,0,0,0,0,
//					0,0,0,0,0,0,0,
//
//					1,0,0,1,1,0,0,
//					1,0,1,0,0,1,0,
//					0,1,0,1,0,1,0,
//					0,1,0,0,0,0,0,
//					0,0,1,0,0,1,0,
//					0,1,0,0,0,1,0,
//					0,0,1,1,1,0,0};
//
//				for (int l = 0; l < (int) allCells.size(); l++) {
//					if (allCells[l] == WIRE) {
//						wireAddresses.push_back(l);
//						wireCount++;
//					} else {
//						emptyCount++;
//					}
//
//				}
////////^///////////////^///////////////////^///////////////////^/////////////////^////////////////////////////^////////////////////////////////
// a "manageable" brain for testing ////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// The following block creates a "manageable" brain for testing ////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		width = 7;
//		height = 7;
//		depth = 4;
//		allCells.resize(0);
//		nextAllCells.resize(0);
//		neighbors.resize(0);
//
//		allCells.resize(width * depth * height);
//		nextAllCells.resize(width * depth * height);
//		neighbors.resize(width * depth * height);
//		wireAddresses.resize(0);
//
//		connectionsCount = 0;
//
//		if ((width * depth * height) < (nrOfBrainNodes * worldConnectionsSeparation)) {
//			cout << "ERROR: WireBrain requires a bigger brain width * depth * height must be >= (nrOfNodes * worldConnectionsSeparation)!\nExiting\n" << endl;
//			exit(1);
//		}
//		for (int i = 0; i < nrOfBrainNodes; i++) {
//			nodesAddresses[i] = worldConnectionsSeparation * i;
//			nodesNextAddresses[i] = ((width * depth * height) - 1) - (worldConnectionsSeparation * i);
//		}
//
//		allCells = {
//			1,1,1,0,0,0,1,
//			0,1,1,0,0,0,0,
//			0,0,1,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,1,0,
//			0,0,0,1,1,1,1,
//
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,1,0,
//			0,0,0,0,1,1,0,
//			0,0,0,0,1,1,0,
//			0,0,0,0,0,1,0,
//			0,0,0,1,1,1,1,
//
//			1,1,1,1,1,1,0,
//			1,0,0,0,0,0,0,
//			1,0,0,0,0,0,0,
//			1,0,0,0,0,0,0,
//			1,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//
//			0,0,0,0,0,0,1,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,1,1,
//			1,1,0,0,0,0,0,
//			1,1,0,1,1,1,1};
//
//		for (int l = 0; l < (int) allCells.size(); l++) {
//			if (allCells[l] == WIRE) {
//				wireAddresses.push_back(l);
//			}
//		}
		////////^///////////////^///////////////////^///////////////////^/////////////////^////////////////////////////^////////////////////////////////
		// a "manageable" brain for testing ////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// make neighbor connections
		for (auto l : wireAddresses) {  // for every cell
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);
			for (int x = -1; x < 2; x++) {  // for every neighbor in x,y,z, if that neighbor is wireAddresses, add it to the neighbor list
				for (int y = -1; y < 2; y++) {
					for (int z = -1; z < 2; z++) {
						int testX = cellX + x;
						int testY = cellY + y;
						int testZ = cellZ + z;
						//cout << testX << " " << testY << " " << testZ << "\n";
						if (((testX >= 0) && (testX < width)) && ((testY >= 0) && (testY < height)) && ((testZ >= 0) && (testZ < depth))) {
							//cout << "  valid\n";
							int neighborIndex = testX + (testY * width) + (testZ * (width * height));
							if ((allCells[neighborIndex] == WIRE) && (l != neighborIndex)) {
								//connectionsCount++;
								neighbors[l].push_back(neighborIndex);
							}
						}
					}
				}
			}
		}

		// add wiregenes wormholes
		vector<pair<int, int>> wormholeList;
		wormholeList.clear();
		if (genomeDecodingMethod == "wiregenes") {

			int sourceX, sourceY, sourceZ;
			int destinationX, destinationY, destinationZ;
			int sourceIndex, destinationIndex;
			for (auto feature : wormholeFeatures) {  // for every wire feature
//				cout << "wormholeFeature: " << feature[0] << "," << feature[1] << "," << feature[2] << "   " << feature[3] << "," << feature[4] << "," << feature[5] << " D:" << feature[6] << endl;

				sourceX = feature[0];
				sourceY = feature[1];
				sourceZ = feature[2];

				destinationX = feature[3];
				destinationY = feature[4];
				destinationZ = feature[5];

				sourceIndex = sourceX + (sourceY * width) + (sourceZ * (width * height));
				destinationIndex = destinationX + (destinationY * width) + (destinationZ * (width * height));
				if (allCells[sourceIndex] == WIRE && allCells[destinationIndex] == WIRE) {
					if (wiregenesWormholesBidirectional == 0) {
						//neighbors[destinationIndex].push_back(sourceIndex);
						wormholeList.push_back( { sourceIndex, destinationIndex });
						//connectionsCount++;
					}
					if (wiregenesWormholesBidirectional == 1) {
						//(feature[6]) ? neighbors[destinationIndex].push_back(sourceIndex) : neighbors[sourceIndex].push_back(destinationIndex);
						(feature[6]) ? wormholeList.push_back( { sourceIndex, destinationIndex }) : wormholeList.push_back( { destinationIndex, sourceIndex });
						//connectionsCount++;
					}
					if (wiregenesWormholesBidirectional == 2) {
						wormholeList.push_back( { sourceIndex, destinationIndex });
						wormholeList.push_back( { destinationIndex, sourceIndex });
						//connectionsCount++;
						//connectionsCount++;
					}
				}
			}
		}

		// prune unconnected wire

		vector<int> wirePruneList;
		vector<vector<int>> clusterList;
		clusterList.clear();
		int currentClusterIndex = 0;
		vector<int> currentToCheckList;

		int currentCell;
		wirePruneList = wireAddresses;  // all of the cells to check

		while (wirePruneList.size() > 0) {
			clusterList.resize(currentClusterIndex + 1);
			currentToCheckList.clear();
			clusterList[currentClusterIndex].push_back(wirePruneList[0]);  // add the first cell in the prune list to the currentList
			currentToCheckList.push_back(wirePruneList[0]);  // and also to the ToCheckList
			wirePruneList[0] = wirePruneList[wirePruneList.size() - 1];  // remove this cell from the list of cells to check
			wirePruneList.pop_back();

			while (currentToCheckList.size() > 0) {
				currentCell = currentToCheckList[0];
				currentToCheckList[0] = currentToCheckList[currentToCheckList.size() - 1];
				currentToCheckList.pop_back();

				for (int n : neighbors[currentCell]) {  // for each neighbor of this cell...
					if (find(wirePruneList.begin(), wirePruneList.end(), n) != wirePruneList.end()) {  // if that neighbor has not already been visited
						clusterList[currentClusterIndex].push_back(n);  // add it to the cluster
						currentToCheckList.push_back(n);  // also add it to a list of cells that we need to check
						int index = 0;
						for (size_t i = 0; i < wirePruneList.size(); i++) {  // get the index of this neighbor
							if (wirePruneList[i] == n) {
								index = i;
							}
						}
						wirePruneList[index] = wirePruneList[wirePruneList.size() - 1];  // remove this neighbor from the wirePruneList
						wirePruneList.pop_back();
					}
				}
			}
			currentClusterIndex++;
		}

		// now that we have clusters, determine for each cluster if it connects to an input, an output, or both

		vector<bool> clusterIsInputConnected;
		vector<bool> clusterIsOutputConnected;

		clusterIsInputConnected.resize(clusterList.size());
		clusterIsOutputConnected.resize(clusterList.size());

		for (size_t c = 0; c < clusterList.size(); c++) {
			clusterIsInputConnected[c] = false;
			clusterIsOutputConnected[c] = false;
			for (auto e : clusterList[c]) {
				for (auto v : nodesAddresses) {
					if (e == v) {
						clusterIsInputConnected[c] = true;
					}
				}
				for (auto v : nodesNextAddresses) {
					if (e == v) {
						clusterIsOutputConnected[c] = true;
					}
				}
			}
		}

		size_t i = 0;
		bool changedState = true;
		int targetCluster, sourceCluster;

		while (changedState == true) {
			i = 0;
			changedState = false;
			while (i < wormholeList.size()) {  // if there are still wormholes to check, or the state of clusters has changed
				//cout << i << "  current wormholeList : ";
				//for (auto w : wormholeList) {
				//	cout << w.first << "->" << w.second << "  ";
				//}
				//cout << endl;
				targetCluster = -1;
				sourceCluster = -1;
				for (size_t c = 0; c < clusterList.size(); c++) {
					for (size_t v = 0; v < clusterList[c].size(); v++) {
						//cout << "check for source: " << wormholeList[i].first << " =?= " << clusterList[c][v] << endl;
						//cout << "check for target: " << wormholeList[i].second << " =?= " << clusterList[c][v] << endl;
						if (wormholeList[i].first == clusterList[c][v]) {
							sourceCluster = c;
							//cout << "  source cluster set to: " << sourceCluster << endl;
						}
						if (wormholeList[i].second == clusterList[c][v]) {
							targetCluster = c;
							//cout << "  target cluster set to: " << targetCluster << endl;
						}
					}
				}
				//cout << "   source cluster = " << sourceCluster << "  ->  target cluster = " << targetCluster << endl;
				if (sourceCluster == targetCluster) {  // wormhole source and target are in the same cluster.
					neighbors[wormholeList[i].second].push_back(wormholeList[i].first);  // now wormholeList[i].first will transmit charge to wormholeList[i].second
					//cout << "** same cluster: " << wormholeList[i].first << "  " << wormholeList[i].second << endl;
					wormholeList[i] = wormholeList[wormholeList.size() - 1];  // remove this wormhole from the list
					wormholeList.pop_back();
				} else if (sourceCluster == -1 || targetCluster == -1) {  // at least one end of the wormhole is not wire
				//cout << "** unconnected: " << wormholeList[i].first << "  " << wormholeList[i].second << endl;
					wormholeList[i] = wormholeList[wormholeList.size() - 1];  // remove this wormhole from the list
					wormholeList.pop_back();
				} else {  // wormhole connects two clusters
					//cout << "** diffrent cluster: " << wormholeList[i].first << "  " << wormholeList[i].second << endl;
					if (clusterIsInputConnected[sourceCluster] && !clusterIsInputConnected[targetCluster]) {  // the source cluster is connected to an input, but the target cluster is not
						clusterIsInputConnected[targetCluster] = true;  // note that the target cluster now is connected to an input
						changedState = true;  // we have changed state and will need to check again
						//cout << "  ** cluster " << targetCluster << " now input connected" << endl;
					}
					if (clusterIsOutputConnected[targetCluster] && !clusterIsOutputConnected[sourceCluster]) {  // the target cluster is connected to an output, but the source cluster is not
						clusterIsOutputConnected[sourceCluster] = true;  // note that the source cluster now is connected to an output
						changedState = true;  // we have changed state and will need to check again
						//cout << "  ** cluster " << sourceCluster << " now output connected" << endl;
					}
					if (clusterIsInputConnected[sourceCluster] && clusterIsOutputConnected[targetCluster]) {  // if both clusters are connected to input and output...
						neighbors[wormholeList[i].second].push_back(wormholeList[i].first);  // now wormholeList[i].first will transmit charge to wormholeList[i].second
						//cout << "  ** added wormhole connection " << wormholeList[i].first << "->" << wormholeList[i].second << endl;
						wormholeList[i] = wormholeList[wormholeList.size() - 1];  // remove this wormhole from the list
						wormholeList.pop_back();
					} else {
						i++;
					}
				}
			}
		}

		//cout << "after correcting for wormholes, wormholeList : ";
		//for (auto w : wormholeList) {
		//	cout << w.first << "->" << w.second << "  ";
		//}

		//for (size_t c = 0; c < clusterList.size(); c++) {
		//cout << "cluster[" << c << "]\t input connected: " << clusterIsInputConnected[c] << "\t output connected: " << clusterIsOutputConnected[c] << endl;
		//for (auto e : clusterList[c]) {
		//cout << e << " ";
		//}
		//cout << endl;
		//}

		//displayBrainState();

		// now update wireAddresses to only include connected wire
		int wireCount = 0;
		wireAddresses.clear();
		for (size_t c = 0; c < clusterList.size(); c++) {
			if (clusterIsInputConnected[c] && clusterIsOutputConnected[c]) {
				wireAddresses.insert(wireAddresses.end(), clusterList[c].begin(), clusterList[c].end());
				wireCount += (int) clusterList[c].size();
			}
		}

		// update the connectionsCount
		connectionsCount = 0;
		for (auto w : wireAddresses) {
			connectionsCount += (int) neighbors[w].size();
		}
		//cout << connectionsCount;

		// now update allCells
		vector<int> newAllCells;
		newAllCells.resize(width * depth * height);
		for (auto w : wireAddresses) {
			newAllCells[w] = 1;
		}
		swap(newAllCells, allCells);

		//displayBrainState();

		//cout << "  made wire brain with : " << connectionsCount << " connections and " << wireCount << " wires." << endl;

		// columns to be added to ave file
		aveFileColumns.clear();
		aveFileColumns.push_back("wireBrainWireCount");
		aveFileColumns.push_back("wireBrainConnectionsCount");

	}

	virtual ~WireBrain() = default;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override {
		shared_ptr<WireBrain> newBrain = make_shared<WireBrain>(_genome, nrInNodes, nrOutNodes, nrHiddenNodes);
		return newBrain;
	}

	virtual void chargeUpdate() {
		//cout << "in chargeUpdate()" << endl;
		//int wireCount = 0;
		//int evalCount = 0;
		//int evalToCharge = 0;
		//int evalToDecay = 0;
		//int evalToWire = 0;

		nextAllCells = allCells;

		// propagate charge in the brain
		int chargeCount;
		for (auto cellAddress : wireAddresses) {
			//wireCount++;
			if (allCells[cellAddress] == WIRE) {
				chargeCount = 0;
				int nc = (int) neighbors[cellAddress].size() - 1;  // get the number of neighbors which are wire
				while (nc >= 0 && chargeCount < overchargeThreshold) {  // for each neighbor
					if (allCells[neighbors[cellAddress][nc]] == CHARGE) {  // if that neighbor is charged
						chargeCount++;
						nextAllCells[cellAddress] = CHARGE;  // set this WIRE to CHARGE
					}
					nc--;
				}
				if (chargeCount >= overchargeThreshold) {
					nextAllCells[cellAddress] = WIRE;  // if overcharged, change back to WIRE
					//evalToCharge++;
				}
				//cout << "  " << cellAddress << " " << nextAllCells[cellAddress] << endl;
			} else {  // this wire is currently either charged or in decay
				nextAllCells[cellAddress] = allCells[cellAddress] - 1;
			}
		}
		allCells = nextAllCells;

		// if constantInputs, rechage the inputs
		if (constantInputs) {
			for (int i = 0; i < nrOfBrainNodes; i++) {  // for each input cell
				//evalCount++;
				if (nodes[i] != 0) {  // if this node is on
					if (allCells[nodesAddresses[i]] != HOLLOW) {  // if the connected location is uncharged wireAddresses...
						allCells[nodesAddresses[i]] = CHARGE * Bit(nodes[i]);  // charge it.
						//evalToCharge++;
					}
				}
			}
		}
		// read and accumulate outputs
		// NOTE: output cells can go into charge/decay sets
		for (int i = 0; i < nrOfBrainNodes; i++) {
			//cout << i << " " << nodesNextAddresses[i] << " " << nodesNext[i] <<endl;
			nextNodes[i] = nextNodes[i] + (allCells[nodesNextAddresses[i]] == CHARGE);
			//cout << i << " " << nodesNextAddresses[i] << " " << nodesNext[i] <<endl;
		}
	}

	virtual void chargeUpdateTrit() {
		//cout << "in chargeUpdate()" << endl;
		//int wireCount = 0;
		//int evalCount = 0;
		//int evalToCharge = 0;
		//int evalToDecay = 0;
		//int evalToWire = 0;

		nextAllCells = allCells;

		// propagate charge in the brain
		int chargeCount;
		for (auto cellAddress : wireAddresses) {
			//wireCount++;
			nextAllCells[cellAddress] = 1;
			if (allCells[cellAddress] == WIRE) {
				chargeCount = 0;
//				int nc = (int) neighbors[cellAddress].size() - 1;  // get the number of neighbors which are wire
//				while (nc >= 0) {  // for each neighbor
//					if (allCells[neighbors[cellAddress][nc]] == CHARGE) {  // if that neighbor is charged
//						chargeCount++;
//						//nextAllCells[cellAddress] = CHARGE;  // set this WIRE to CHARGE
//					}
//					if (allCells[neighbors[cellAddress][nc]] == NEGCHARGE) {  // if that neighbor is charged
//						chargeCount--;
//						//nextAllCells[cellAddress] = CHARGE;  // set this WIRE to CHARGE
//					}
//					nc--;
//				}

				for (auto n : neighbors[cellAddress]) {
					if (allCells[n] == CHARGE) {  // if that neighbor is charged
						chargeCount++;
						//nextAllCells[cellAddress] = CHARGE;  // set this WIRE to CHARGE
					}
					if (allCells[n] == NEGCHARGE) {  // if that neighbor is charged
						chargeCount--;
						//nextAllCells[cellAddress] = CHARGE;  // set this WIRE to CHARGE
					}
				}
				if (chargeCount > 0 && chargeCount < overchargeThreshold) {
					nextAllCells[cellAddress] = CHARGE;  // if overcharged, change back to WIRE
					//evalToCharge++;
				} else if (chargeCount < 0 && chargeCount > (overchargeThreshold * -1)) {
					nextAllCells[cellAddress] = NEGCHARGE;
				}
				//cout << "  " << cellAddress << " " << nextAllCells[cellAddress] << endl;
			} else if (allCells[cellAddress] == NEGCHARGE) {
				nextAllCells[cellAddress] = CHARGE - 1;
			} else {  // this wire is currently either charged or in decay
				nextAllCells[cellAddress] = allCells[cellAddress] - 1;
			}
		}
		allCells = nextAllCells;

		// if constantInputs, rechage the inputs
		if (constantInputs) {
			for (int i = 0; i < nrOfBrainNodes; i++) {  // for each input cell
				//evalCount++;
				if (nodes[i] != 0) {  // if this node is on
					if (allCells[nodesAddresses[i]] != HOLLOW) {  // if the connected location is uncharged wireAddresses...
						allCells[nodesAddresses[i]] = CHARGE * Trit(nodes[i]);  // charge it.
						//evalToCharge++;
					}
				}
			}
		}
		// read and accumulate outputs
		// NOTE: output cells can go into charge/decay sets
		for (int i = 0; i < nrOfBrainNodes; i++) {
			//cout << i << " " << nodesNextAddresses[i] << " " << nodesNext[i] <<endl;
			nextNodes[i] = nextNodes[i] + allCells[nodesNextAddresses[i]];
			//cout << i << " " << nodesNextAddresses[i] << " " << nodesNext[i] <<endl;
		}
	}

	virtual void update() override {
		//cout << "in update()"<<endl;

		/// first see if we we already know this input

//		for (auto v:inputCount){
//			if (v != 0){
//				cout << "found: " << v << endl;
//				exit(1);
//			}
//		}

		if (cacheResults) {
			long inputLookUpValue = 0;
			//cout << "\nInput nodes: ";

			for (int i = 0; i < nrOfBrainNodes; i++) {  // load inputs into inputLookUpValue
				inputLookUpValue = Bit(nodes[i]) + (inputLookUpValue << 1);
				//cout << nodes[i];
			}
			//cout << " = " << inputLookUpValue << endl << "  count: " << inputCount[inputLookUpValue] << endl;
			if (inputCount[inputLookUpValue] >= cacheResultsCount) {  // if we have seen this value at least cacheResultsCount
				long outputValue = inputLookUpTable[inputLookUpValue][Random::getIndex(cacheResultsCount)];  // pull a stored value randomly
				//cout << "                                                                     outputValue: " << outputValue << " = ";
				for (int i = nrOfBrainNodes - 1; i > -1; i--) {  // load outputValue into nodesNext
					nextNodes[i] = outputValue & 1;  // get right most bit
					outputValue = outputValue >> 1;  // clip off right most bit
					//cout << nodesNext[i];
				}
				//cout << endl;
			} else {  // we have not seen this input value enough times, and we will need to actually do the work
				for (auto w : wireAddresses) {  // clear out any wire that is charged or decay from last update
					allCells[w] = 1;
				}
				for (int i = 0; i < nrOfBrainNodes; i++) {  // set up inputs and outputs
					nextNodes[i] = 0;  // reset all nodesNext
					if (!allowNegativeCharge) {
						if (Bit(nodes[i]) == 1 && allCells[nodesAddresses[i]] == WIRE) {  // for each node if it is on and connects to wire
							allCells[nodesAddresses[i]] = CHARGE;  // charge the wire
						}
					} else {
						if (Trit(nodes[i]) != 0 && allCells[nodesAddresses[i]] == WIRE) {  // for each node if it is on and connects to wire
							allCells[nodesAddresses[i]] = CHARGE * Trit(nodes[i]);  // charge the wire
						}
					}
					//// for testing only!!!////
					//allCells[0]=CHARGE;
					/////////////////////////////
				}
				if (recordActivity) {
					SaveBrainState("wireBrain.run");
				}
				for (int count = 0; count < chargeUpdatesPerUpdate; count++) {
					if (!allowNegativeCharge) {
						chargeUpdate();
					} else {
						chargeUpdateTrit();
					}
					if (recordActivity) {
						SaveBrainState(recordActivityFileName);
					}
				}
				//////
				// set lookup table value!
				//////
				long outputValue = 0;
				for (int i = 0; i < nrOfBrainNodes; i++) {  // load outputs into outputValue
					outputValue = nextNodes[i] + (outputValue << 1);
				}
				inputLookUpTable[inputLookUpValue].push_back(outputValue);  // push outputValue into the lookup table

			}
			inputCount[inputLookUpValue]++;

		} else {  // no caching
			for (auto w : wireAddresses) {  // clear out anything that is charged or decay from last update
				allCells[w] = 1;
			}
			for (int i = 0; i < nrOfBrainNodes; i++) {  // set up inputs and outputs
				nextNodes[i] = 0;  // reset nodesNext
				if (nodes[i] == 1 && allCells[nodesAddresses[i]] == WIRE) {  // if this node is on and connects to wire
					allCells[nodesAddresses[i]] = CHARGE;
				}
			}

			for (int count = 0; count < chargeUpdatesPerUpdate; count++) {
				chargeUpdate();
			}
		}

		nodes = nextNodes;  // update nodes

	}

	virtual void SaveBrainState(string fileName) {
//		for (int i = 0; i < nrOfNodes; i++) {
//			int l = nodesAddresses[i];
//			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
//			int cellY = (l % (width * height)) / width;
//			int cellZ = l / (width * height);
//
//			cout << "node" << i << "@(" << cellX << "," << cellY << "," << cellZ << ")=" << nodes[i] << "  ";
//		}
//
//		cout << endl;
//		cout << "---------------------------------------------------\n";

		string stateNow = "";

		for (auto cell : allCells) {
			if (cell == 0) {
				stateNow += "E";
			} else if (cell == 1) {
				stateNow += "W";
			} else if (cell == CHARGE) {
				stateNow += "C";
			} else if (cell == NEGCHARGE) {
				stateNow += "N";
			} else {
				stateNow += "D";
			}
		}
		FileManager::writeToFile(fileName, stateNow, to_string(width) + ',' + to_string(height) + ',' + to_string(depth));  //fileName, data, header - used when you want to output formatted data (i.e. genomes)
//		for (int i = 0; i < nrOfNodes; i++) {
//			int l = nodesNextAddresses[i];
//			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
//			int cellY = (l % (width * height)) / width;
//			int cellZ = l / (width * height);
//
//			cout << "node" << i << "@(" << cellX << "," << cellY << "," << cellZ << ")=" << nodesNext[i] << "  ";
//		}
//		cout << endl;
//		char c;
//		cin >> c;
	}

	virtual void displayBrainState() {
		for (int i = 0; i < nrOfBrainNodes; i++) {
			int l = nodesAddresses[i];
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);

			cout << "node" << i << "@(" << cellX << "," << cellY << "," << cellZ << ")=" << nodes[i] << "  ";
		}

		cout << endl;
		cout << "---------------------------------------------------\n";

		//int i = 0;
		int w = 0;
		int d = 0;

		for (auto cell : allCells) {
			if (cell == 0) {
				cout << " ";
			} else if (cell == 1) {
				cout << ".";
			} else if (cell == CHARGE) {
				cout << "+";
			} else if (cell == NEGCHARGE) {
				cout << "-";
			} else {
				cout << "o";
			}
			//i++;
			w++;
			d++;
			if (w == width) {
				cout << "\n";
				w = 0;
			}
			if (d == width * height) {
				cout << "---------------------------------------------------\n";

				d = 0;
			}

		}
		for (int i = 0; i < nrOfBrainNodes; i++) {
			int l = nodesNextAddresses[i];
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);

			cout << "node" << i << "@(" << cellX << "," << cellY << "," << cellZ << ")=" << nextNodes[i] << "  ";
		}
		cout << endl;
		char c;
		cin >> c;
	}

	virtual string description() override {
		return "WireBrain\n";
	}

	virtual vector<string> getStats() override {
		vector<string> dataPairs;

		dataPairs.push_back("brainWidth");
		dataPairs.push_back(to_string(width));
		dataPairs.push_back("brainHeight");
		dataPairs.push_back(to_string(height));
		dataPairs.push_back("brainDepth");
		dataPairs.push_back(to_string(depth));

		dataPairs.push_back("wireBrainWireCount");
		dataPairs.push_back(to_string(wireAddresses.size()));

		dataPairs.push_back("wireBrainConnectionsCount");
		dataPairs.push_back(to_string(connectionsCount));

		return dataPairs;
	}

public:
	virtual void initalizeGenome(shared_ptr<AbstractGenome> _genome) {
		int codonMax = (1 << Global::bitsPerCodon) - 1;

		if (genomeDecodingMethod == "bitmap") {
			auto genomeHandler = _genome->newHandler(_genome);

			for (int i = 0; i < width * depth * height; i++) {  // fill the genome with 0s and 1s randomly with a biased ratio
				genomeHandler->writeInt(Random::P(bitmapInitialFillRatio), 0, 1);
			}
		}
		if (genomeDecodingMethod == "wiregenes") {
			_genome->fillRandom();

			auto genomeHandler = _genome->newHandler(_genome);

			for (int i = 0; i < wiregenesInitialGeneCount; i++) {
				genomeHandler->randomize();
				int pick = Random::getInt(42, 44);
				genomeHandler->writeInt(pick, 0, codonMax);
				genomeHandler->writeInt(codonMax - pick, 0, codonMax);
			}
		}
	}

}
;

#endif /* defined(__BasicMarkovBrainTemplate__WireBrain__) */
