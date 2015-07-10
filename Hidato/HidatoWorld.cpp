//
//  HidatoWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/6/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "HidatoWorld.h"

/* make a puzzle */
void refreshPuzzle(int missing);
bool withinBounds(int X,int Y);

HidatoPuzzle::HidatoPuzzle(vector<int> data){
	xDim=(int)log2((int)data.size());
	yDim=xDim;
	maze.resize(xDim);
	state.resize(xDim);
	int I=0;
	for(int i=0;i<xDim;i++){
		maze[i].resize(yDim);
		state[i].resize(yDim);
		for(int j=0;j<yDim;j++){
			maze[i][j]=data[I]-1;
			I++;
			state[i][j]=FIXED;
		}
	}
}

HidatoPuzzle::~HidatoPuzzle(){
	
}

bool HidatoPuzzle::withinBounds(int X,int Y){
	if((X<0)||(X>=4)||(Y<0)||(Y>=4))
		return false;
	return true;
}

void HidatoPuzzle::show(){
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++)
			printf("%02i ",maze[i][j]);
		printf("\n");
	}
}

void HidatoPuzzle::refreshPuzzle(int missing){
	for(int i=0;i<maze.size();i++){
		for(int j=0;j<maze[i].size();j++){
			state[i][j]=FIXED;
			if(maze[i][j]==0){
				startX=i;
				startY=j;
			}
		}
	}
	for(int i=0;i<missing;i++){
		int x,y;
		do{
			x=rand()%maze.size();
			y=rand()%maze[0].size();
		} while((maze[x][y]==0)||(maze[x][y]==15)||(state[x][y]==EMPTY));
		state[x][y]=EMPTY;
	}
}


/* the actual world implementation */

int HidatoWorld::missing=4;

HidatoWorld::HidatoWorld(){
	FILE *F=fopen("HidatoGrids4x4.txt","r+t");
	puzzles.resize(1000);
	for(int i=0;i<1000;i++){
		vector<int> data;
		int I;
		for(int j=0;j<16;j++){
			fscanf(F,"%i",&I);
			data.push_back(I);
		}
		HidatoPuzzle *H=new HidatoPuzzle(data);
		puzzles[i]=H;
	}
	fclose(F);
	printf("All Puzzles loaded\n");
}

HidatoWorld::~HidatoWorld(){
	for(HidatoPuzzle* puzzle:puzzles)
		delete puzzle;
}

vector<double> HidatoWorld::evaluateFitness(vector<Agent*> agents,bool analyse){
	vector<double> W;
	for(int i=0;i<agents.size();i++){
		double w=0.0;
		for(int r=0;r<repeats;r++){
			w+=testIndividual(agents[i],analyse);
		}
		W.push_back(w/(double)repeats);
		//		W.push_back(pow(w,1.0/(double)repeats));
	}
	return W;
}

double HidatoWorld::testIndividual(Agent *agent,bool analyse){
	HidatoPuzzle *puzzle=puzzles[rand()%puzzles.size()];
	puzzle->refreshPuzzle(missing);
	int xPos=puzzle->startX;
	int yPos=puzzle->startY;
	int dir=rand()&3;
	agent->resetBrain();
	vector<int> steps;
	steps.clear();
	steps.push_back(0);
	for(int t=0;t<512;t++){
		//load the top sensor
		agent->states[0]=1.0*(double)(puzzle->maze[xPos][yPos]&1);
		agent->states[1]=1.0*(double)((puzzle->maze[xPos][yPos]>>1)&1);
		agent->states[2]=1.0*(double)((puzzle->maze[xPos][yPos]>>2)&1);
		agent->states[3]=1.0*(double)((puzzle->maze[xPos][yPos]>>3)&1);
		if(puzzle->withinBounds(xPos+xm[dir], yPos+ym[dir])){
			agent->states[4]=1.0*(double)(puzzle->maze[xPos+xm[dir]][yPos+ym[dir]]&1);
			agent->states[5]=1.0*(double)((puzzle->maze[xPos+xm[dir]][yPos+ym[dir]]>>1)&1);
			agent->states[6]=1.0*(double)((puzzle->maze[xPos+xm[dir]][yPos+ym[dir]]>>2)&1);
			agent->states[7]=1.0*(double)((puzzle->maze[xPos+xm[dir]][yPos+ym[dir]]>>3)&1);
		} else {
			agent->states[4]=0.0;
			agent->states[5]=0.0;
			agent->states[6]=0.0;
			agent->states[7]=0.0;
		}
		agent->updateStates();
		int action=agent->Bit(agent->states[8])+(agent->Bit(agent->states[9])<<1);
		switch(action){
			case 0:
				break;
			case 1:
				dir=(dir+1)&3;
				break;
			case 2:
				dir=(dir-1)&3;
				break;
			case 3:
				if(puzzle->withinBounds(xPos+xm[dir], yPos+ym[dir])){
					xPos+=xm[dir];
					yPos+=ym[dir];
					steps.push_back(puzzle->maze[xPos][yPos]);
				}
				break;
		}
	}
	double fitness=1.0;
	for(int i=0;i<16;i++){
		if(steps[i%steps.size()]==i)
			fitness*=2.0;
	}
	return fitness;
}
