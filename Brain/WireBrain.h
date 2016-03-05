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

	int width, depth, height;
	vector<shared_ptr<cell>> allCells;
	unordered_set<shared_ptr<cell>> charged, chargedNext, decay, decayNext, wire;
	vector<shared_ptr<cell>> inputCells, outputCells;
	vector<bool> nodes, nodesNext;

	WireBrain() {
		width = 46;
		height = 20;
		depth = 20;
	}

	WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) {
		width = 46;
		height = 20;
		depth = 20;

		nodes.resize(_nrOfNodes);
		nodesNext.resize(_nrOfNodes);

		int connectionsCount = 0;
		int wireCount = 0;
		int emptyCount = 0;

		// load genome into allCells
		auto genomeHandler = genome->newHandler(genome, true);
		for (int l = 0; l < width * depth * height; l++) {
			allCells.push_back(make_shared<cell>(genomeHandler->readInt(0, 4) / 4));  // 0 = empty, 1 = wire
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

		if (width < (_nrOfNodes * 3)) {
			cout << "ERROR: WireBrain requires that width >- 4 x number of nodes!\nExiting\n";
			exit(1);
		}
		for (int i = 0; i < _nrOfNodes; i++) {
			inputCells.push_back(make_shared<cell>(1));  // add a wire input
			if (allCells[(width / _nrOfNodes) * i]->value == 1) {
				inputCells[i]->neighbors.push_back(allCells[(width / _nrOfNodes) * i]);
				connectionsCount++;
			}
			outputCells.push_back(make_shared<cell>(1));  // add a wire output
			if (allCells[((width * depth * height) - 1) - ((width / _nrOfNodes) * i)]->value == 1) {
				allCells[((width * depth * height) - 1) - ((width / _nrOfNodes) * i)]->neighbors.push_back(outputCells[i]);
				connectionsCount++;
			}
		}
		cout << "  made wire brain with : " << connectionsCount << " connections (w:"<<wireCount<<"/e:"<<emptyCount<<")\n";
	}

	virtual ~WireBrain() = default;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
		shared_ptr<WireBrain> newBrain = make_shared<WireBrain>(_genome, 15);
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
			if (cell->chargeCount < 3) {
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
		for (int count = 0; count < 100; count++) {
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

#endif /* defined(__BasicMarkovBrainTemplate__WireBrain__) */
