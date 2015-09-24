//
//  RedBlueBerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "RedBlueBerryWorld.h"

/* *** implementation of the World *** */

double RedBlueBerryWorldSettings::TSK;

void RedBlueBerryWorldSettings::initializeParameters(){
	Parameters::setupParameter("RedBlueBerryWorld::TSC", TSK, 1.4, "cost to change food sources");
}



RedBlueBerryWorld::RedBlueBerryWorld() {
	initDataFiles = 0;
}

double RedBlueBerryWorld::testIndividual(Agent *agent, bool analyse) {
	int grid[xDim][yDim];
	int i, j;
	int xp = xDim / 2, yp = yDim / 2, dir = rand() & 7; //xp and yp are x and y positions. agent starts at center of grid and with a random position
	int berry[4] = { 0, 0, 0, 0 }; //the array berry has 4 elements and they are all 0
	int eaten[2] = { 0, 0 }; //the array eaten has 2 elements and they are both 0
	int switched = 0;
	vector<int> stateCollector;
	double score = 0.0;
	for (i = 1; i < xDim - 1; i++)
		for (j = 1; j < yDim - 1; j++) { //as you go across the x and y directions in grid...
			/*if((i&1)==(j&1))
			 grid[i][j]=1;
			 else
			 grid[i][j]=2; //the above lines fill the grid with alternating 1s and 2s
			 berry[grid[i][j]]++; //tally the number of 1s and 2s in the 1st and 2nd positions in berry // AREND WHICH IS BLUE WHICH IS RED
			 */
			grid[i][j] = (rand() & 1) + 1;

		}
	for (i = 0; i < xDim; i++) { //makes horizontal walls
		grid[i][0] = 3;
		grid[i][yDim - 1] = 3;
	}
	for (j = 0; j < yDim; j++) { //makes vertical walls
		grid[0][j] = 3;
		grid[xDim - 1][j] = 3;
	}
	//*
	//I think this bit of code randomizes the world a little bit
	/*for(i=1;i<xDim*yDim*0.5;i++){ //for the first half the spots in the array...
	 int x1,y1,x2,y2;
	 x1=rand()&(xDim-1); //all of these generate random numbers between 0 and 15
	 y1=rand()&(yDim-1);
	 x2=rand()&(xDim-1);
	 y2=rand()&(yDim-1);
	 if((grid[x1][y1]!=3)&&(grid[x2][y2]!=3)){ //if the spaces specified by these two random points on the grid is not a wall...
	 j=grid[x1][y1]; //set var j equal to whatever berry was on the first specified space ---ASK AREND--- why do you need all this stuff with j?
	 grid[x1][y1]=grid[x2][y2]; //set the second specified space to the same berry as the first one (not sure???)
	 grid[x2][y2]=j; //redefine j
	 }

	 }*/
	//*/
	agent->resetBrain();
	int lastFood = -1; //set the last berry eaten to something neither berry is
	for (int t = 0; t < 400; t++) { //do this 1000 times:
		agent->states[0] = grid[xp][yp] & 1; //if the position you're on is an even number, write 0. else write 1
		agent->states[1] = (grid[xp][yp] >> 1) & 1; //write a 1 if the number of your position, bitshifted 1 right
		agent->states[2] = grid[(xp + xm[(dir - 1) & 7]) & (xDim - 1)][(yp + ym[(dir - 1) & 7]) & (yDim - 1)] & 1;
		agent->states[3] = (grid[(xp + xm[(dir - 1) & 7]) & (xDim - 1)][(yp + ym[(dir - 1) & 7]) & (yDim - 1)] >> 1)
				& 1;
		agent->states[4] = grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] & 1;
		agent->states[5] = (grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] >> 1) & 1;
		agent->states[6] = grid[(xp + xm[(dir - 1) & 7]) & (xDim + 1)][(yp + ym[(dir + 1) & 7]) & (yDim - 1)] & 1;
		agent->states[7] = (grid[(xp + xm[(dir - 1) & 7]) & (xDim + 1)][(yp + ym[(dir + 1) & 7]) & (yDim - 1)] >> 1)
				& 1;
		//agent->states[8]=0.0;
		//agent->states[9]=0.0;
		//agent->states[10]=0.0;

		if (analyse) {
			int S = 0;
			for (int i = 0; i < 10; i++)
				S = (S << 1) + agent->Bit(agent->states[i]);
			agent->updateStates();
			for (int i = 10; i < agent->nrOfStates; i++)
				S = (S << 1) + agent->Bit(agent->states[i]);
			stateCollector.push_back(S);
		} else {
			agent->updateStates();
		}
		int output1 = agent->Bit(agent->states[8]) + (agent->Bit(agent->states[9]) << 1);
		int output2 = agent->Bit(agent->states[10]);
		if ((output2 == 1) && (grid[xp][yp] != 0)) {
			berry[grid[xp][yp]]--;
			if (lastFood == -1)
				lastFood = grid[xp][yp];
			else {
				if (lastFood == grid[xp][yp]) {
					score += 1.0;
					eaten[grid[xp][yp] - 1]++;
				} else {
					score = score + 1.0 - RedBlueBerryWorldSettings::TSK;
					lastFood = grid[xp][yp];
					eaten[grid[xp][yp] - 1]++;
					switched++;
				}
			}
			grid[xp][yp] = 0;
			/*
			 if(lastFood==1)
			 grid[xp][yp]=2;
			 else
			 grid[xp][yp]=1;
			 if((rand()&7)==0)
			 */

		}
		switch (output1) {
		case 0: //nothing
			break;
		case 1: //left
			dir = (dir - 1) & 7;
			break;
		case 2: //right
			dir = (dir + 1) & 7;
			break;
		case 3: //forward
			if (grid[(xp + xm[dir]) & (xDim - 1)][(yp + ym[dir]) & (yDim - 1)] != 3) {
				if (grid[xp][yp] == 0) {
					grid[xp][yp] = 1 + (rand() & 1);
					berry[grid[xp][yp]]++;
				}
				xp += xm[dir];
				yp += ym[dir];
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
	if (score < 0.0)
		score = 0.0;
	if (analyse) {
		//printf("%p\n",agent->genome);
		Data::Add(switched, "switches", agent->genome);
		Data::Add(eaten[0], "red", (Genome*) agent->genome);
		Data::Add(eaten[1], "blue", (Genome*) agent->genome);
		Data::Add(eaten[0] + eaten[1], "total", (Genome*) agent->genome);
		Data::Add(score, "score", (Genome*) agent->genome);
		//Data::Add(Analyse::computeAtomicPhi(stateCollector, agent->nrOfStates), "phi", (Genome*) agent->genome);
	}
	return score;
}

void RedBlueBerryWorld::saveData(string mode, string outputDirectory_Name, int update, vector<Genome*> population) {

	if (initDataFiles != 1) {    // open the files and start them.
		initDataFiles = 1;

		string red_File_Name = outputDirectory_Name + "/Reds_dat.py";
		string blue_File_Name = outputDirectory_Name + "/Blues_dat.py";
		string switch_File_Name = outputDirectory_Name + "/Switch_dat.py";

		RED_FILE.open(red_File_Name.c_str());
		RED_FILE << "data = [";

		BLUE_FILE.open(blue_File_Name.c_str());
		BLUE_FILE << "data = [";

		SWITCH_FILE.open(switch_File_Name.c_str());
		SWITCH_FILE << "data = [";
	}
	if (mode == "write") {    // add data to files that are already open
		RED_FILE << "[";
		BLUE_FILE << "[";
		SWITCH_FILE << "[";
		for (size_t i = 0; i < population.size(); i++) {
			RED_FILE << Data::Get("red", (Genome*) population[i]) << ",";
			BLUE_FILE << Data::Get("blue", (Genome*) population[i]) << ",";
			SWITCH_FILE << Data::Get("switches", (Genome*) population[i]) << ",";
		}
		RED_FILE << "],\n";
		BLUE_FILE << "],\n";
		SWITCH_FILE << "],\n";
	}
	if (mode == "close") {    //Close red, blue, and switch files
		RED_FILE << "]\n\n\n";
		RED_FILE.close();

		BLUE_FILE << "]\n\n\n";
		BLUE_FILE.close();

		SWITCH_FILE << "]\n\n\n";
		SWITCH_FILE.close();
	}

}
