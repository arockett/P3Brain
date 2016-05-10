//
//  BerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "BerryWorld.h"

shared_ptr<ParameterLink<double>> BerryWorld::TSKPL = Parameters::register_parameter("WORLD_BERRY-taskSwitchingCost", 1.4, "cost to change food sources");
shared_ptr<ParameterLink<int>> BerryWorld::worldUpdatesPL = Parameters::register_parameter("WORLD_BERRY-worldUpdates", 400, "amount of time an brain is tested");

shared_ptr<ParameterLink<int>> BerryWorld::foodTypesPL = Parameters::register_parameter("WORLD_BERRY-foodTypes", 2, "number of types of food");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood1PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood1", 1.0, "reward for eating a Food1");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood2PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood2", 1.0, "reward for eating a Food2");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood3PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood3", 1.0, "reward for eating a Food3");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood4PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood4", 1.0, "reward for eating a Food4");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood5PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood5", 1.0, "reward for eating a Food5");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood6PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood6", 1.0, "reward for eating a Food6");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood7PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood7", 1.0, "reward for eating a Food7");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForFood8PL = Parameters::register_parameter("WORLD_BERRY-rewardForFood8", 1.0, "reward for eating a Food8");

shared_ptr<ParameterLink<double>> BerryWorld::rewardForTurnPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-rewardForTurn", 0.0, "reward for turning");
shared_ptr<ParameterLink<double>> BerryWorld::rewardForMovePL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-rewardForMove", 0.0, "reward for moving");

shared_ptr<ParameterLink<int>> BerryWorld::ratioFood0PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood0", 0, "Relative likelihood to leave empty space empty");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood1PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood1", 1, "Relative likelihood to place Food1");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood2PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood2", 1, "Relative likelihood to place Food2");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood3PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood3", 1, "Relative likelihood to place Food3");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood4PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood4", 1, "Relative likelihood to place Food4");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood5PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood5", 1, "Relative likelihood to place Food5");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood6PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood6", 1, "Relative likelihood to place Food6");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood7PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood7", 1, "Relative likelihood to place Food7");
shared_ptr<ParameterLink<int>> BerryWorld::ratioFood8PL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-replacementRatioFood8", 1, "Relative likelihood to place Food8");

shared_ptr<ParameterLink<int>> BerryWorld::WorldXPL = Parameters::register_parameter("WORLD_BERRY-worldX", 8, "world X size");
shared_ptr<ParameterLink<int>> BerryWorld::WorldYPL = Parameters::register_parameter("WORLD_BERRY-worldY", 8, "world Y size");
shared_ptr<ParameterLink<bool>> BerryWorld::borderWallsPL = Parameters::register_parameter("WORLD_BERRY-makeBorderWalls", true, "if true world will have a bounding wall");
shared_ptr<ParameterLink<int>> BerryWorld::randomWallsPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-makeRandomWalls", 0, "add this many walls to the world");

shared_ptr<ParameterLink<bool>> BerryWorld::clearOutputsPL = Parameters::register_parameter("WORLD_BERRY-clearOutputs", false, "if true outputs will be cleared on each world update");

shared_ptr<ParameterLink<bool>> BerryWorld::allowMoveAndEatPL = Parameters::register_parameter("WORLD_BERRY_ADVANCED-allowMoveAndEat", false, "if true, the brain can move and eat in the same world update");
shared_ptr<ParameterLink<bool>> BerryWorld::senseDownPL = Parameters::register_parameter("WORLD_BERRY-senseDown", true, "if true, Agent can sense what it's standing on");
shared_ptr<ParameterLink<bool>> BerryWorld::senseFrontPL = Parameters::register_parameter("WORLD_BERRY-senseFront", true, "if true, Agent can sense what's in front of it");
shared_ptr<ParameterLink<bool>> BerryWorld::senseFrontSidesPL = Parameters::register_parameter("WORLD_BERRY-senseFrontSides", false, "if true, Agent can sense what's in front to the left and right of it");
shared_ptr<ParameterLink<bool>> BerryWorld::senseWallsPL = Parameters::register_parameter("WORLD_BERRY-senseWalls", false, "if true, Agent can sense Walls");


shared_ptr<ParameterLink<int>> BerryWorld::replacementPL = Parameters::register_parameter("WORLD_BERRY-replacement", -1, "-1 = random, 0 = nPL = eplacement, 1 = replace other (note: Food0, can be replace by Food0)");

shared_ptr<ParameterLink<bool>> BerryWorld::recordConsumptionRatioPL = Parameters::register_parameter("WORLD_BERRY-recordConsumptionRatio", false, "if true, record greater of red/blue+1 or blue/red+1");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListPL = Parameters::register_parameter("WORLD_BERRY-recordFoodList", true, "if true, record list of food eaten");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListEatEmptyPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListEatEmpty", false, "if true, foodList will include attempts to eat 0");
shared_ptr<ParameterLink<bool>> BerryWorld::recordFoodListNoEatPL = Parameters::register_parameter("WORLD_BERRY-recordFoodListNoEat", false, "if true, if true foodList will include no eat (-1)");


BerryWorld::BerryWorld() {

	worldUpdates = worldUpdatesPL->lookup();

	foodTypes = foodTypesPL->lookup();


	 TSK = TSKPL->lookup();

	 rewardForTurn = rewardForTurnPL->lookup();
	 rewardForMove = rewardForMovePL->lookup();

	 WorldY = WorldYPL->lookup();
	 WorldX = WorldXPL->lookup();
	 borderWalls = borderWallsPL->lookup();
	 randomWalls = randomWallsPL->lookup();

	 allowMoveAndEat = rewardForTurnPL->lookup();

	 senseDown = senseDownPL->lookup();
	 senseFront = senseFrontPL->lookup();
	 senseFrontSides = senseFrontSidesPL->lookup();
	 senseWalls = senseWallsPL->lookup();
	 clearOutputs = clearOutputsPL->lookup();

	 replacement = replacementPL->lookup();

	 recordConsumptionRatio = recordConsumptionRatioPL->lookup();
	 recordFoodList = recordFoodListPL->lookup();
	 recordFoodListEatEmpty = recordFoodListEatEmptyPL->lookup();
	 recordFoodListNoEat = recordFoodListNoEatPL->lookup();

	if (foodTypes < 1 || foodTypes > 8) {
		cout << "In BerryWorld you either have too few or too many foodTypes (must be >0 and <=8)\n\nExiting\n\n";
		exit(1);
	}
	//senseWalls = senseWalls & (borderWalls | (randomWalls > 0));  // if there are no walls, there is no need to sense them!

	outputNodesCount = 3;  // number of brain nodes used for output, 2 for move, 1 for eat

	if (senseWalls) {
		inputNodesCount = senseDown * foodTypes + ((senseFront * foodTypes) + senseWalls) + (2 * ((senseFrontSides * foodTypes) + senseWalls));
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
	foodRatioLookup[0] = ratioFood0PL->lookup();
	foodRatioLookup[1] = ratioFood1PL->lookup();
	foodRatioLookup[2] = ratioFood2PL->lookup();
	foodRatioLookup[3] = ratioFood3PL->lookup();
	foodRatioLookup[4] = ratioFood4PL->lookup();
	foodRatioLookup[5] = ratioFood5PL->lookup();
	foodRatioLookup[6] = ratioFood6PL->lookup();
	foodRatioLookup[7] = ratioFood7PL->lookup();
	foodRatioLookup[8] = ratioFood8PL->lookup();

	foodRatioTotal = 0;
	for (int i = 0; i <= foodTypes; i++) {
		foodRatioTotal += foodRatioLookup[i];
	}

	foodRewards.resize(9);  // stores reward of each type of food
	foodRewards[0] = 0;
	foodRewards[1] = rewardForFood1PL->lookup();
	foodRewards[2] = rewardForFood2PL->lookup();
	foodRewards[3] = rewardForFood3PL->lookup();
	foodRewards[4] = rewardForFood4PL->lookup();
	foodRewards[5] = rewardForFood5PL->lookup();
	foodRewards[6] = rewardForFood6PL->lookup();
	foodRewards[7] = rewardForFood7PL->lookup();
	foodRewards[8] = rewardForFood8PL->lookup();

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
	for (int y = 0; y < WorldY; y++) {
		for (int x = 0; x < WorldX; x++) {
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
						score -= TSK;  // pay the task switch cost
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
