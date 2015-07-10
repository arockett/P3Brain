//
//  fbLearnWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/10/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "fbLearnWorld.h"

#define xDim 64
#define yDim 64

#define empty 0
#define wall 1

const int xm4[4]={0,1,0,-1};
const int ym4[4]={-1,0,1,0};

struct Pos{
	int x,y;
	static Pos newPos(int X,int Y);
};

Pos Pos::newPos(int X,int Y){
	Pos P;
	P.x=X;
	P.y=Y;
	return P;
}

fbLearnWorld::fbLearnWorld(){
	area.resize(xDim);
	distMap.resize(xDim);
	dirMap.resize(xDim);
	for(int i=0;i<xDim;i++){
		area[i].resize(yDim);
		distMap[i].resize(yDim);
		dirMap[i].resize(yDim);
	}
}

int fbLearnWorld::makeNumberNotBorder(int range){
	int n;
	do{
		n=rand()%range;
	}while((n==0)||(n==range-1));
	return n;
}

void fbLearnWorld::makeMap(){
	vector<Pos> current,next;
	int cDist=1;
	int i,j,k;
	do{
		cDist=1;
		for(i=0;i<xDim;i++)
			for(j=0;j<yDim;j++){
				distMap[i][j]=-1;
				area[i][j]=-1;
				dirMap[i][j]=-1;
				if((i==0)||(i==(xDim-1))||(j==0)||(j==(yDim-1))||((rand()&7)==0))
					area[i][j]=0;
			}
		targetX=makeNumberNotBorder(xDim);
		targetY=makeNumberNotBorder(yDim);
		distMap[targetX][targetY]=0;
		current.clear();
		next.clear();
		current.push_back(Pos::newPos(targetX,targetY));
		while(current.size()!=0){
			for(i=0;i<current.size();i++){
				for(j=0;j<4;j++)
					if((area[current[i].x+xm4[j]][current[i].y+ym4[j]]!=0)
					   &&(distMap[current[i].x+xm4[j]][current[i].y+ym4[j]]==-1)){
						distMap[current[i].x+xm4[j]][current[i].y+ym4[j]]=cDist;
						next.push_back(Pos::newPos(current[i].x+xm4[j],current[i].y+ym4[j]));
					}
			}
			current.clear();
			current=next;
			next.clear();
			cDist++;
		}
	}while(cDist<xDim/2);
	do{
		startX=rand()&(xDim-1);
		startY=rand()&(yDim-1);
	} while(distMap[startX][startY]!=xDim/2);
	for(i=1;i<xDim-1;i++)
		for(j=1;j<yDim-1;j++)
			if(distMap[i][j]>0){
				int mD=distMap[i][j];
				dirMap[i][j]=0;
				for(k=0;k<4;k++)
					if(distMap[i+xm4[k]][j+ym4[k]]!=-1){
						if(distMap[i+xm4[k]][j+ym4[k]]<mD){
							mD=distMap[i+xm4[k]][j+ym4[k]];
							dirMap[i][j]=k;
						}
					}
			}
	dirMap[targetX][targetY]=rand()&3;
}

vector<double> fbLearnWorld::evaluateFitness(vector<Agent*> agents,bool analyse){
	vector<double> allW;
	makeMap();
	for(int i=0;i<agents.size();i++){
		if(analyse){
			//analysis mode!
			FILE *F=fopen(Data::getDefaultString("phiFileName").c_str(),"w+t");
			fprintf(F,"map,goal0,goal1,goal2,goal3,goal4,goal5,goal6,goal7,goal8,goal9\n");
			for(int fbOn=0;fbOn<2;fbOn++){
				if(fbOn==0)
					FeedbackGate::feedbackON=true;
				else
					FeedbackGate::feedbackON=false;
				phiDataCollector.clear();
				for(int k=0;k<100;k++){
					//total 100 tests per Org.
					for(int j=0;j<24;j++){
						//per map
						currentMapID=j;
						testIndividual(agents[i], analyse);
					}
				}
				if(fbOn==0){
					for(int j=0;j<24;j++){
						fprintf(F,"fbON_%i",j);
						for(int g=0;g<10;g++){
							if((phiDataCollector[j].find(g)!=phiDataCollector[j].end())
							   &&(phiDataCollector[j][g].size()>10000)){
								//has reached the goal g times at least
								//and has enough data in it
								double phi=Analyse::computeAtomicPhi(phiDataCollector[j][g], agents[i]->nrOfStates);
								printf("FB:%i Map: %i Goal: %i - phi=%f\n",fbOn,j,g,phi);
								fprintf(F,",%f",phi);
							} else {
								printf("FB:%i Map: %i Goal: %i - phi=Nan!\n",fbOn,j,g);
								fprintf(F,",nan");
							}
						}
						fprintf(F,"\n");
					}
				}
				if(fbOn==0)
					fprintf(F,"lifetimeFbON");
				else
					fprintf(F,"lifetimeFbOFF");
				
				for(int j=0;j<24;j++){
					vector<int> allData;
					for(int g=0;g<10;g++){
						if(phiDataCollector[j].find(g)!=phiDataCollector[j].end())
							allData.insert(allData.end(), phiDataCollector[j][g].begin(),phiDataCollector[j][g].end());
					}
					if(allData.size()>10000){
						double phi=Analyse::computeAtomicPhi(allData, agents[i]->nrOfStates);
						fprintf(F,",%f",phi);
						printf("fb:%i lifetime phi map %i :%f\n",fbOn,j,phi);
					} else {
						fprintf(F,",nan");
						printf("fb:%i lifetime phi map %i :nan\n",fbOn,j);
					}
				}
				fprintf(F,"\n");
			}
			fclose(F);
		} else {
			//evolve mode
			double W=0.0;
			//iterate over all 24 worlds!
			for(int j=0;j<24;j++){
				currentMapID=j;
				W+=testIndividual(agents[i], analyse);
			}
			allW.push_back(W/24.0);
		}
	}
	return allW;
}


double fbLearnWorld::testIndividual(Agent *agent,bool analyse){
	int xPos,yPos,dir;
	int t;
	int goalsReached=0;
	string timeDelta="";
	int steps=4*(xDim+yDim);
	int i;
	
	vector<Pos> positions;
	
	xPos=startX;
	yPos=startY;
	dir=rand()&3;
	double fitness=0.0;
	for(t=0;t<steps;t++){
		//if(analyse)
		//	positions.push_back(Pos::newPos(xPos,yPos));
		for(i=0;i<4;i++)
			agent->states[i]=0.0;
		agent->states[(dirMap[xPos][yPos]-dir)&3]=1.0;
		agent->updateStates();
		if(analyse){
			phiDataCollector[currentMapID][goalsReached].push_back(agent->IntFromAllStates());
		}
		int action=(agent->Bit(agent->states[agent->nrOfStates-2])<<1)+agent->Bit(agent->states[agent->nrOfStates-1]);
		switch(mapping[currentMapID][action]){
			case 0:break;
			case 1:dir=(dir-1)&3;break;
			case 2:dir=(dir+1)&3;break;
			case 3:if(distMap[xPos+xm4[dir]][yPos+ym4[dir]]!=-1){
				xPos=xPos+xm4[dir];
				yPos=yPos+ym4[dir];
			}
			break;
		}
		fitness+=1.0/pow((double)(distMap[xPos][yPos]+1),2.0);
		if (distMap[xPos][yPos] == 0)
		{
			fitness+=1000.0;
			goalsReached++;
			do{
				xPos=rand()%xDim;
				yPos=rand()%yDim;
			}while (distMap[xPos][yPos] != yDim/2);
			dir=rand()&3;
		}
	}
	if(FeedbackGate::feedbackON==true)
		Data::Add(goalsReached, "goalReachedFBON"+to_string(currentMapID), (Genome*)agent->genome);
	else
		Data::Add(goalsReached, "goalReachedFBOFF"+to_string(currentMapID), (Genome*)agent->genome);
	if(analyse){
		/*
		string S="";
		string target="";
		int maxDist=0;
		//find max Dist
		//		for(i=0;i<xDim;i++)
		//			for(int j=0;j<yDim;j++){
		//				if(distMap[i][j]>maxDist)
		//					maxDist=distMap[i][j];
		//			}
		//		for(i=0;i<xDim;i++)
		//			for(int j=0;j<yDim;j++){
		//				if(distMap[i][j]!=-1){
		//					int d=(int)((double)distMap[i][j]/(double)maxDist*256.0);
		//					for(int ox=0;ox<3;ox++)
		//						for(int oy=0;oy<3;oy++)
		//							S.append(to_string(ox+(i*3))+","+to_string(oy+(j*3))+","+to_string(256-d)+","+to_string(d)+","+to_string(128)+"\n");
		//				}
		//			}
		target.append(to_string(targetX)+","+to_string(targetY)+"\n");
		agent->dataCollector->addData(target, "targetPos");
		
		for(i=0;i<positions.size();i++)
			S.append(to_string(positions[i].x)+","+to_string(positions[i].y)+"\n");
		//		printf("%s\n",S.c_str());
		agent->dataCollector->addData( S,"path");
		
		string stateList="";
		for(i=0; i<agent->stateHistory.size();i++)
			stateList.append(to_string(i) + ',' + to_string(agent->stateHistory[i]) + "\n");
		agent->dataCollector->addData(stateList, "states");
		 */
	}
	return fitness;
}
