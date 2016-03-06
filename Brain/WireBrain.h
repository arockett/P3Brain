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

	class cell {
 	public:
		int value;
		int chargeCount;
		vector<shared_ptr<cell>> neighbors;

		cell(int _value) {
			value = _value;
			chargeCount = 0;
		}
	};

 public:

	int width, depth, height, nrOfNodes;
	vector<shared_ptr<cell>> allCells;
	unordered_set<shared_ptr<cell>> charged, chargedNext, decay, decayNext, wire;
	vector<shared_ptr<cell>> inputCells, outputCells;
	vector<bool> nodes, nodesNext;

	WireBrain() {
		nrOfNodes = defaultNrOfBrainNodes;
		width = defaultWidth;
		height = defaultHeight;
		depth = defaultDepth;
	}

	WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) {
		nrOfNodes = _nrOfNodes;
		width = defaultWidth;
		height = defaultHeight;
		depth = defaultDepth;

		nodes.resize(_nrOfNodes);
		nodesNext.resize(_nrOfNodes);

		int connectionsCount = 0;
		int wireCount = 0;
		int emptyCount = 0;

		// load genome into allCells
		auto genomeHandler = genome->newHandler(genome, true);
		for (int l = 0; l < width * depth * height; l++) {
			allCells.push_back(make_shared<cell>(genomeHandler->readDouble(0, 1) < initialFillRatio));  // 0 = empty, 1 = wire
			(allCells[l]->value == 1)?wireCount++:emptyCount++;
		}

		// make neighbor connections
		for (int l = 0; l < width * depth * height; l++) {  // for every cell
			if (allCells[l]->value == 1) {
				int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
				int cellY = (l % (width * height)) / width;
				int cellZ = l / (width * height);
				for (int x = -1; x < 2; x++) {	// for every neighbor in x,y,z, if that neighbor is wire, add it to the neighbor list
					for (int y = -1; y < 2; y++) {
						for (int z = -1; z < 2; z++) {
							int testX = cellX + x;
							int testY = cellY + y;
							int testZ = cellZ + z;
							//cout << testX << " " << testY << " " << testZ << "\n";
							if (((testX >= 0) && (testX < width)) && ((testY >= 0) && (testY < height)) && ((testZ >= 0) && (testZ < depth))) {
								//cout << "  valid\n";
								int neighborIndex = testX + (testY * width) + (testZ * (width * height));
								if ((allCells[neighborIndex]->value == 1) && (l != neighborIndex)) {
									connectionsCount++;
									allCells[l]->neighbors.push_back(allCells[neighborIndex]);
								}
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
		//		for (int i = 0; i < nrOfNodes; i++) {
		//			nodesAddresses[i] = worldConnectionsSeparation * i;
		//			nodesNextAddresses[i] = ((width * depth * height) - 1) - (worldConnectionsSeparation * i);
		//			//cout << worldConnectionsSeparation * i << " " << nodesAddresses[i] << "    " << nodesNextAddresses[i] << "\n";
		//		}
		for (int i = 0; i < nrOfNodes; i++) {
			inputCells.push_back(make_shared<cell>(1));  // add a wire input
			if (allCells[worldConnectionsSeparation * i]->value == 1) {
				inputCells[i]->neighbors.push_back(allCells[worldConnectionsSeparation * i]);
				connectionsCount++;
			}
			outputCells.push_back(make_shared<cell>(1));  // add a wire output
			if (allCells[((width * depth * height) - 1) - (worldConnectionsSeparation * i)]->value == 1) {
				allCells[((width * depth * height) - 1) - (worldConnectionsSeparation * i)]->neighbors.push_back(outputCells[i]);
				connectionsCount++;
			}
		}
		//cout << "  made wire brain with : " << connectionsCount << " connections (w:"<<wireCount<<"/e:"<<emptyCount<<")\n";
	}

	virtual ~WireBrain() = default;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
		shared_ptr<WireBrain> newBrain = make_shared<WireBrain>(_genome, defaultNrOfBrainNodes);
		return newBrain;
	}

	virtual void chargeUpdate() {
		int evalCount = 0;
		//cout << "in chargeUpdate()\n";
		// propagate inputs note, input cells are always on and do not go into charged or decay sets
		for (int i = 0; i < (int) inputCells.size(); i++) {  // for each input cell
			if (inputCells[i]->value == 3) {  // if charged
				for (auto n : inputCells[i]->neighbors) {  // push charge to neighbors
					if (n->value == 1) {
						n->chargeCount++;
						chargedNext.insert(n);
						evalCount++;
					}
				}
				if (!constantInputs){
					inputCells[i]->value = 0;
				}
			}
		}
		// propagate charge in the brain
		for (auto cell : charged) {
			for (auto n : cell->neighbors) {
				if (n->value == 1) {
					n->chargeCount++;
					chargedNext.insert(n);
					evalCount++;
				}
			}
			cell->value = 2;  // set this cells value to decay
			decayNext.insert(cell);  // add this cell to decay
		}
		// remove cells from decay
		for (auto cell : decay) {
			cell->value = 1;
		}

		// determine which cells in chargedCells are overcharged
		charged.clear();
		for (auto cell : chargedNext) {
			if (cell->chargeCount < overchargeThreshold) {
				charged.insert(cell);
				cell->value = 3;
			}
			cell->chargeCount = 0;
		}

		chargedNext.clear();
		decay = decayNext;
		decayNext.clear();

		// read and accumulate outputs
		// NOTE: output cells can go into charge/decay sets
		for (int i = 0; i < (int) outputCells.size(); i++) {
			nodesNext[i] = nodesNext[i] | (outputCells[i]->value == 3);
			//cout << outputCells[i]->value << " ? " << (int)nodesNext[i] << " | " << (outputCells[i]->value == 3) << " :: ";
		}
		//cout << endl;

		//cout << " ec: "	<< evalCount++;

	}

	virtual void update() {
		//cout << "in update()\n";
		for (int i = 0; i < (int) nodes.size(); i++) {  // set up
			nodesNext[i] = 0;  // reset nodesNext
			outputCells[i]->value = 1;  // clear values in outputCells
			inputCells[i]->value = 3 * nodes[i];  // inputs will be 0 or 3
		}
		//cout << "initialized I/O\n";
		for (auto cell : charged) {  // clear out anything in charged from last update
			cell->value = 1;
		}
		charged.clear();
		chargedNext.clear();

		for (auto cell : decay) {  // clear out anything in decay from last update
			cell->value = 1;
		}

		decay.clear();
		decayNext.clear();
		for (int count = 0; count < chargeUpdatesPerUpdate; count++) {
			chargeUpdate();
			//description();
			//cout << "--\n";
		}
		//exit(1);
		nodes = nodesNext;
//		for (auto n : nodes) {
//			cout << n << " ";
//		}
//		cout << endl;
		//description();
		//cout << "\n\n";
	}

	virtual string description() {
		int w = 0;
		int inOutSpacing = width / (int) inputCells.size();  // space between inputs and outputs
		for (int i = 0; i < width; i++) {
			if (i % inOutSpacing == 0 && i / inOutSpacing < (int) inputCells.size()) {
				cout << inputCells[i / inOutSpacing]->value;
			} else {
				cout << "x";
			}
		}
		cout << endl;
		for (auto cell : allCells) {
			cout << cell->value;
			w++;
			if (w == width) {
				cout << "\n";
				w = 0;
			}
		}
		for (int i = 0; i < width; i++) {
			if ((width - 1 - i) % inOutSpacing == 0 && (width - 1 - i) / inOutSpacing < (int) inputCells.size()) {
				cout << outputCells[(width - 1 - i) / inOutSpacing]->value;
			} else {
				cout << "x";
			}
		}
		cout << endl;
		return "WireBrain\n";
	}
	virtual vector<string> getStats() {
		vector<string> stats;
		return stats;
	}
	virtual void resetBrain() {
	}

 public:
	inline void setState(const int& state, const double& value) {
		if (state < (int) nodes.size()) {
			nodes[state] = value;
		} else {
			cout << "Writing to invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}
	inline double getState(const int& state) {
		if (state < (int) nodes.size()) {
			return nodes[state];
		} else {
			cout << "Reading from invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}

};

class WireBrain2 : public AbstractBrain {

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

	WireBrain2() {
		width = defaultWidth;
		height = defaultHeight;
		depth = defaultDepth;
		nrOfNodes = defaultNrOfBrainNodes;

	}

	WireBrain2(shared_ptr<AbstractGenome> genome, int _nrOfNodes) {
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
			allCells[l] = genomeHandler->readDouble(0, 1) < initialFillRatio;  // 0 = empty, 1 = wireAddresses
			if (allCells[l] == 1) {
				wireAddresses.push_back(l);
				wireCount++;
			} else {
				emptyCount++;
			}
		}

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
							if ((allCells[neighborIndex] == 1) && (l != neighborIndex)) {
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

	virtual ~WireBrain2() = default;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
		shared_ptr<WireBrain2> newBrain = make_shared<WireBrain2>(_genome, defaultNrOfBrainNodes);
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
			if (allCells[cellAddress] == 1) {
				chargeCount = 0;
				for (auto n : neighbors[cellAddress]) {
					if (allCells[n] == 3) {
						chargeCount++;
						//evalCount++;
					}
				}
				if (chargeCount > 0 && chargeCount < overchargeThreshold) {
					nextAllCells[cellAddress] = 3;
					//evalToCharge++;
				}
			} else if (allCells[cellAddress] == 2) {
				nextAllCells[cellAddress] = 1;
				//evalCount++;
				//evalToWire++;
			} else if (allCells[cellAddress] == 3) {
				nextAllCells[cellAddress] = 2;
				//evalCount++;
				//evalToDecay++;
			}
		}
		allCells = nextAllCells;
		if (constantInputs) {
			for (int i = 0; i < nrOfNodes; i++) {  // for each input cell
				//evalCount++;
				if (nodes[i] == 1) {  // if this node is on
					if (allCells[nodesAddresses[i]] == 1 || allCells[nodesAddresses[i]] == 2) {  // if the connected location is uncharged wireAddresses...
						allCells[nodesAddresses[i]] = 3;  // charge it.
						//evalToCharge++;
					}
				}
			}
		}
		// read and accumulate outputs
		// NOTE: output cells can go into charge/decay sets
		for (int i = 0; i < nrOfNodes; i++) {
			nodesNext[i] = nodesNext[i] | (allCells[nodesNextAddresses[i]] == 3);
			//cout << outputCells[i]->value << " ? " << (int)nodesNext[i] << " | " << (outputCells[i]->value == 3) << " :: ";
		}
		//cout << endl;

		//cout << "e: " << evalCount << "  ToCharge: " << evalToCharge << "  ToDecay: " << evalToDecay << "  ToWire: " << evalToWire << "\n";
		//cout << wireCount << " " << flush;

	}

	virtual void update() {
		//cout << "in update()\n";
		for (auto w : wireAddresses) {  // clear out anything in charged from last update
			allCells[w] = 1;
		}
		for (int i = 0; i < nrOfNodes; i++) {  // set up
			nodesNext[i] = 0;  // reset nodesNext
			if (nodes[i] == 1 && allCells[nodesAddresses[i]] == 1) {  // if this node is on and connects to wire
				allCells[nodesAddresses[i]] = 3;
			}
		}
		//cout << "initialized I/O\n";

		for (int count = 0; count < chargeUpdatesPerUpdate; count++) {
			chargeUpdate();
			//description();
			//cout << "--\n";
		}
		//cout << "--::--\n";
		//exit(1);
//		for (auto n:nodes){
//			cout << n;
//		}
//		cout << "\n";
		nodes = nodesNext;
//		for (auto n : nodes) {
//			cout << n << " ";
//		}
//		cout << endl;
		//description();
		//cout << "\n\n";
	}

	virtual string description() {
		int w = 0;
		int d = 0;
		int inOutSpacing = worldConnectionsSeparation;  // space between inputs and outputs
		for (int i = 0; i < width; i++) {
			if (i % inOutSpacing == 0 && i / inOutSpacing < nrOfNodes) {
				cout << nodes[i / inOutSpacing] * 3;
			} else {
				cout << "x";
			}
		}
		cout << endl;
		for (auto cell : allCells) {
			cout << cell;
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
		for (int i = 0; i < width; i++) {
			if ((width - 1 - i) % inOutSpacing == 0 && (width - 1 - i) / inOutSpacing < nrOfNodes) {
				cout << nodesNext[(width - 1 - i) / inOutSpacing] * 3;
			} else {
				cout << "x";
			}
		}
		cout << endl;
		return "WireBrain\n";
	}
	virtual vector<string> getStats() {
		vector<string> stats;
		return stats;
	}
	virtual void resetBrain() {
	}

 public:
	inline void setState(const int& state, const double& value) {
		if (state < (int) nodes.size()) {
			nodes[state] = value;
		} else {
			cout << "Writing to invalid brain state - this brain needs more states!\nExiting\n";
			exit(1);
		}
	}
	inline double getState(const int& state) {
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
