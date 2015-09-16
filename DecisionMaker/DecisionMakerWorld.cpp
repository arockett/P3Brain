//
//  DecisionMakerWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 7/27/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include "DecisionMakerWorld.h"

int DecisionMakerWorld::thinkTime=1;
double DecisionMakerWorld::pNoise=0.25;
double DecisionMakerWorld::piNoise=0.25;
double DecisionMakerWorld::alpha=0.0;
string DecisionMakerWorld::DATAFileName="";

double DecisionMakerWorld::getPi(double p,double theAlpha){
	return (-0.5*theAlpha+0.5)+(p*theAlpha);
}

DecisionMakerWorld::DecisionMakerWorld(){
	
}

double DecisionMakerWorld::testIndividual(Agent *agent, bool analyse){
	double fitness=1.0;
	int correct=0;
	int incorrect=0;
	double fixedP[10]={0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95};
	if(analyse){
		FILE *F=fopen(DATAFileName.c_str(),"w+t");
		fprintf(F,"p1,pay1,p2,pay2,U1,U2,C1,C2,undec\n");
		for(int t1=0;t1<100;t1++){
			for(int t2=0;t2<100;t2++){
				agent->resetBrain();
				double P[2],Pay[2],U[2];
				P[0]=fixedP[t1%10];
				Pay[0]=fixedP[(int)(t1/10)];
				P[1]=fixedP[t2%10];
				Pay[1]=fixedP[(int)(t2/10)];
				U[0]=P[0]*Pay[0];
				U[1]=P[1]*Pay[1];
				bool timedOut=true;
				for(int j=0;j<thinkTime+10;j++){
					agent->states[0]=((double)rand()/(double)RAND_MAX)<P[0]?1.0:0.0;
					agent->states[1]=((double)rand()/(double)RAND_MAX)<P[1]?1.0:0.0;
					agent->states[2]=((double)rand()/(double)RAND_MAX)<Pay[0]?1.0:0.0;
					agent->states[3]=((double)rand()/(double)RAND_MAX)<Pay[1]?1.0:0.0;
					if(j>thinkTime)
						agent->states[4]=1;
					else
						agent->states[4]=0;
					agent->updateStates();
					if(j>thinkTime){
						int action=0;
						if(agent->Bit(agent->states[5])==0)
							action=agent->Bit(agent->states[6]);
						if(agent->Bit(agent->states[8])==0)
							action=agent->Bit(agent->states[7])*2;
						switch(action){
							case 0: case 3: // no decicive answer
								break;
							case 1:
								fitness*=1.0+U[0];
								fprintf(F,"%f,%f,%f,%f,%f,%f,1,0,0\n",P[0],Pay[0],P[1],Pay[1],U[0],U[1]);
								j=thinkTime*100;
								timedOut=false;
								break;
							case 2:
								fitness*=1.0+U[1];
								fprintf(F,"%f,%f,%f,%f,%f,%f,0,1,0\n",P[0],Pay[0],P[1],Pay[1],U[0],U[1]);
								j=thinkTime*100;
								timedOut=false;
								break;
						}
					}
				}
				if(timedOut){
					if(U[0]<U[1])
						fprintf(F,"%f,%f,%f,%f,%f,%f,1,0,1\n",P[0],Pay[0],P[1],Pay[1],U[0],U[1]);
					else
						fprintf(F,"%f,%f,%f,%f,%f,%f,0,1,1\n",P[0],Pay[0],P[1],Pay[1],U[0],U[1]);
				}
			}
		}
		fclose(F);
	} else {
		// normal evolution
		for(int i=0;i<repeats;i++){
			agent->resetBrain();
			double P[2],Pay[2],U[2];
			for(int l=0;l<2;l++){
				double nx,ny;
				do{
					nx=(((double)rand()/(double)RAND_MAX)*pNoise)-(pNoise/2.0);
					ny=(((double)rand()/(double)RAND_MAX)*piNoise)-(piNoise/2.0);
					P[l]=((double)rand()/(double)RAND_MAX)+nx;
					Pay[l]=getPi(P[l],alpha)+ny;
				}while( (P[l]<0.0)||(P[l]>1.0)||(Pay[l]<0.0)||(Pay[l]>1.0));
				U[l]=P[l]*Pay[l];
			}
			bool timedOut=true;
			for(int j=0;j<thinkTime+10;j++){
				agent->states[0]=((double)rand()/(double)RAND_MAX)<P[0]?1.0:0.0;
				agent->states[1]=((double)rand()/(double)RAND_MAX)<P[1]?1.0:0.0;
				agent->states[2]=((double)rand()/(double)RAND_MAX)<Pay[0]?1.0:0.0;
				agent->states[3]=((double)rand()/(double)RAND_MAX)<Pay[1]?1.0:0.0;
				if(j>thinkTime)
					agent->states[4]=1;
				else
					agent->states[4]=0;
				agent->updateStates();
				if(j>thinkTime){
					int action=0;
					if(agent->Bit(agent->states[5])==0)
						action=agent->Bit(agent->states[6]);
					if(agent->Bit(agent->states[8])==0)
						action=agent->Bit(agent->states[7])*2;
					switch(action){
						case 0: case 3: // no decicive answer
							break;
						case 1:
							fitness*=1.0+U[0];
							if(U[0]>U[1]){
								correct++;
								if(analyse)
									Data::Add(1, "P"+to_string(P[1])+"_Pay"+to_string(Pay[1]), (Genome*)agent->genome);
							}
							else{
								incorrect++;
								if(analyse)
									Data::Add(0, "P"+to_string(P[1])+"_Pay"+to_string(Pay[1]), (Genome*)agent->genome);
							}
							j=thinkTime*100;
							timedOut=false;
							break;
						case 2:
							fitness*=1.0+U[1];
							if(U[1]>U[0]){
								correct++;
							}
							else{
								incorrect++;
							}
							j=thinkTime*100;
							timedOut=false;
							break;
					}
				}
			}
			if(timedOut){
				incorrect++;
			}
		}
		Data::Add(correct, "correct", (Genome*)agent->genome);
		Data::Add(incorrect, "incorrect", (Genome*)agent->genome);
		Data::Add(fitness, "fitness", (Genome*)agent->genome);
	}
	return fitness;
}