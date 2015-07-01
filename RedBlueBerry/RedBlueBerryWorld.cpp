//
//  RedBlueBerryWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 6/15/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "RedBlueBerryWorld.h"

/* *** implementation of the World *** */

double RedBlueBerryWorld::TSK=0.0;

double RedBlueBerryWorld::testIndividual(Agent *agent,bool analyse){
	int grid[xDim][yDim];
	int i,j;
	int xp=xDim/2,yp=yDim/2,dir=rand()&7;
	int berry[4]={0,0,0,0};
	int eaten[2]={0,0};
	int switched=0;
	vector<int> stateCollector;
	double fitness=0.0;
	for(i=1;i<xDim-1;i++)
		for(j=1;j<yDim-1;j++){
			if((i&1)==(j&1))
				grid[i][j]=1;
			else
				grid[i][j]=2;
			berry[grid[i][j]]++;
		}
	for(i=0;i<xDim;i++){
		grid[i][0]=3;
		grid[i][yDim-1]=3;
	}
	for(j=0;j<yDim;j++){
		grid[0][j]=3;
		grid[xDim-1][j]=3;
	}
	//*
	for(i=1;i<xDim*yDim*0.5;i++){
		int x1,y1,x2,y2;
		x1=rand()&(xDim-1);
		y1=rand()&(yDim-1);
		x2=rand()&(xDim-1);
		y2=rand()&(yDim-1);
		if((grid[x1][y1]!=3)&&(grid[x2][y2]!=3)){
			j=grid[x1][y1];
			grid[x1][y1]=grid[x2][y2];
			grid[x2][y2]=j;
		}
	}
	//*/
	agent->resetBrain();
	int lastFood=-1;
	for(int t=0;t<1000;t++){
		agent->states[0]=grid[xp][yp]&1;
		agent->states[1]=(grid[xp][yp]>>1)&1;
		agent->states[2]=grid[(xp+xm[(dir-1)&7])&(xDim-1)][(yp+ym[(dir-1)&7])&(yDim-1)]&1;
		agent->states[3]=(grid[(xp+xm[(dir-1)&7])&(xDim-1)][(yp+ym[(dir-1)&7])&(yDim-1)]>>1)&1;
		agent->states[4]=grid[(xp+xm[dir])&(xDim-1)][(yp+ym[dir])&(yDim-1)]&1;
		agent->states[5]=(grid[(xp+xm[dir])&(xDim-1)][(yp+ym[dir])&(yDim-1)]>>1)&1;
		agent->states[6]=grid[(xp+xm[(dir-1)&7])&(xDim+1)][(yp+ym[(dir+1)&7])&(yDim-1)]&1;
		agent->states[7]=(grid[(xp+xm[(dir-1)&7])&(xDim+1)][(yp+ym[(dir+1)&7])&(yDim-1)]>>1)&1;
		agent->states[8]=0.0;
		agent->states[9]=0.0;
		agent->states[10]=0.0;
		if(analyse){
			int S=0;
			for(int i=0;i<10;i++)
				S=(S<<1)+agent->Bit(agent->states[i]);
			agent->updateStates();
			for(int i=10;i<agent->nrOfStates;i++)
				S=(S<<1)+agent->Bit(agent->states[i]);
			stateCollector.push_back(S);
		} else {
			agent->updateStates();
		}
		int output1=agent->Bit(agent->states[8])+(agent->Bit(agent->states[9])<<1);
		int output2=agent->Bit(agent->states[10]);
		if((output2==1)&&(grid[xp][yp]!=0)){
			berry[grid[xp][yp]]--;
			if(lastFood==-1)
				lastFood=grid[xp][yp];
			else{
				if(lastFood==grid[xp][yp]){
					fitness+=1.0;
					eaten[grid[xp][yp]-1]++;
				}
				else{
					fitness=fitness+1.0-TSK;
					lastFood=grid[xp][yp];
					eaten[grid[xp][yp]-1]++;
					switched++;
				}
			}
			grid[xp][yp]=0;
			/*
			 if(lastFood==1)
			 grid[xp][yp]=2;
			 else
			 grid[xp][yp]=1;
			 if((rand()&7)==0)
			 */
			
		}
		switch(output1){
			case 0: //nothing
				break;
			case 1: //left
				dir=(dir-1)&7;
				break;
			case 2: //right
				dir=(dir+1)&7;
				break;
			case 3: //forward
				if(grid[(xp+xm[dir])&(xDim-1)][(yp+ym[dir])&(yDim-1)]!=3){
					if(grid[xp][yp]==0){
						grid[xp][yp]=1+(rand()&1);
						berry[grid[xp][yp]]++;
					}
					xp+=xm[dir];
					yp+=ym[dir];
				}
				break;
		}
//		Data::Add(xp, "x"+to_string(t), agent->genome);
//		Data::Add(yp, "y"+to_string(t), agent->genome);
		/*
		for(int x=0;x<xDim;x++){
			for(int y=0;y<yDim;y++){
				if((x==xp)&&(y==yp))
					printf("X");
				else
					printf("%i",grid[x][y]);
			}
			printf("\n");
		}
		printf("%f\n",fitness);
		 */
	}
	if(fitness<0.0)
		fitness=1.0;
	if(analyse){
		//printf("%p\n",agent->genome);
		Data::Add(switched, "switches", agent->genome);
		Data::Add(eaten[0], "red", (Genome*)agent->genome);
		Data::Add(eaten[1], "blue", (Genome*)agent->genome);
		Data::Add(eaten[0]+eaten[1], "total", (Genome*)agent->genome);
		Data::Add(fitness,"W",(Genome*)agent->genome);
		Data::Add(Analyse::computeAtomicPhi(stateCollector, agent->nrOfStates),"phi",(Genome*)agent->genome);
	}
	//printf("P %p %i %s\n",(Genome*)agent->genome,eaten[0],Data::Get("red",(Genome*)agent->genome).c_str());

	return fitness;
}
