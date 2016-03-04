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
	set<shared_ptr<cell>> charged, chargedNext, decay, decayNext, wire;
	vector<shared_ptr<cell>> inputCells, outputCells;
	vector<bool> nodes, nodesNext;

	WireBrain(){
		width = 100;
		depth = 100;
		height = 5;
	}

	WireBrain(shared_ptr<AbstractGenome> genome, int _nrOfNodes) {
		width = 100;
		depth = 100;
		height = 5;

		// load genome into allCells
		auto genomeHandler = genome->newHandler(genome, true);
		for (int l = 0; l < width * depth * height; l++) {
			allCells.push_back(make_shared<cell>(genomeHandler->readInt(0, 1)));  // 0 = empty, 1 = wire
		}

		// make neibor connections
		for (int l = 0; l < width * depth * height; l++) {  // for every cell
			int cellX = (l % (width * height)) % width;  // find this cells x,y,z location in brain
			int cellY = (l % (width * height)) / width;
			int cellZ = l / (width * height);
			for (int x = -1; x < 2; x++) {	// for every neighbor in x,y,z, if that neighbor is wire, add it to the neighbor list
				for (int y = -1; y < 2; y++) {
					for (int z = -1; z < 2; z++) {
						int neighborIndex = (cellX + x) + ((cellY + y) * width) + ((cellZ + z) * (width * height));
						if ((neighborIndex >= 0) && (neighborIndex < (int)allCells.size())) {
							if ((allCells[neighborIndex]->value == 1) && (l != neighborIndex)) {
								allCells[l]->neighbors.push_back(allCells[neighborIndex]);
							}
						}
					}
				}
			}
		}
		nodes.resize(_nrOfNodes);

		if (width < (_nrOfNodes * 4)) {
			cout << "ERROR: WireBrain requires that width >- 4 x number of nodes!\nExiting\n";
		}
		for (int i = 0; i < _nrOfNodes; i++) {
			inputCells.push_back(make_shared<cell>(1));  // add a wire input
			inputCells[i]->neighbors.push_back(allCells[(width % _nrOfNodes) * i]);

			outputCells.push_back(make_shared<cell>(1));  // add a wire output
			allCells[((width * depth * height) - 1) - ((width % _nrOfNodes) * i)]->neighbors.push_back(outputCells[i]);
		}
	}

	virtual ~WireBrain() = default;

	virtual shared_ptr<AbstractBrain> makeBrainFromGenome(shared_ptr<AbstractGenome> _genome) {
		shared_ptr<WireBrain> newBrain = make_shared<WireBrain>(_genome, 15);
		return newBrain;
	}

	virtual void chargeUpdate() {
		// propagate inputs note, input cells are always on and do not go into charged or decay sets
		for (int i = 0; i < (int)inputCells.size(); i++) { // for each input cell
			if (inputCells[i]->value == 3) { // if charged
				for (auto n : inputCells[i]->neighbors) { // push charge to neighbors
					if (n->value == 1) {
						n->chargeCount++;
						chargedNext.insert(n);
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
		for (int i = 0; i < (int)outputCells.size(); i++) {
			nodesNext[i] = nodesNext[i] & (outputCells[i]->value == 3);
		}

	}

	virtual void update() {
		for (int i = 0; i < (int)nodes.size(); i++){ // set up
			nodesNext[i] = 0; // reset nodesNext
			outputCells[i]->value = 0; // clear values in outputCells
			inputCells[i]->value = 1 * nodes[i];
		}
		for (auto cell : charged){ // clear out anything in charged from last update
			cell->value = 1;
		}
		charged.clear();
		chargedNext.clear();

		for (auto cell : decay){ // clear out anything in decay from last update
			cell->value = 1;
		}


		decay.clear();
		decayNext.clear();
		for (int count = 0; count < width; count++) {
			chargeUpdate();
		}
		nodes = nodesNext;
	}

	virtual string description(){
		return "WireBrain\n";
	}
	virtual vector<string> getStats(){
		vector<string> stats;
		return stats;
	}
	virtual void resetBrain(){}

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
