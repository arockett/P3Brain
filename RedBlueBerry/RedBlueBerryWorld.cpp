//
//  RedBlueBerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "RedBlueBerryWorld.h"
#include "Random.h"
/* *** implementation of the World *** */

double& RedBlueBerryWorld::TSK = Parameters::register_parameter("RBW_taskSwitchingCost", 1.4,
		"cost to change food sources", "WORLD - RED BLUE");
int& RedBlueBerryWorld::foodSourceTypes = Parameters::register_parameter("RBW_foodSourceTypes", 2,
		"number of types of food", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood1 = Parameters::register_parameter("RBW_rewardForFood1", 1.0,
		"reward for eating a Food1", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood2 = Parameters::register_parameter("RBW_rewardForFood2", 1.0,
		"reward for eating a Food2", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood3 = Parameters::register_parameter("RBW_rewardForFood3", 1.0,
		"reward for eating a Food3", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood4 = Parameters::register_parameter("RBW_rewardForFood4", 1.0,
		"reward for eating a Food4", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood5 = Parameters::register_parameter("RBW_rewardForFood5", 1.0,
		"reward for eating a Food5", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood6 = Parameters::register_parameter("RBW_rewardForFood6", 1.0,
		"reward for eating a Food6", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood7 = Parameters::register_parameter("RBW_rewardForFood7", 1.0,
		"reward for eating a Food7", "WORLD - RED BLUE");
double& RedBlueBerryWorld::rewardForFood8 = Parameters::register_parameter("RBW_rewardForFood8", 1.0,
		"reward for eating a Food8", "WORLD - RED BLUE");

int& RedBlueBerryWorld::xDim = Parameters::register_parameter("RBW_WorldX", 8, "world X size", "WORLD - RED BLUE");
int& RedBlueBerryWorld::yDim = Parameters::register_parameter("RBW_WorldY", 8, "world Y size", "WORLD - RED BLUE");
bool& RedBlueBerryWorld::borderWalls = Parameters::register_parameter("RBW_makeBorderWalls", true,
		"if true world will have a bounding wall", "WORLD - RED BLUE");
int& RedBlueBerryWorld::randomWalls = Parameters::register_parameter("RBW_makeRandomWalls", 0,
		"add this many walls to the world", "WORLD - RED BLUE");

bool& RedBlueBerryWorld::clearOutputs = Parameters::register_parameter("RBW_clearOutputs", false,
		"if true outputs will be cleared on each world update", "WORLD - RED BLUE");

bool& RedBlueBerryWorld::allowMoveAndEat = Parameters::register_parameter("RBW_allowMoveAndEat", false,
		"if true, the agent can move and eat in the same world update", "WORLD - RED BLUE");

bool& RedBlueBerryWorld::senseDown = Parameters::register_parameter("RBW_senseDown", true,
		"if true, Agent can sense what it's standing on", "WORLD - RED BLUE");
bool& RedBlueBerryWorld::senseFront = Parameters::register_parameter("RBW_senseFront", true,
		"if true, Agent can sense what's in front of it", "WORLD - RED BLUE");
bool& RedBlueBerryWorld::senseFrontSides = Parameters::register_parameter("RBW_senseFrontSides", true,
		"if true, Agent can sense what's in front to the left and right of it", "WORLD - RED BLUE");
bool& RedBlueBerryWorld::senseWalls = Parameters::register_parameter("RBW_senseWalls", true,
		"if true, Agent can sense Walls", "WORLD - RED BLUE");

RedBlueBerryWorld::RedBlueBerryWorld() {
	senseWalls = senseWalls & (borderWalls | (randomWalls > 0)); // if there are no walls, there is no need to sense them!

	outputStatesCount = 3; // number of brain states used for output, 2 for move, 1 for eat

	if (senseWalls) {
		inputStatesCount = (senseDown * foodSourceTypes) + ((senseFront * foodSourceTypes) + senseWalls)
				+ (2 * ((senseFrontSides * foodSourceTypes) + senseWalls));
		// sense down does not include walls (can't stand on a wall (yet!) * types of food
		// senseFront * types of food + wall, same for senseFrontSides, but there are 2
	} else { // no border walls
		inputStatesCount = (senseDown * foodSourceTypes) + (senseFront * foodSourceTypes)
				+ (2 * (senseFrontSides * foodSourceTypes));
		// sense down * types of food, same for senseFront, same for senseFrontSides, but there are 2
	}
}

double RedBlueBerryWorld::testIndividual(Agent *agent, bool analyse) {
	int grid[xDim][yDim];
	int i, j;
	int xp = xDim / 2, yp = yDim / 2, dir = Random::getInt(7); //xp and yp are x and y positions. agent starts at center of grid and with a random rotation

	int switched = 0;
	int statesAssignmentCounter;

	vector<int> stateCollector;
	double score = 0.0;
	int lastFood = -1; //nothing has been eaten yet!

	int foodSourceTypes = 2;
	vector<int> foodRewards;
	foodRewards.resize(8);
	foodRewards[0] = rewardForFood1;
	foodRewards[1] = rewardForFood2;
	foodRewards[2] = rewardForFood3;
	foodRewards[3] = rewardForFood4;
	foodRewards[4] = rewardForFood5;
	foodRewards[5] = rewardForFood6;
	foodRewards[6] = rewardForFood7;
	foodRewards[7] = rewardForFood8;

	vector<int> eaten; //the array eaten has 2 elements and they are both 0
	eaten.resize(foodSourceTypes);

	if (borderWalls) {
		for (i = 1; i < xDim - 1; i++)
			for (j = 1; j < yDim - 1; j++) { //as you go across the x and y directions in grid...
				grid[i][j] = Random::getInt(1) + 1; // plant red or blue food
			}
		for (i = 0; i < xDim; i++) { //makes horizontal walls
			grid[i][0] = WALL;
			grid[i][yDim - 1] = WALL;
		}
		for (j = 0; j < yDim; j++) { //makes vertical walls
			grid[0][j] = WALL;
			grid[xDim - 1][j] = WALL;
		}

	} else { // no border walls
		for (i = 0; i < xDim; i++) {
			for (j = 0; j < yDim; j++) { //as you go across the x and y directions in grid...
				grid[i][j] = Random::getInt(1) + 1; // plant red or blue food
			}
		}
	}

	for (int i = 0; i < randomWalls; i++) { // add walls
		grid[Random::getIndex(xDim)][Random::getIndex(yDim)] = WALL;
	}

	agent->resetBrain();

	int output1 = 0;
	int output2 = 0;

	int here, leftFront, front, rightFront;

	for (int t = 0; t < 400; t++) { //run agent for t brain updates

		here = grid[xp][yp]; // where the agent is standing
		front = grid[loopMod((xp + xm[dir]), (xDim))][loopMod((yp + ym[dir]), (yDim))];
		leftFront = grid[loopMod((xp + xm[(dir - 1) & 7]), (xDim))][loopMod((yp + ym[(dir - 1) & 7]), (yDim))];
		rightFront = grid[loopMod((xp + xm[(dir + 1) & 7]), (xDim))][loopMod((yp + ym[(dir + 1) & 7]), (yDim))];

		statesAssignmentCounter = 0;

		if (senseWalls) {
			if (senseDown) {
				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at here location
					agent->setState(statesAssignmentCounter++, (here == i + 1));
				}
			}
			if (senseFront) {
				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
					agent->setState(statesAssignmentCounter++, (front == i + 1));
				}
				agent->setState(statesAssignmentCounter++, (front == WALL));
			}
			if (senseFrontSides) {
				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
					agent->setState(statesAssignmentCounter++, (leftFront == i + 1));
					agent->setState(statesAssignmentCounter++, (rightFront == i + 1));
				}
				agent->setState(statesAssignmentCounter++, (leftFront == WALL));
				agent->setState(statesAssignmentCounter++, (rightFront == WALL));
			}
		} else { // don't sense walls
			if (senseDown) {
				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at here location
					agent->setState(statesAssignmentCounter++, (here == i + 1));
				}
			}
			if (senseFront) {
				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
					agent->setState(statesAssignmentCounter++, (front == i + 1));
				}
			}
			if (senseFrontSides) {
				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
					agent->setState(statesAssignmentCounter++, (leftFront == i + 1));
					agent->setState(statesAssignmentCounter++, (rightFront == i + 1));
				}
			}
		}

//		if (senseWalls) {
//			if (senseDown) {
//				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at here location
//					agent->states[statesAssignmentCounter++] = (here == i + 1);
//				}
//			}
//			if (senseFront) {
//				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
//					agent->states[statesAssignmentCounter++] = (front == i + 1);
//				}
//				agent->states[statesAssignmentCounter++] = (front == WALL);
//			}
//			if (senseFrontSides) {
//				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
//					agent->states[statesAssignmentCounter++] = (leftFront == i + 1);
//					agent->states[statesAssignmentCounter++] = (rightFront == i + 1);
//				}
//				agent->states[statesAssignmentCounter++] = (leftFront == WALL);
//				agent->states[statesAssignmentCounter++] = (rightFront == WALL);
//			}
//		} else { // don't sense walls
//			if (senseDown) {
//				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at here location
//					agent->states[statesAssignmentCounter++] = (here == i + 1);
//				}
//			}
//			if (senseFront) {
//				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
//					agent->states[statesAssignmentCounter++] = (front == i + 1);
//				}
//			}
//			if (senseFrontSides) {
//				for (int i = 0; i < foodSourceTypes; i++) { // fill first states with food values at front location
//					agent->states[statesAssignmentCounter++] = (leftFront == i + 1);
//					agent->states[statesAssignmentCounter++] = (rightFront == i + 1);
//				}
//			}
//		}

//		if (borderWalls) {
//		agent->states[0] = (here == RED); // standing on red
//		agent->states[1] = (here == BLUE); // standing on blue
//		agent->states[2] = (front == RED); // front is red
//		agent->states[3] = (front == BLUE); // front is blue
//		agent->states[4] = (front == WALL); // front is wall
//		agent->states[5] = (leftFront == RED); // front left is red
//		agent->states[6] = (rightFront == RED); // front right is red
//		agent->states[7] = (leftFront == BLUE); // front left is blue
//		agent->states[8] = (rightFront == BLUE); // front right is blue
//		agent->states[9] = (leftFront == WALL); // front left is wall
//		agent->states[10] = (rightFront == WALL); // front right is wall
//
//		} else {
//			agent->states[0] = here == RED; // standing on red
//			agent->states[1] = here == BLUE; // standing on blue
//			agent->states[2] = leftFront == RED; // front left is red
//			agent->states[3] = front == RED; // front is red
//			agent->states[4] = rightFront == RED; // front right is red
//			agent->states[5] = leftFront == BLUE; // front left is blue
//			agent->states[6] = front == BLUE; // front is blue
//			agent->states[7] = rightFront == BLUE; // front right is blue
//		}

		if (clearOutputs) {
			agent->setState(inputStatesCount, 0.0);
			agent->setState(inputStatesCount + 1, 0.0);
			agent->setState(inputStatesCount + 2, 0.0);
		}

		if (analyse) { // gather some datta before and after running update
			int S = 0;
			for (int i = 0; i < inputStatesCount; i++)
				S = (S << 1) + Bit(agent->states[i]);
			agent->updateStates();
			for (int i = inputStatesCount + outputStatesCount; i < agent->nrOfBrainStates; i++)
				S = (S << 1) + Bit(agent->states[i]);
			stateCollector.push_back(S);
		} else {
			agent->updateStates(); // just run the update!
		}

		// set output values
		output1 = Bit(agent->getState(inputStatesCount)) + (Bit(agent->getState(inputStatesCount + 1)) << 1);
		//output2 = Bit(agent->getState(inputStatesCount + 2));

		if ((output1 == 0) && (grid[xp][yp] != EMPTY)) { // eat food here (if there is food here)
			if (lastFood != -1) { // if some food has already been eaten
				if (lastFood != grid[xp][yp]) { // if this food is different then the last food eaten
					score -= TSK; // pay the task switch cost
					switched++;
				}
			}
			score += foodRewards[grid[xp][yp] - 1]; // you ate a food... good for you!
			lastFood = grid[xp][yp]; // remember the last food eaten
			eaten[grid[xp][yp] - 1]++; // track the number of each berry eaten
			grid[xp][yp] = 0; // clear this location
		}
		if (1) {//((output2 == 0) || (allowMoveAndEat == 1)) { // if we did not eat or we allow moving and eating in the same world update
			switch (output1) {
			case 0: //nothing
				break;
			case 1: //turn left
				dir = (dir - 1) & 7;
				break;
			case 2: //turn right
				dir = (dir + 1) & 7;
				break;
			case 3: //move forward
				if (grid[loopMod((xp + xm[dir]), (xDim))][loopMod((yp + ym[dir]), (yDim))] != WALL) { // if the proposed move is not a wall
					if (grid[xp][yp] == EMPTY) { // if the current location is empty
						grid[xp][yp] = Random::getInt(1) + 1; // plant a red or blue food
					}
					xp = loopMod((xp + xm[dir]), xDim); // move to the new X
					yp = loopMod((yp + ym[dir]), yDim); // move to the new Y
				}
				break;
			}
		}
//		Data::Add(xp, "x"+to_string(t), agent->genome);
//		Data::Add(yp, "y"+to_string(t), agent->genome);

		/* uncommnet to print test output
		 for(int x=0;x<xDim;x++){
		 for(int y=0;y<yDim;y++){
		 if((x==xp)&&(y==yp))
		 printf("X");
		 else
		 printf("%i",grid[x][y]);
		 }
		 printf("\n");
		 }
		 printf("%f\n",score);
		 for (int blah = 0; blah < 1000000; blah++){}
		 */
	}
	if (score < 0.0) {
		score = 0.0;
	}
	Data::Add(switched, "switches", agent->genome->data);
	Data::Add(eaten[0], "red", agent->genome->data);
	Data::Add(eaten[1], "blue", agent->genome->data);
	Data::Add(eaten[0] + eaten[1], "total", agent->genome->data);
	Data::Add(score, "score", agent->genome->data);
	if (analyse) {
		Data::Add(Analyse::computeAtomicPhi(stateCollector, agent->nrOfBrainStates), "phi", agent->genome->data);
	}
	return score;
}
