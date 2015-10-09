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

double& RedBlueBerryWorld::TSK = Parameters::register_parameter("taskSwitchingCost", 1.4, "cost to change food sources",
		"WORLD - RED BLUE");

RedBlueBerryWorld::RedBlueBerryWorld() {

}

int loopMod(int x, int y) {
	if (x < 0) {
		x = x % y + y;
	}
	return x % y;
}

double RedBlueBerryWorld::testIndividual(Agent *agent, bool analyse) {
	int grid[xDim][yDim];
	int i, j;
//WAS int xp = xDim / 2, yp = yDim / 2, dir = rand() & 7; //xp and yp are x and y positions. agent starts at center of grid and with a random rotation
	int xp = xDim / 2, yp = yDim / 2, dir = Random::getInt(7); //xp and yp are x and y positions. agent starts at center of grid and with a random rotation
//int berry[4] = { 0, 0, 0, 0 }; //the array berry has 4 elements and they are all 0
	int eaten[2] = { 0, 0 }; //the array eaten has 2 elements and they are both 0
	int switched = 0;
	vector<int> stateCollector;
	double score = 0.0;
	int lastFood = -1; //nothing has been eaten yet!

	for (i = 1; i < xDim - 1; i++)
		for (j = 1; j < yDim - 1; j++) { //as you go across the x and y directions in grid...
			grid[i][j] = Random::getInt(1) + 1; // plase red or blue food
		}
//	for (i = 0; i < xDim; i++) { //makes horizontal walls
//		grid[i][0] = 3;
//		grid[i][yDim - 1] = 3;
//	}
//	for (j = 0; j < yDim; j++) { //makes vertical walls
//		grid[0][j] = 3;
//		grid[xDim - 1][j] = 3;
//	}

	agent->resetBrain();

	for (int t = 0; t < 400; t++) { //run agent for t brain updates

//		agent->states[0] = grid[xp][yp] & 1; //if the position you're on is an even number, write 0. else write 1
//		agent->states[1] = (grid[xp][yp] >> 1) & 1; //write a 1 if the number of your position, bitshifted 1 right
//		agent->states[2] = grid[(xp + xm[(dir - 1) & 7]) & (xDim - 1)][(yp + ym[(dir - 1) & 7]) & (yDim - 1)] & 1;
//		agent->states[3] = (grid[(xp + xm[(dir - 1) & 7]) & (xDim - 1)][(yp + ym[(dir - 1) & 7]) & (yDim - 1)] >> 1)
//				& 1;
//		agent->states[4] = grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] & 1;
//		agent->states[5] = (grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] >> 1) & 1;
//		agent->states[6] = grid[(xp + xm[(dir + 1) & 7]) & (xDim - 1)][(yp + ym[(dir + 1) & 7]) & (yDim - 1)] & 1;
//		agent->states[7] = (grid[(xp + xm[(dir + 1) & 7]) & (xDim - 1)][(yp + ym[(dir + 1) & 7]) & (yDim - 1)] >> 1)
//				& 1;
//
		int testStates[8];

		testStates[0] = grid[xp][yp] & 1; //if the position you're on is an even number, write 0. else write 1
		testStates[1] = (grid[xp][yp] >> 1) & 1; //write a 1 if the number of your position, bitshifted 1 right
		testStates[2] = grid[(xp + xm[(dir - 1) & 7]) & (xDim - 1)][(yp + ym[(dir - 1) & 7]) & (yDim - 1)] & 1;
		testStates[3] = (grid[(xp + xm[(dir - 1) & 7]) & (xDim - 1)][(yp + ym[(dir - 1) & 7]) & (yDim - 1)] >> 1) & 1;
		testStates[4] = grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] & 1;
		testStates[5] = (grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] >> 1) & 1;
		testStates[6] = grid[(xp + xm[(dir + 1) & 7]) & (xDim - 1)][(yp + ym[(dir + 1) & 7]) & (yDim - 1)] & 1;
		testStates[7] = (grid[(xp + xm[(dir + 1) & 7]) & (xDim - 1)][(yp + ym[(dir + 1) & 7]) & (yDim - 1)] >> 1) & 1;

//		agent->states[0] = grid[xp][yp] == 1; //1 if you are standing on a red
//		agent->states[1] = grid[xp][yp] == 2; //1 if you are standing on a blue
//		agent->states[2] = grid[(xp + xm[(dir - 1) & 7]) % (xDim)][(yp + ym[(dir - 1) & 7]) % (yDim)] == 1; // front and left is red
//		agent->states[3] = grid[(xp + xm[(dir - 1) & 7]) % (xDim)][(yp + ym[(dir - 1) & 7]) % (yDim)] == 2; // front and left is blue
//		agent->states[4] = grid[(xp + xm[dir]) % (xDim)][(yp + ym[dir]) % (yDim)] == 1; // front is red
//		agent->states[5] = grid[(xp + xm[dir]) % (xDim)][(yp + ym[dir]) % (yDim)] == 2; // front is blue
//		agent->states[6] = grid[(xp + xm[(dir + 1) & 7]) % (xDim)][(yp + ym[(dir + 1) & 7]) % (yDim)] == 1; // front and right is red
//		agent->states[7] = grid[(xp + xm[(dir + 1) & 7]) % (xDim)][(yp + ym[(dir + 1) & 7]) % (yDim)] == 2; // front and right is blue

		cout << "Setting States\n";
		agent->states[0] = grid[xp][yp] == 1 || grid[xp][yp] == 3; //1 if you are standing on a red
		agent->states[1] = grid[xp][yp] == 2 || grid[xp][yp] == 3; //1 if you are standing on a blue
		agent->states[2] = grid[loopMod(  (xp + xm[(dir - 1) & 7]) , (xDim)  )][loopMod(  (yp + ym[(dir - 1) & 7]) , (yDim))] == 1
				|| grid[loopMod(  (xp + xm[(dir - 1) & 7]) , (xDim)  )][loopMod(  (yp + ym[(dir - 1) & 7]) , (yDim))] == 3; // front and left is red
		agent->states[3] = grid[loopMod( (xp + xm[(dir - 1) & 7]) , (xDim))][loopMod((yp + ym[(dir - 1) & 7]) , (yDim))] == 2
				|| grid[loopMod( (xp + xm[(dir - 1) & 7]) , (xDim))][loopMod((yp + ym[(dir - 1) & 7]) , (yDim))]  == 3; // front and left is blue
		agent->states[4] = grid[loopMod((xp + xm[dir]) , (xDim))][loopMod((yp + ym[dir]) , (yDim))] == 1
				|| grid[loopMod((xp + xm[dir]) , (xDim))][loopMod((yp + ym[dir]) , (yDim))] == 3; // front is red
		agent->states[5] = grid[loopMod((xp + xm[dir]) , (xDim))][loopMod((yp + ym[dir]) , (yDim))] == 2
				|| grid[loopMod((xp + xm[dir]) , (xDim))][loopMod((yp + ym[dir]) , (yDim))] == 3; // front is blue
		agent->states[6] = grid[loopMod((xp + xm[(dir + 1) & 7]) ,(xDim))][loopMod((yp + ym[(dir + 1) & 7]) , (yDim))] == 1
				|| grid[loopMod((xp + xm[(dir + 1) & 7]) ,(xDim))][loopMod((yp + ym[(dir + 1) & 7]) , (yDim))] == 3; // front and right is red
		agent->states[7] = grid[loopMod((xp + xm[(dir + 1) & 7]) , (xDim))][loopMod((yp + ym[(dir + 1) & 7]) , (yDim))] == 2
				|| grid[loopMod((xp + xm[(dir + 1) & 7]) , (xDim))][loopMod((yp + ym[(dir + 1) & 7]) , (yDim))] == 3; // front and right is blue
		cout << "Done Setting States\n";

		int checkStates = 0;
		for (int i = 0; i < 8; i++) {
			if (testStates[i] != agent->states[i]) {
				checkStates = 1;
			}
		}
		if (checkStates) {
			for (int i = 0; i < 8; i++) {
				cout << testStates[i] << " ";
			}
			cout << "\n";
			for (int i = 0; i < 8; i++) {
				cout << agent->states[i] << " ";
			}
			cout << "\n";
			for (int i = 0; i < 8; i++) {
				if (testStates[i] != agent->states[i]) {
					cout << "* ";
				} else {
					cout << "  ";
				}
			}

			cout << "\n---\n";
		}

		//agent->states[8]=0.0;
		//agent->states[9]=0.0;
		//agent->states[10]=0.0;
		if (analyse) {
			int S = 0;
			for (int i = 0; i < 10; i++)
				S = (S << 1) + agent->Bit(agent->states[i]);
			agent->updateStates();
			for (int i = 10; i < agent->nrOfBrainStates; i++)
				S = (S << 1) + agent->Bit(agent->states[i]);
			stateCollector.push_back(S);
		} else {
			agent->updateStates();
		}
		int output1 = agent->Bit(agent->states[8]) + (agent->Bit(agent->states[9]) << 1);
		int output2 = agent->Bit(agent->states[10]);
		if ((output2 == 1) && (grid[xp][yp] != 0)) {
			//berry[grid[xp][yp]]--;
			if (lastFood != -1) {
				if (lastFood != grid[xp][yp]) {
					score -= RedBlueBerryWorld::TSK;
					switched++;
				}
			}
			score += 1.0;
			lastFood = grid[xp][yp];
			eaten[grid[xp][yp] - 1]++;
			grid[xp][yp] = 0;
		}
		cout << "HERE111\n";
		switch (output1) {
		case 0: //nothing
			cout << "HERE C0\n";
			break;
		case 1: //left
			cout << "HERE C1\n";
			dir = (dir - 1) & 7;
			break;
		case 2: //right
			cout << "HERE C2\n";
			dir = (dir + 1) & 7;
			break;
		case 3: //forward
			cout << "HERE C3\n";

			if (grid[loopMod((xp + xm[dir]) , (xDim))][loopMod((yp + ym[dir]) , (yDim))] != 3) {
				if (grid[xp][yp] == 0) {
					grid[xp][yp] = Random::getInt(1) + 1;
					//berry[grid[xp][yp]]++;
				}
				cout << "X:" << xp << " d:" << dir << " m:" << xm[dir] << " - " << (xp + xm[dir]) << " "
						<< loopMod((xp + xm[dir]) , xDim) << "\n";
				cout << "Y:" << yp << " d:" << dir << " m:" << ym[dir] << " - " << (yp + ym[dir]) << " "
						<< loopMod((yp + ym[dir]) , yDim) << "\n";
				xp = loopMod((xp + xm[dir]) , xDim);
				yp = loopMod((yp + ym[dir]) , yDim);
				cout << "PAST\n";

//				int vall = 6;
//
//				for (int i = -10; i <= 10; i++) {
//					cout << i << " %% " << vall << " = " << loopMod(i, vall) << "\n";
//				}
			}
			break;
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
	cout << "returning SCORE\n";
	return score;
}
