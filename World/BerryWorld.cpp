//
//  BerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "BerryWorld.h"

const double& BerryWorld::TSK = Parameters::register_parameter("BERRY_taskSwitchingCost", 1.4, "cost to change food sources", "WORLD - BERRY");
const int& BerryWorld::defaultWorldUpdates = Parameters::register_parameter("BERRY_WorldUpdates", 400, "amount of time an brain is tested", "WORLD - BERRY");

const int& BerryWorld::foodTypes = Parameters::register_parameter("BERRY_foodTypes", 2, "number of types of food", "WORLD - BERRY");
const double& BerryWorld::rewardForFood1 = Parameters::register_parameter("BERRY_rewardForFood1", 1.0, "reward for eating a Food1", "WORLD - BERRY");
const double& BerryWorld::rewardForFood2 = Parameters::register_parameter("BERRY_rewardForFood2", 1.0, "reward for eating a Food2", "WORLD - BERRY");
const double& BerryWorld::rewardForFood3 = Parameters::register_parameter("BERRY_rewardForFood3", 1.0, "reward for eating a Food3", "WORLD - BERRY");
const double& BerryWorld::rewardForFood4 = Parameters::register_parameter("BERRY_rewardForFood4", 1.0, "reward for eating a Food4", "WORLD - BERRY");
const double& BerryWorld::rewardForFood5 = Parameters::register_parameter("BERRY_rewardForFood5", 1.0, "reward for eating a Food5", "WORLD - BERRY");
const double& BerryWorld::rewardForFood6 = Parameters::register_parameter("BERRY_rewardForFood6", 1.0, "reward for eating a Food6", "WORLD - BERRY");
const double& BerryWorld::rewardForFood7 = Parameters::register_parameter("BERRY_rewardForFood7", 1.0, "reward for eating a Food7", "WORLD - BERRY");
const double& BerryWorld::rewardForFood8 = Parameters::register_parameter("BERRY_rewardForFood8", 1.0, "reward for eating a Food8", "WORLD - BERRY");

const double& BerryWorld::rewardForTurn = Parameters::register_parameter("BERRY_rewardForTurn", 0.0, "reward for turning", "WORLD - BERRY - ADVANCED");
const double& BerryWorld::rewardForMove = Parameters::register_parameter("BERRY_rewardForMove", 0.0, "reward for moving", "WORLD - BERRY - ADVANCED");

const int& BerryWorld::ratioFood0 = Parameters::register_parameter("BERRY_replacementRatioFood0", 0, "Relative likelihood to leave empty space empty", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood1 = Parameters::register_parameter("BERRY_replacementRatioFood1", 1, "Relative likelihood to place Food1", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood2 = Parameters::register_parameter("BERRY_replacementRatioFood2", 1, "Relative likelihood to place Food2", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood3 = Parameters::register_parameter("BERRY_replacementRatioFood3", 1, "Relative likelihood to place Food3", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood4 = Parameters::register_parameter("BERRY_replacementRatioFood4", 1, "Relative likelihood to place Food4", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood5 = Parameters::register_parameter("BERRY_replacementRatioFood5", 1, "Relative likelihood to place Food5", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood6 = Parameters::register_parameter("BERRY_replacementRatioFood6", 1, "Relative likelihood to place Food6", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood7 = Parameters::register_parameter("BERRY_replacementRatioFood7", 1, "Relative likelihood to place Food7", "WORLD - BERRY - ADVANCED");
const int& BerryWorld::ratioFood8 = Parameters::register_parameter("BERRY_replacementRatioFood8", 1, "Relative likelihood to place Food8", "WORLD - BERRY - ADVANCED");

const int& BerryWorld::WorldX = Parameters::register_parameter("BERRY_WorldX", 8, "world X size", "WORLD - BERRY");
const int& BerryWorld::WorldY = Parameters::register_parameter("BERRY_WorldY", 8, "world Y size", "WORLD - BERRY");
const bool& BerryWorld::borderWalls = Parameters::register_parameter("BERRY_makeBorderWalls", true, "if true world will have a bounding wall", "WORLD - BERRY");
const int& BerryWorld::randomWalls = Parameters::register_parameter("BERRY_makeRandomWalls", 0, "add this many walls to the world", "WORLD - BERRY - ADVANCED");

const bool& BerryWorld::clearOutputs = Parameters::register_parameter("BERRY_clearOutputs", false, "if true outputs will be cleared on each world update", "WORLD - BERRY");

const bool& BerryWorld::allowMoveAndEat = Parameters::register_parameter("BERRY_allowMoveAndEat", false, "if true, the brain can move and eat in the same world update", "WORLD - BERRY - ADVANCED");
const bool& BerryWorld::senseDown = Parameters::register_parameter("BERRY_senseDown", true, "if true, Agent can sense what it's standing on", "WORLD - BERRY");
const bool& BerryWorld::senseFront = Parameters::register_parameter("BERRY_senseFront", true, "if true, Agent can sense what's in front of it", "WORLD - BERRY");
const bool& BerryWorld::senseFrontSides = Parameters::register_parameter("BERRY_senseFrontSides", false, "if true, Agent can sense what's in front to the left and right of it", "WORLD - BERRY");
const bool& BerryWorld::senseWalls = Parameters::register_parameter("BERRY_senseWalls", false, "if true, Agent can sense Walls", "WORLD - BERRY");
const int& BerryWorld::replacement = Parameters::register_parameter("BERRY_replacement", -1, "-1 = random, 0 = no replacement, 1 = replace other (note: Food0, can be replace by Food0)", "WORLD - BERRY - ADVANCED");

const bool& BerryWorld::recordConsumptionRatio = Parameters::register_parameter("BERRY_recordConsumptionRatio", false, "if true, record greater of red/blue+1 or blue/red+1", "WORLD - BERRY - ADVANCED");
const bool& BerryWorld::recordFoodList = Parameters::register_parameter("BERRY_recordFoodList", true, "if true, record list of food eaten", "WORLD - BERRY - ADVANCED");
const bool& BerryWorld::recordFoodListEatEmpty = Parameters::register_parameter("BERRY_recordFoodListEatEmpty", false, "if true, foodList will include attempts to eat 0", "WORLD - BERRY - ADVANCED");
const bool& BerryWorld::recordFoodListNoEat = Parameters::register_parameter("BERRY_recordFoodListNoEat", false, "if true, if true foodList will include no eat (-1)", "WORLD - BERRY - ADVANCED");


BerryWorld::BerryWorld() {

	worldUpdates = defaultWorldUpdates;

	if (foodTypes < 1 || foodTypes > 8) {
		cout << "In BerryWorld you either have too few or too many foodTypes (must be >0 and <=8)\n\nExiting\n\n";
		exit(1);
	}
	//senseWalls = senseWalls & (borderWalls | (randomWalls > 0));  // if there are no walls, there is no need to sense them!

	outputNodesCount = 3;  // number of brain nodes used for output, 2 for move, 1 for eat

	if (senseWalls) {
		inputNodesCount = (senseDown * foodTypes) + ((senseFront * foodTypes) + senseWalls) + (2 * ((senseFrontSides * foodTypes) + senseWalls));
		// sense down does not include walls (can't stand on a wall (yet!) * types of food
		// senseFront * types of food + wall, same for senseFrontSides, but there are 2
	} else {  // no border walls
		inputNodesCount = (senseDown * foodTypes) + (senseFront * foodTypes) + (2 * (senseFrontSides * foodTypes));
		// sense down * types of food, same for senseFront, same for senseFrontSides, but there are 2
	}

	cout << "BerryWorld requires brains with at least " << inputNodesCount + outputNodesCount << " nodes.\n";
	if (inputNodesCount == 0) {
		cout << "    " << inputNodesCount << " Inputs\t No Inputs\n";
		cout << "    " << outputNodesCount << " Outputs\t nodes 0 to " << outputNodesCount - 1 << "\n";
	} else {
		cout << "    " << inputNodesCount << " Inputs\t nodes 0 to " << inputNodesCount - 1 << "\n";
		cout << "    " << outputNodesCount << " Outputs\t nodes " << inputNodesCount << " to " << inputNodesCount + outputNodesCount - 1 << "\n";

	}
	foodRatioLookup.resize(9);  // stores reward of each type of food NOTE: food is indexed from 1 so 0th entry is chance to leave empty
	foodRatioLookup[0] = ratioFood0;
	foodRatioLookup[1] = ratioFood1;
	foodRatioLookup[2] = ratioFood2;
	foodRatioLookup[3] = ratioFood3;
	foodRatioLookup[4] = ratioFood4;
	foodRatioLookup[5] = ratioFood5;
	foodRatioLookup[6] = ratioFood6;
	foodRatioLookup[7] = ratioFood7;
	foodRatioLookup[8] = ratioFood8;

	foodRatioTotal = 0;
	for (int i = 0; i <= foodTypes; i++) {
		foodRatioTotal += foodRatioLookup[i];
	}

	foodRewards.resize(9);  // stores reward of each type of food
	foodRewards[0] = 0;
	foodRewards[1] = rewardForFood1;
	foodRewards[2] = rewardForFood2;
	foodRewards[3] = rewardForFood3;
	foodRewards[4] = rewardForFood4;
	foodRewards[5] = rewardForFood5;
	foodRewards[6] = rewardForFood6;
	foodRewards[7] = rewardForFood7;
	foodRewards[8] = rewardForFood8;

	// columns to be added to ave file
	aveFileColumns.clear();
	aveFileColumns.push_back("score");
	aveFileColumns.push_back("total");
	if (foodTypes > 1) {
		aveFileColumns.push_back("switches");
	}
	for (int i = 0; i <= foodTypes; i++) {
		string temp_name = "food" + to_string(i);  // make food names i.e. food1, food2, etc.
		aveFileColumns.push_back(temp_name);
	}
	if (recordConsumptionRatio) {  // consumption ratio displays high value of org favors one food over the other and low values if both are being consumed. works on food[0] and food[1] only
		aveFileColumns.push_back("consumptionRatio");
	}
}

void BerryWorld::printGrid(vector<int> grid, pair<int, int> loc, int facing) {
	for (int y = 0; y < BerryWorld::WorldY; y++) {
		for (int x = 0; x < BerryWorld::WorldX; x++) {
			if ((x == loc.first) && (y == loc.second)) {
				cout << facingDisplay[facing] << " ";
			} else {
				if (getGridValue(grid, { x, y }) == WALL) {
					cout << "X";
				} else {
					cout << getGridValue(grid, { x, y });
				}
				cout << " ";
			}
		}
		cout << "\n";
	}
	cout << "\n";
}

double BerryWorld::testIndividual(shared_ptr<Organism> org, bool analyse, bool show) {
	double score = 0.0;

	vector<int> grid = makeTestGrid();

	// organism starts in the center of the world, facing in a random direction.
	pair<int, int> currentLocation = { Random::getIndex(WorldX), Random::getIndex(WorldY) };  // location of the organism
	//currentLocation = { WorldX/2, WorldY/2 };  // location of the organism
	while (getGridValue(grid, currentLocation) == WALL) {
		currentLocation = {Random::getIndex(WorldX), Random::getIndex(WorldY)};  //
	}
	int facing = Random::getIndex(8);  // direction the agent is facing

////////////////////////// uncomment to test world making and food picking
//	printGrid(grid, currentLocation, facing);
//	for (int x = 1; x < WorldX - 1; x++) {
//		for (int y = 1; y < WorldY - 1; y++) {
//			setGridValue(grid, { x, y }, pickFood(-1));
//		}
//	}
//	cout << endl;
//	printGrid(grid, currentLocation, facing);
//	for (int x = 1; x < WorldX - 1; x++) {
//		for (int y = 1; y < WorldY - 1; y++) {
//			setGridValue(grid, { x, y }, pickFood(-1));
//		}
//	}
//	cout << endl;
//	printGrid(grid, currentLocation, facing);
//
//	exit(1);
////////////////////////// uncomment to test world making and food picking


////////////////////////// uncomment to test food ratios
//	// note, I am not treating 0 diffrently in these tests (i.e. if replacement = "other" (1) 0 will not be replaced by 0 (unlike in berry world)
//	cout << endl;
//
//	vector <int> c;
//	c.resize(foodTypes+1);
//	for (int i = 0; i < 10000; i++) {
//		if (replacement == -1) {
//			setGridValue(grid, { 2, 2 }, pickFood(-1));  // plant a random food
//		} else if (replacement == 1) {
//			setGridValue(grid, { 2, 2 }, pickFood(getGridValue(grid, { 2, 2 })));  // plant a different
//		} else {
//			setGridValue(grid, { 2, 2 }, EMPTY);  // plant a 0
//		}
//
//		c[getGridValue(grid, { 2, 2 })]++;;
//	}
//	cout << endl;
//	cout << endl;
//
//	cout << "food counts:" << endl;
//	for (auto v:c){
//		cout << "  " << v << endl;
//	}
//
//	cout << "foodRatioLookup:" << endl;
//
//	for (auto v:foodRatioLookup){
//		cout << "  " << v << endl;
//	}
//
//	cout << "foodRatioTotal: " << foodRatioTotal << endl;
//	exit(1);
////////////////////////// uncomment to test food ratios


	// set up to track what food is eaten
	int switches = 0;  // number of times organism has switched food source
	int lastFood = -1;  //nothing has been eaten yet!
	int foodHereOnArrival = getGridValue(grid, currentLocation); //value of the food when we got here - needed for replacement method.
	vector<int> eaten;  // stores number of each type of food was eaten in total for this test. [0] stores number of times org attempted to eat on empty location
	eaten.resize(foodTypes + 1);

	if (recordFoodList) {
		org->dataMap.Append("foodList", -2);  // -2 = a world initialization, -1 = did not eat this step
	}

	// set up vars needed to run
	int output1 = 0;  // store outputs from brain
	int output2 = 0;

	int here, leftFront, front, rightFront;  // store grid values relitive to organsism

	int nodesAssignmentCounter;  // this world can has number of brainState inputs set by parameter. This counter is used while assigning inputs

	vector<int> stateCollector;  // used in the phi calculation

	// make sure the brain does not have values from last run
	org->brain->resetBrain();

	for (int t = 0; t < worldUpdates; t++) {  //run agent for "worldUpdates" brain updates

		here = getGridValue(grid, currentLocation);
		front = getGridValue(grid, moveOnGrid(currentLocation, facing));
		leftFront = getGridValue(grid, moveOnGrid(currentLocation, turnLeft(facing)));
		rightFront = getGridValue(grid, moveOnGrid(currentLocation, turnRight(facing)));

		nodesAssignmentCounter = 0;  // get ready to start assigning inputs
		if (senseWalls) {
			if (senseDown) {
				for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at here location
					org->brain->setInput(nodesAssignmentCounter++, (here == i + 1));
				}
			}
			if (senseFront) {
				for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
					org->brain->setInput(nodesAssignmentCounter++, (front == i + 1));
				}
				org->brain->setInput(nodesAssignmentCounter++, (front == WALL));
			}
			if (senseFrontSides) {
				for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
					org->brain->setInput(nodesAssignmentCounter++, (leftFront == i + 1));
					org->brain->setInput(nodesAssignmentCounter++, (rightFront == i + 1));
				}
				org->brain->setInput(nodesAssignmentCounter++, (leftFront == WALL));
				org->brain->setInput(nodesAssignmentCounter++, (rightFront == WALL));
			}
		} else {  // don't sense walls
			if (senseDown) {
				for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at here location
					org->brain->setInput(nodesAssignmentCounter++, (here == i + 1));
				}
			}
			if (senseFront) {
				for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
					org->brain->setInput(nodesAssignmentCounter++, (front == i + 1));
				}
			}
			if (senseFrontSides) {
				for (int i = 0; i < foodTypes; i++) {  // fill first nodes with food values at front location
					org->brain->setInput(nodesAssignmentCounter++, (leftFront == i + 1));
					org->brain->setInput(nodesAssignmentCounter++, (rightFront == i + 1));
				}
			}
		}

		if (show) {
			cout << "\n----------------------------\n";
			cout << "\ngeneration update: " << Global::update << "  world update: " << t << "\n";
			cout << "currentLocation: " << currentLocation.first << "," << currentLocation.second << "  :  " << facing << "\n";
			cout << "inNodes: ";
			for (int i = 0; i < inputNodesCount; i++) {
				cout << org->brain->readInput(i) << " ";
			}
			cout << "\nlast outNodes: ";
			for (int i = 0; i < outputNodesCount; i++) {
				cout << org->brain->readOutput(i) << " ";
			}
			cout << "\n\n  -- brain update --\n\n";
		}

		// inputNodesCount is now set to the first output Brain State Address. we will not move it until the next world update!
		if (clearOutputs) {
			org->brain->resetOutputs();
		}

//		if (analyse) {  // gather some data before and after running update
//			int S = 0;
//			for (int i = 0; i < inputNodesCount; i++)
//				S = (S << 1) + Bit(org->brain->readInput(i));
//			org->brain->update();
//			for (int i = inputNodesCount + outputNodesCount; i < org->brain->nrOfBrainNodes; i++)
//				S = (S << 1) + Bit(org->brain->getState(i));
//			stateCollector.push_back(S);
//		} else {
			org->brain->update();  // just run the update!
//		}

		// set output values
		// output1 has info about the first 2 output bits these [00 eat, 10 left, 01 right, 11 move]
		output1 = Bit(org->brain->readOutput(0)) + (Bit(org->brain->readOutput(1)) << 1);
		// output 2 has info about the 3rd output bit, which either does nothing, or is eat.
		output2 = Bit(org->brain->readOutput(2));

		if (output2 == 1) {  // if org tried to eat
			int foodHere = getGridValue(grid, currentLocation);
			if ((recordFoodList && foodHere != 0) || (recordFoodList && recordFoodListEatEmpty)) {
				org->dataMap.Append("foodList", foodHere);  // record that org ate food (or tried to at any rate)
			}
			eaten[foodHere]++;  // track the number of each berry eaten, including 0s
			if (foodHere != EMPTY) {  // eat food here (if there is food here)
				if (lastFood != -1) {  // if some food has already been eaten
					if (lastFood != foodHere) {  // if this food is different then the last food eaten
						score -= BerryWorld::TSK;  // pay the task switch cost
						switches++;
					}
				}
				lastFood = foodHere;  // remember the last food eaten
				score += foodRewards[foodHere];  // you ate a food... good for you!
				setGridValue(grid, currentLocation, 0);  // clear this location
			}
		} else {
			if (recordFoodList && recordFoodListNoEat) {
				org->dataMap.Append("foodList", -1);  // record that org did not try to eat this time
			}
		}

		if ((output2 == 0) || (allowMoveAndEat == 1)) {  // if we did not eat or we allow moving and eating in the same world update
			switch (output1) {
			case 0:  //nothing
				break;
			case 1:  //turn left
				facing = turnLeft(facing);
				score += rewardForTurn;
				break;
			case 2:  //turn right
				facing = turnRight(facing);
				score += rewardForTurn;
				break;
			case 3:  //move forward
				if (getGridValue(grid, moveOnGrid(currentLocation, facing)) != WALL) {  // if the proposed move is not a wall
					score += rewardForMove;
					if (getGridValue(grid, currentLocation) == EMPTY) {  // if the current location is empty...
						if (replacement == -1 || foodHereOnArrival == EMPTY) { // if replacement = random (-1) or the this location was already empty when org got here...
							setGridValue(grid, currentLocation, pickFood(-1));  // plant a random food
						} else if (replacement == 1 && foodHereOnArrival > EMPTY) { // if replacement = other (1) and there was some food here when org got here...
							setGridValue(grid, currentLocation, pickFood(foodHereOnArrival));  // plant a different food when what was here
						}
						// if replacement = no replacement (0), no replacement/do nothing
					}
					currentLocation = moveOnGrid(currentLocation, facing);
					foodHereOnArrival = getGridValue(grid, currentLocation); //value of the food when we got here - needed for replacement method.

				}
				break;
			}
		}

		if (show) {
			cout << "outNodes: " << Bit(org->brain->readOutput(0)) << " " << Bit(org->brain->readOutput(1)) << " " << Bit(org->brain->readOutput(2)) << "\n";
			cout << "output1: " << output1 << "  output2: " << output2 << "\n";
			cout << "\n  -- world update --\n\n";
			printGrid(grid, currentLocation, facing);
			cout << "last: " << lastFood << " here: " << getGridValue(grid, currentLocation) << "\nloc: " << currentLocation.first << "," << currentLocation.second << "  facing: " << facing << "\n";
			cout << "score: " << score << "\nfood1: " << eaten[1] << "  food2: " << eaten[2] << "  switches: " << switches << "\n";
		}

	}  // end world evaluation loop

	if (score < 0.0) {
		score = 0.0;
	}

	int total_eaten = 0;
	for (int i = 0; i <= foodTypes; i++) {
		if (i != 0) {  // don't count the attempts to eat empty!
			total_eaten += eaten[i];
		}
		string temp_name = "allfood" + to_string(i);  // make food names i.e. food1, food2, etc.
		org->dataMap.Append(temp_name, eaten[i]);
	}
	if (recordConsumptionRatio) {  // consumption ratio displays high value of org favors one food over the other and low values if both are being consumed. works on food[0] and food[1] only
		(eaten[1] > eaten[2]) ? org->dataMap.Append("allconsumptionRatio", (double) eaten[1] / (double) (eaten[2] + 1)) : org->dataMap.Append("allconsumptionRatio", (double) eaten[2] / (double) (eaten[1] + 1));
	}
	org->dataMap.Append("alltotal", total_eaten);  // total food eaten (regardless of type)

	org->dataMap.Append("allswitches", switches);

	return score;
}
