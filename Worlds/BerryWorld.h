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

#include "../BaseCode/Brain.h"
#include "../BaseCode/Genome.h"
#include "../BaseCode/World.h"
#include "../Analyse/Analyse.h"
#include "../Utilities/Parameters.h"
#include "../Utilities/Utilities.h"


using namespace std;

class MadBotAI;

class BerryWorld : public World {
 public:
  const int numberOfDirections = 8;
  const int xm[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };  //these are directions
  const int ym[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };

  const int EMPTY = 0;
  const int RED = 1;
  const int BLUE = 2;
  const int WALL = 9;

  static double& TSK;
  static int& worldUpdates;
  static int& foodSourceTypes;
  static double& rewardForFood1;
  static double& rewardForFood2;
  static double& rewardForFood3;
  static double& rewardForFood4;
  static double& rewardForFood5;
  static double& rewardForFood6;
  static double& rewardForFood7;
  static double& rewardForFood8;

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

  int inputStatesCount = 0;
  int outputStatesCount = 0;

  BerryWorld();

  double testIndividual(shared_ptr<Organism> org, bool analyse);

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
  vector<int> makeTestGrid();
  inline int turnLeft(int facing) {
    return (facing < 1) ? numberOfDirections : facing-1;
  }
  inline int turnRight(int facing) {
    return ((facing >= (numberOfDirections - 1))? 0 : facing+1);
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__BerryWorld__) */
