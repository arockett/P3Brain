//
//  BlinkyWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/2/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "BlinkyWorld.h"

bool BlinkyWorld::doHammer=false;
bool BlinkyWorld::constantHome=true;
bool BlinkyWorld::constantFood=true;
int BlinkyWorld::defaultDist=0;
double BlinkyWorld::hammerP=0.04;
double BlinkyWorld::externalCognitiveNoise=0.0;

const double cPI=3.14159265359;

bool inside(int X,int Y,int tx, int ty,int d){
	if((X>=tx-d)&&(X<=tx+d)&&(Y>=ty-d)&&(Y<=ty+d))
		return true;
	return false;
}

double BlinkyWorld::testIndividual(Agent *agent, bool analyse){
	int i;
	int endTime=9900+(rand()&255);
	int dX,dY,pX,pY;
	int foodDist;
	int agentState=0;
	int stepsSinceFoodIntake=0;
	double angle[8]={ 0.0, 45.0, 90.0,135.0,180.0,225.0,270.0,315.0};
	double foodCollected=0.0;
	double alpha;
	bool gotHammered,gotBlessed,gotTeased;
	int totalNrReturns=0;
	int timeOut=0,timeIn=0;
	int backtrackHome=0,backtrackFood=0;
	bool lastVisitHome=true;
	bool outside=false;
	int dir,xPos,yPos;
	double fitness=0.0;
	vector<int> timeOutCollector,timeInCollector;
	timeOutCollector.clear();
	timeInCollector.clear();
	int food=0;
	//setup start pos and direction
	if(defaultDist==0)
		foodDist=2;
	else
		foodDist=defaultDist;
	dir=0;//rand()&7;
	xPos=xDim/2;
	yPos=yDim/2;
	alpha=360.0*(double)rand()/(double)RAND_MAX;
	dX=(int)(sin(alpha/180.0*3.1415)*(double)foodDist);
	dY=(int)(cos(alpha/180.0*3.1415)*(double)foodDist);
	pX=(xDim/2)+dX;
	pY=(yDim/2)+dY;
	//need to initialize fitness
	agent->resetBrain();
	//the core loop
	for(int t=0;t<endTime;t++){
		gotHammered=false;
		gotBlessed=false;
		gotTeased=false;
		if(doHammer)
			if((double)rand()/(double)RAND_MAX<hammerP){
				dir=rand()&7;
				gotHammered=true;
			}
		
		agent->states[0]=0.0;
		agent->states[1]=0.0;
		agent->states[2]=0.0;
		agent->states[3]=0.0;
		if(inside(xPos+xm[dir],yPos+ym[dir],xDim/2,yDim/2,1))
			agent->states[3]=1.0;
		bool neither=true;
		if(inside(xPos,yPos,xDim/2,yDim/2,1)){ //inside Home
			agent->states[1]=1.0;
			//check for home backtrack
			neither=false;
			if(outside==true){
				outside=false;
				if(lastVisitHome==true){
					backtrackHome++;
				}
			}
			lastVisitHome=true;
		}
		if(inside(xPos,yPos,pX,pY,1)){ //inside food
			agent->states[0]=1.0;
			//check for food backtrack
			neither=false;
			if(outside==true){
				outside=false;
				if(lastVisitHome==false){
					backtrackFood++;
				}
			}
			lastVisitHome=false;
		}
		if(neither)
			outside=true;

		if(inside(xPos+xm[dir],yPos+ym[dir],pX,pY,1)){ //inside food
			agent->states[2]=1.0;
		}
		for(i=0;i<8;i++){
			agent->states[4+i]=0.0;
			agent->states[12+i]=0.0;
		}
		
		//home mapped into the retina:
		double dx=(xDim/2)-xPos;
		double dy=(yDim/2)-yPos;
		double d=sqrt((dx*dx)+(dy*dy));
		double Ux,Uy,Vx,Vy;
		Ux=dx/d;
		Uy=dy/d;
		Vx=cos(angle[dir]*(cPI/180.0));
		Vy=sin(angle[dir]*(cPI/180.0));
		double theAngle = atan2(((Ux*Vy)-(Uy*Vx)), ((Ux*Vx)+(Uy*Vy)))*180.0/cPI;
		if(isnan(theAngle))
			theAngle=0.0;
		if((constantHome)||((float)rand()/(float)RAND_MAX<1.0/d)){
			//if(d<5.0)
			agent->states[4+(int)((theAngle+180.0)/360.0*8.0)]=1.0;
			gotBlessed=true;
		}
		//food mapped into the second retina
		dx=pX-xPos;
		dy=pY-yPos;
		d=sqrt((dx*dx)+(dy*dy));
		Ux=dx/d;
		Uy=dy/d;
		Vx=cos(angle[dir]*(cPI/180.0));
		Vy=sin(angle[dir]*(cPI/180.0));
		theAngle = atan2(((Ux*Vy)-(Uy*Vx)), ((Ux*Vx)+(Uy*Vy)))*180.0/cPI;
		if(isnan(theAngle))
			theAngle=0.0;
		if((constantFood)||((float)rand()/(float)RAND_MAX<1.0/d)){
			agent->states[12+(int)((theAngle+180.0)/360.0*8.0)]=1.0;
			gotTeased=true;
		}
		//apply external cognitive noise
		if((double)rand()/(double)RAND_MAX<externalCognitiveNoise){
			int siteToFlip=rand()%agent->nrOfStates;
			agent->states[siteToFlip]=1.0-agent->states[siteToFlip];
		}
		agent->updateStates();
		int action= (agent->Bit(agent->states[29])<<2)+
					(agent->Bit(agent->states[30])<<1)+
					(agent->Bit(agent->states[31]));
//		action=rand()&7;
		if(agentState==0){
			timeOut++;
		} else {
			timeIn++;
		}
		switch(action){
			case 0:
				//idle -> do nothing
				break;
			case 1:
				//turn left
				dir=(dir+1)&7;
				break;
			case 2:
				//turn right
				dir=(dir-1)&7;
				break;
			case 3:
				//go forward
				stepsSinceFoodIntake++;
				if(inside(xPos,yPos,xDim/2,yDim/2,1)){
					if(agentState==1){ //you come from food :)
						//                            agent->fitness+=foodCollected/(double)stepsSinceFoodIntake*(double)foodDist;
						//                        agent->fitness+=foodCollected;
						fitness+=(1.0/(double)stepsSinceFoodIntake)*(double)foodDist*(double)foodDist;
						foodCollected=0.0;
						if(defaultDist==0){
							foodDist+=1;
							if(foodDist>100)
								foodDist=100;
						}
						else{
							foodDist=defaultDist;
						}
						
						alpha=360.0*(double)rand()/(double)RAND_MAX;
						dX=(int)(sin(alpha/180.0*3.1415)*(double)foodDist);
						dY=(int)(cos(alpha/180.0*3.1415)*(double)foodDist);
						pX=(xDim/2)+dX;
						pY=(yDim/2)+dY;
						agentState=0;
						totalNrReturns++;
						timeInCollector.push_back(timeIn);
						timeOutCollector.push_back(timeOut);
						timeIn=0;timeOut=0;
						food=foodDist;
					}
				}
				xPos+=xm[dir];
				yPos+=ym[dir];
				break;
			case 4: case 5: case 6: case 7:
				//collect
				if(inside(xPos,yPos,pX,pY,1)){
					foodCollected=1.0;
					agentState=1; //you just collected food...
					stepsSinceFoodIntake=0;
				}
				break;
		}
	}
	 
	if(analyse){
		string lead="";
		if(constantFood) lead=lead+"f"; else lead=lead+"F";
		if(constantHome) lead=lead+"h"; else lead=lead+"H";
		if(doHammer) lead=lead+"R"; else lead=lead+"r";
		lead=lead+"_";
		double meanIn=0.0;
		double meanOut=0.0;
		if((timeInCollector.size()>0)&&(timeOutCollector.size()>0)){
			for(int z=0;z<timeInCollector.size();z++)
				meanIn+=(double)timeInCollector[z];
			for(int z=0;z<timeOutCollector.size();z++)
				meanOut+=(double)timeOutCollector[z];
			meanIn/=(double)timeInCollector.size();
			meanOut/=(double)timeOutCollector.size();
		}
		Data::Add(meanIn, lead+"meanIn", (Genome*)agent->genome);
		Data::Add(meanOut, lead+"meanOut", (Genome*)agent->genome);
		Data::Add(pow(1.05,fitness), lead+"W", (Genome*)agent->genome);
		Data::Add(foodDist,lead+"furthest",(Genome*)agent->genome);
		Data::Add(totalNrReturns,lead+"totalReturns",(Genome*)agent->genome);
		Data::Add(backtrackFood,lead+"btFood",(Genome*)agent->genome);
		Data::Add(backtrackHome,lead+"btHome",(Genome*)agent->genome);
		vector<vector<int>> M=agent->getConnectivityMatrix();
		Data::Add(Analyse::getDensity(M),"density",(Genome*)agent->genome);
		Data::Add(Analyse::getMaxConnectivityDensity(M),"maxConDense",(Genome*)agent->genome);
		Data::Add(Analyse::getMeanConnectivityDensity(M),"meanConDense",(Genome*)agent->genome);
	}
	return pow(1.01,fitness);

}