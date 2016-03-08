//
//  BerryWorld.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__BerryWorld__
#define __BasicMarkovBrainTemplate__BerryWorld__

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "../Genome/Genome.h"
#include "../World/World.h"
#include "../Analyze/Analyze.h"
#include "../Brain/Brain.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Random.h"

using namespace std;

class MadBotAI;

class BerryWorld : public World {
 public:
	const int numberOfDirections = 8;
	const int xm[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };  //these are directions
	const int ym[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	const char facingDisplay[8] = { 94, 47, 62, 92, 118, 47, 60, 92 };

	const int EMPTY = 0;
	const int WALL = 9;

	static double& TSK;
	static int& worldUpdates;
	static int& foodTypes;
	static double& rewardForFood1;
	static double& rewardForFood2;
	static double& rewardForFood3;
	static double& rewardForFood4;
	static double& rewardForFood5;
	static double& rewardForFood6;
	static double& rewardForFood7;
	static double& rewardForFood8;

	static double& rewardForTurn;
	static double& rewardForMove;

	static int& ratioFood0;
	static int& ratioFood1;
	static int& ratioFood2;
	static int& ratioFood3;
	static int& ratioFood4;
	static int& ratioFood5;
	static int& ratioFood6;
	static int& ratioFood7;
	static int& ratioFood8;

	static int& WorldY;
	static int& WorldX;
	static bool& borderWalls;
	static int& randomWalls;

	static bool& allowMoveAndEat;

	static bool& senseDown;
	static bool& senseFront;
	static bool& senseFrontSides;
	static bool& senseWalls;
	static bool& clearOutputs;

	static int& replacement;

	int inputStatesCount = 0;
	int outputStatesCount = 0;

	int foodRatioCount;  // sum of ratioFood for foods in use
	vector<int> foodRatioLookup;
	vector<double> foodRewards;

	BerryWorld();

//  double testIndividual(shared_ptr<Organism> org, bool analyse);

	double testIndividual(shared_ptr<Organism> org, bool analyse, bool show = 0) override;

	int pickFood(int lastfood) {
		//cout << "In BerryWorld::pickFood(int lastfood)\n";
		int lookup, counter, pick;
		if (lastfood < 1) {  // if lastfood is < 0 (or was 0) then return a random food
			lookup = Random::getInt(1, foodRatioCount);  // get a random int [1,sum of food ratios]
			counter = foodRatioLookup[0];  // set the counter to the ratio of replace with empty (0)
			pick = 0;  // set pick to empty (0)
			while (counter < lookup) {
				pick++;  // this is not our pick, so advance to the next pick and...
				counter += foodRatioLookup[pick];  // add this new picks ratio to counter
			}
		} else {  // if given a last food, pick a food that is not that.
			if (lastfood > foodTypes) {
				cout << "ERROR: In BerryWorld::pickFood() - lastfood > foodTypes (i.e. last food eaten is not in foodTypes!)\nExiting.\n\n";
				exit(1);
			}
			if (foodRatioCount - foodRatioLookup[lastfood] == 0){
				cout << "ERROR: In BerryWorld::pickFood() - lastfood set, but there is only one foodType so pick can not be a diffrent foodType\n\nExiting";
				exit(1);
			}
			lookup = Random::getInt(1, foodRatioCount - foodRatioLookup[lastfood]);  // get a random int [1,sum of food ratios] but leave out the ratio of last food
			counter = foodRatioLookup[0];  // set the counter to the ratio of replace with empty (0)
			pick = 0;  // set pick to empty (0)
			if (lastfood == 0) {  // if the last food was empty...
				counter = foodRatioLookup[1];  // set counter to ratio of food1 instead
				pick = 1;  // set pick to food1 (1)
			}
			while (counter < lookup) {
				pick++;  // this is not our pick, so advance to the next pick and...
				if (pick == lastfood) {
					pick++;  // if the new pick = lastfood, then skip it and...
				}
				counter += foodRatioLookup[pick];  // add this new picks ratio to counter
			}
		}
		//cout << "  Leaving BerryWorld::pickFood(int lastfood)\n";
		return pick;

	}

	// convert x,y into a grid index
	int getGridIndexFromXY(pair<int, int> loc) {
		return loc.first + (loc.second * BerryWorld::WorldX);
	}

	// return value on grid at index
	int getGridValue(const vector<int> &grid, int index) {
		return grid[index];
	}

	// return the value on grid at x,y
	int getGridValue(const vector<int> &grid, pair<int, int> loc) {
		return getGridValue(grid, getGridIndexFromXY(loc));
	}

	// takes x,y and updates them by moving one step in facing
	pair<int, int> moveOnGrid(pair<int, int> loc, int facing) {
		return {loopMod((loc.first + xm[facing]), BerryWorld::WorldX), loopMod((loc.second + ym[facing]), BerryWorld::WorldY)};
	}

	// update value at index in grid
	void setGridValue(vector<int> &grid, int index, int value) {
		grid[index] = value;
	}

	// update value at x,y in grid
	void setGridValue(vector<int> &grid, pair<int, int> loc, int value) {
		setGridValue(grid, getGridIndexFromXY(loc), value);
	}

	// return a vector of size x*y
	vector<int> makeGrid(int x, int y) {
		vector<int> grid;
		grid.resize(x * y);
		return grid;
	}

	// return a vector of size x*y (grid) with walls with borderWalls (if borderWalls = true) and randomWalls (that many) randomly placed walls
	vector<int> makeTestGrid() {
		vector<int> grid = makeGrid(WorldX, WorldY);

		for (int y = 0; y < WorldY; y++) {  // fill grid with food (and outer wall if needed)
			for (int x = 0; x < WorldX; x++) {
				if (borderWalls && (x == 0 || x == WorldX - 1 || y == 0 || y == WorldY - 1)) {
					setGridValue(grid, { x, y }, WALL);  // place walls on edge
				} else {
					setGridValue(grid, { x, y }, pickFood(-1));  // place random food where there is not a wall
				}
			}
		}

		if ((randomWalls >= WorldX * WorldY) && !borderWalls) {
			cout << "In BerryWorld::makeTestGrid() To many random walls... exiting!" << endl;
			exit(1);
		}
		if ((randomWalls >= (WorldX - 2) * (WorldY - 2)) && borderWalls) {
			cout << "In BerryWorld::makeTestGrid() To many random walls... exiting!" << endl;
			exit(1);
		}

		for (int i = 0; i < randomWalls; i++) {  // add random walls
			if (borderWalls) {
				setGridValue(grid, { Random::getInt(1, WorldX - 2), Random::getInt(1, WorldY - 2) }, WALL);  // if borderWalls than don't place random walls on the outer edge
			} else {
				setGridValue(grid, { Random::getIndex(WorldX), Random::getIndex(WorldY) }, WALL);  // place walls anywhere
			}
		}
		return grid;
	}

	inline int turnLeft(int facing) {
		return (facing < 1) ? numberOfDirections - 1 : facing - 1;
	}
	inline int turnRight(int facing) {
		return ((facing >= (numberOfDirections - 1)) ? 0 : facing + 1);
	}

	void printGrid(vector<int> grid, pair<int, int> loc, int facing);
};

#endif /* defined(__BasicMarkovBrainTemplate__BerryWorld__) */
