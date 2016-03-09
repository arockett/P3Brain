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

#include "Brain.h"
#include "../Genome/Genome.h"

#include "../Utilities/Parameters.h"
#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"

// define the wire states
#define CHARGE 3
#define DECAY 2
#define WIRE 1
#define HOLLOW 0

using namespace std;

class WireBrain : public AbstractBrain {

	static int& defaultWidth;
	static int& defaultHeight;
	static int& defaultDepth;
	static int& defaultNrOfBrainNodes;
	static int& worldConnectionsSeparation;
	static double& initialFillRatio;
	static int& overchargeThreshold;
	static int& chargeUpdatesPerUpdate;
	static bool& constantInputs;
//	class cell {
// 	public:
//		int value;
//		int chargeCount;
//		vector<shared_ptr<cell>> neighbors;
//
//		cell(int _value) {
//			value = _value;
//			chargeCount = 0;
//		}
//	};

 public:

	int width, depth, height, nrOfNodes;
	//vector<shared_ptr<cell>> allCells;
	//unordered_set<shared_ptr<cell>> charged, chargedNext, decay, decayNext, wire;
	//vector<shared_ptr<cell>> inputCells, outputCells;

	vector<bool> nodes, nodesNext;
	vector<int> nodesAddresses, nodesNextAddresses;  // where the nodes connect to the brain

	vector<int> allCells, nextAllCells;  // list of all cells in this brain
	vector<vector<int>> neighbors;  // for every cell list of wired neighbors (most will be empty)
	vector<int> wireAddresses;  // list of addresses for all cells which are wireAddresses (uncharged, charged and decay)

	WireBrain() {
		width = defaultWidth;
		height = defaultHeight;
		depth = defaultDepth;
		nrOfNodes = defaultNrOfBrainNodes;

	}

//	WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) {
//		nrOfNodes = _nrOfNodes;
//		width = defaultWidth;
//		height = defaultHeight;
//		depth = defaultDepth;
//
//		nodes.resize(nrOfNodes);
//		nodesNext.resize(nrOfNodes);
//		nodesAddresses.resize(nrOfNodes);
//		nodesNextAddresses.resize(nrOfNodes);
//
//		allCells.resize(width * depth * height);
//		nextAllCells.resize(width * depth * height);
//		neighbors.resize(width * depth * height);
//
//		int connectionsCount = 0;
//		int wireCount = 0;
//		int emptyCount = 0;
//
//		// load genome into allCells
//		auto genomeHandler = genome->newHandler(genome, true);
//		for (int l = 0; l < width * depth * height; l++) {
//			allCells[l] = genomeHandler->readDouble(0, 1) < initialFillRatio;  // 1 (WIRE) will be assigned initialFillRatio % of the time
//			if (allCells[l] == WIRE) {
//				wireAddresses.push_back(l);
//				wireCount++;
//			} else {
//				emptyCount++;
//			}
//		}
//
//		// make neighbor connections
//		for (auto l : wireAddresses) {  // for every cell
//			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
//			int cellY = (l % (width * height)) / width;
//			int cellZ = l / (width * height);
//			for (int x = -1; x < 2; x++) {	// for every neighbor in x,y,z, if that neighbor is wireAddresses, add it to the neighbor list
//				for (int y = -1; y < 2; y++) {
//					for (int z = -1; z < 2; z++) {
//						int testX = cellX + x;
//						int testY = cellY + y;
//						int testZ = cellZ + z;
//						//cout << testX << " " << testY << " " << testZ << "\n";
//						if (((testX >= 0) && (testX < width)) && ((testY >= 0) && (testY < height)) && ((testZ >= 0) && (testZ < depth))) {
//							//cout << "  valid\n";
//							int neighborIndex = testX + (testY * width) + (testZ * (width * height));
//							if ((allCells[neighborIndex] == WIRE) && (l != neighborIndex)) {
//								connectionsCount++;
//								neighbors[l].push_back(neighborIndex);
//							}
//						}
//					}
//				}
//			}
//		}
//
//		if (width * depth * height < (nrOfNodes * worldConnectionsSeparation)) {
//			cout << "ERROR: WireBrain requires a bigger brain width * depth * height must be >= (nrOfNodes * worldConnectionsSeparation)!\nExiting\n";
//			exit(1);
//		}
//		for (int i = 0; i < nrOfNodes; i++) {
//			nodesAddresses[i] = worldConnectionsSeparation * i;
//			nodesNextAddresses[i] = ((width * depth * height) - 1) - (worldConnectionsSeparation * i);
//			//cout << worldConnectionsSeparation * i << " " << nodesAddresses[i] << "    " << nodesNextAddresses[i] << "\n";
//		}
//		//cout << "  made wire brain with : " << connectionsCount << " connections (w:" << wireCount << "/e:" << emptyCount << ")\n";
//
//
//	}

	WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) {
		nrOfNodes = _nrOfNodes;
		width = defaultWidth;
		height = defaultHeight;
		depth = defaultDepth;

		nodes.resize(nrOfNodes);
		nodesNext.resize(nrOfNodes);
		nodesAddresses.resize(nrOfNodes);
		nodesNextAddresses.resize(nrOfNodes);

		allCells.resize(width * depth * height);
		nextAllCells.resize(width * depth * height);
		neighbors.resize(width * depth * height);

		int connectionsCount = 0;
		int wireCount = 0;
		int emptyCount = 0;

		// load genome into allCells
		auto genomeHandler = genome->newHandler(genome, true);
		for (int l = 0; l < width * depth * height; l++) {
			allCells[l] = genomeHandler->readDouble(0, 1) < initialFillRatio;  // 1 (WIRE) will be assigned initialFillRatio % of the time
			if (allCells[l] == WIRE) {
				wireAddresses.push_back(l);
				wireCount++;
			} else {
				emptyCount++;
			}
		}

//		width = 7;
//		height = 7;
//		depth = 3;
//
//		allCells.resize(width * depth * height);
//		nextAllCells.resize(width * depth * height);
//		neighbors.resize(width * depth * height);
//
//		connectionsCount = 0;
//		wireCount = 0;
//		emptyCount = 0;
//
//		allCells = {
//			1,0,0,1,1,0,0,
//			0,0,1,0,0,1,0,
//			0,1,0,1,0,1,0,
//			0,1,1,1,0,0,0,
//			0,0,1,0,0,1,0,
//			0,1,0,0,1,1,1,
//			1,1,1,1,1,0,1,
//
//			1,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,1,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//			0,0,0,0,0,0,0,
//
//			1,0,0,1,1,0,0,
//			1,0,1,0,0,1,0,
//			0,1,0,1,0,1,0,
//			0,1,0,0,0,0,0,
//			0,0,1,0,0,1,0,
//			0,1,0,0,0,1,0,
//			0,0,1,1,1,0,0};
//
//		for (int l = 0; l < (int) allCells.size(); l++) {
//			if (allCells[l] == WIRE) {
//				wireAddresses.push_back(l);
//				wireCount++;
//			} else {
//				emptyCount++;
//			}
//
//		}

		// make neighbor connections
		for (auto l : wireAddresses) {  // for every cell
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);
			for (int x = -1; x < 2; x++) {	// for every neighbor in x,y,z, if that neighbor is wireAddresses, add it to the neighbor list
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
								connectionsCount++;
								neighbors[l].push_back(neighborIndex);
							}
						}
					}
				}
			}
		}

		if (width * depth * height < (nrOfNodes * worldConnectionsSeparation)) {
			cout << "ERROR: WireBrain requires a bigger brain width * depth * height must be >= (nrOfNodes * worldConnectionsSeparation)!\nExiting\n";
			exit(1);
		}
		for (int i = 0; i < nrOfNodes; i++) {
			nodesAddresses[i] = worldConnectionsSeparation * i;
			nodesNextAddresses[i] = ((width * depth * height) - 1) - (worldConnectionsSeparation * i);
			//cout << worldConnectionsSeparation * i << " " << nodesAddresses[i] << "    " << nodesNextAddresses[i] << "\n";
		}
		//cout << "  made wire brain with : " << connectionsCount << " connections (w:" << wireCount << "/e:" << emptyCount << ")\n";

	}

	virtual ~WireBrain() = default;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) override {
		shared_ptr<WireBrain> newBrain = make_shared<WireBrain>(_genome, defaultNrOfBrainNodes);
		return newBrain;
	}

	virtual void chargeUpdate() {
		//cout << "in chargeUpdate()\n";
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
				int nc = (int) neighbors[cellAddress].size() - 1;  // get the number of neighbors which are wire - 1
				while (nc >= 0 && chargeCount < overchargeThreshold) {  // for each neighbor
					if (allCells[neighbors[cellAddress][nc]] == CHARGE) {  // if that neighbor is charged
						chargeCount++;
						nextAllCells[cellAddress] = CHARGE;  // set this WIRE to CHARGE
						//cout << "nc: " << nc << " chargeCount: " << chargeCount << endl;
						//evalCount++;
					}
					nc--;
				}
				if (chargeCount == overchargeThreshold) {
					nextAllCells[cellAddress] = WIRE;  // if overcharged, change back to WIRE
					//evalToCharge++;
				}
				//cout << "  " << cellAddress << " " << nextAllCells[cellAddress] << endl;
			} else if (allCells[cellAddress] == DECAY) {
				nextAllCells[cellAddress] = WIRE;
				//evalCount++;
				//evalToWire++;
			} else if (allCells[cellAddress] == CHARGE) {
				nextAllCells[cellAddress] = DECAY;
				//evalCount++;
				//evalToDecay++;
			}
		}
		allCells = nextAllCells;

		// if constantInputs, rechage the inputs
		if (constantInputs) {
			for (int i = 0; i < nrOfNodes; i++) {  // for each input cell
				//evalCount++;
				if (nodes[i] == 1) {  // if this node is on
					if (allCells[nodesAddresses[i]] == WIRE || allCells[nodesAddresses[i]] == DECAY) {  // if the connected location is uncharged wireAddresses...
						allCells[nodesAddresses[i]] = CHARGE;  // charge it.
						//evalToCharge++;
					}
				}
			}
		}
		// read and accumulate outputs
		// NOTE: output cells can go into charge/decay sets
		for (int i = 0; i < nrOfNodes; i++) {
			//cout << i << " " << nodesNextAddresses[i] << " " << nodesNext[i] <<endl;
			nodesNext[i] = nodesNext[i] | (allCells[nodesNextAddresses[i]] == CHARGE);
			//cout << i << " " << nodesNextAddresses[i] << " " << nodesNext[i] <<endl;
		}

	}

	virtual void update() override {
		//cout << "in update()\n";
		for (auto w : wireAddresses) {  // clear out anything that is charged or decay from last update
			allCells[w] = 1;
		}
		for (int i = 0; i < nrOfNodes; i++) {  // set up inputs and outputs
			nodesNext[i] = 0;  // reset nodesNext
			if (nodes[i] == 1 && allCells[nodesAddresses[i]] == WIRE) {  // if this node is on and connects to wire
				allCells[nodesAddresses[i]] = CHARGE;
			}
		}

		for (int count = 0; count < chargeUpdatesPerUpdate; count++) {
			chargeUpdate();
			//displayBrainState();
		}
		//displayBrainState();
		nodes = nodesNext;

	}

	virtual void displayBrainState() {
		for (int i = 0; i < nrOfNodes; i++) {
			int l = nodesAddresses[i];
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);

			cout << "node" << i << "@(" << cellX << "," << cellY << "," << cellZ << ")=" << nodes[i] << "  ";
		}

		cout << endl;
		cout << "---------------------------------------------------\n";

		int w = 0;
		int d = 0;

		for (auto cell : allCells) {
			if (cell == 0) {
				cout << ".";
			}
			if (cell == 1) {
				cout << "-";
			}
			if (cell == 2) {
				cout << "*";
			}
			if (cell == 3) {
				cout << "O";
			}

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
		for (int i = 0; i < nrOfNodes; i++) {
			int l = nodesNextAddresses[i];
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);

			cout << "node" << i << "@(" << cellX << "," << cellY << "," << cellZ << ")=" << nodesNext[i] << "  ";
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

		dataPairs.push_back("brainWireCount");
		dataPairs.push_back(to_string(wireAddresses.size()));

		return dataPairs;
	}
	virtual void resetBrain() override {
	}

 public:
	inline void setState(const int& state, const double& value) override {
		if (state < (int) nodes.size()) {
			nodes[state] = value;
		} else {
			cout << "Writing to invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}
	inline double getState(const int& state) override {
		if (state < (int) nodes.size()) {
			return nodes[state];
		} else {
			cout << "Reading from invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}

}
;

#endif /* defined(__BasicMarkovBrainTemplate__WireBrain__) */
