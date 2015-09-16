//
//  main.cpp
//  RobustNavigation
//
//  Created by Arend Hintze on 7/2/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <stdlib.h>
#include <vector>

#include "Genome.h"
#include "Optimizer.h"
#include "World.h"
#include "Data.h"
#include "Agent.h"
#include "BlinkyWorld.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

int main(int argc, const char * argv[]) {
	int updates=100;
	int popSize=100;
	int intervall=100;
	int nrOfBrainStates=32;
	vector<Genome*> population,newPopulation;
	vector<double> W;
	string LODFileName="";
	string GENFileName="";
	Optimizer *optimizer=(Optimizer*)new GA();
	//	Optimizer *optimizer=(Optimizer*)new Tournament();
	World *world= (World*)new BlinkyWorld();//new World();
	bool gateFlags[8];
	
	//default command line paramters
	Data::setDefaultParameter("updates", &updates, 100);
	Data::setDefaultParameter("popSize", &popSize, 100);
	Data::setDefaultParameter("intervall", &intervall, 100);
	Data::setDefaultParameter("LOD", &LODFileName, "defaultLOD.csv");
	Data::setDefaultParameter("GEN", &GENFileName, "defaultGEN.csv");
	Data::setDefaultParameter("repeats", &World::repeats, 1);
	Data::setDefaultParameter("elitism", &Optimizer::elitism, 1);
	Data::setDefaultParameter("pointMutationRate", Data::makeDefaultDouble("pointMutationRate"), 0.005);
	Data::setDefaultParameter("brainSize", &nrOfBrainStates, 32);
	Data::setDefaultParameter("probGate", &gateFlags[0], false);
	Data::setDefaultParameter("detGate", &gateFlags[1], false);
	Data::setDefaultParameter("fbGate", &gateFlags[2], false);
	Data::setDefaultParameter("gpGate", &gateFlags[3], false);
	Data::setDefaultParameter("thGate", &gateFlags[4], false);
	Data::setDefaultParameter("epsilonGate", &gateFlags[5], true);
	Data::setDefaultParameter("epsilon", &FixedEpsilonGate::epsilon, 0.1);
	Data::setDefaultParameter("skipGate", Data::makeDefaultDouble("skipGate"), 0.0);
	Data::setDefaultParameter("voidOutput", Data::makeDefaultDouble("voidOutput"), 0.0);
	
	Data::UseCommandLineParameters(argc,argv);
	
	//setup Gates
	for(int i=0;i<256;i++)
		Gate::AddGate(i, NULL);
	if(gateFlags[0]) Gate::AddGate(42,[](Genome* genome,int pos) {return new Gate(genome,pos);});
	if(gateFlags[1]) Gate::AddGate(43,[](Genome* genome,int pos) {return new DeterministicGate(genome,pos);});
	if(gateFlags[2]) Gate::AddGate(44,[](Genome* genome,int pos) {return new FeedbackGate(genome,pos);});
	if(gateFlags[3]) Gate::AddGate(45,[](Genome* genome,int pos) {return new GPGate(genome,pos);});
	if(gateFlags[4]) Gate::AddGate(46,[](Genome* genome,int pos) {return new Thresholdgate(genome,pos);});
	if(gateFlags[5]) Gate::AddGate(47,[](Genome* genome,int pos) {return new FixedEpsilonGate(genome,pos);});
	
	
	//printf("Here!\n");
	//exit(0);
	
	srand(getpid()); // need different includes for windows XPLATFORM
	vector<double> lW;
	Genome *defaultAncestor=NULL;
	int C=0;
	do{
		vector<Agent*> lPop;
		if(defaultAncestor!=NULL)
			defaultAncestor->kill();
		defaultAncestor=new Genome();
		defaultAncestor->fillRandom();
		lPop.push_back(new Agent(defaultAncestor,nrOfBrainStates));
		lW=world->evaluateFitness(lPop, false);
		printf("%i %f\n",C++,lW[0]);//,lPop[0]->gateList().c_str());
		delete lPop[0];
	}while(lW[0]<=1.0);
	
	//setup population
	for(int i=0;i<popSize;i++){
		Genome *G;
		if(defaultAncestor==NULL){
			G=new Genome();
			G->fillRandom();
		} else {
			G=(Genome*)defaultAncestor->makeMutatedOffspring(0.01);
		}
		population.push_back((Genome*)G);
	}
	
	//evolution loop
	for(Data::update=0;Data::update<updates;Data::update++){
		//translate all genomes to agents
		vector<Agent*> agents;
		for(int i=0;i<popSize;i++)
			agents.push_back(new Agent((Genome*)population[i],nrOfBrainStates));
		
		//evaluate each organism in the population using a World
		W=world->evaluateFitness(agents,false);
		
		//delete all agents
		for(int i=0;i<popSize;i++)
			delete agents[i];
		
		//make next generation using an optimizer
		newPopulation=optimizer->makeNextGeneration(population,W);
		printf("update: %i maxFitness:%f\n",Data::update,optimizer->maxFitness);
		for(int i=0;i<population.size();i++){
			population[i]->kill();
			population[i]=newPopulation[i];
		}
		newPopulation.clear();
	}
	
	//Add additional Data through analyze mode
	//not needed in the basic example here
	//*
	//get the LOD
//	Genome *mostRecentCommonAncestor=Data::getMostRecentCommonAncestor(population[0]);
	vector<Genome*> LOD=Data::getLOD(population[0]->ancestor);
	//iterate over the LOD and have analyze on
	//world will add all parameters you typically need to Data
	for(int condition=0;condition<3;condition++){
		switch(condition){
			case 0:
				BlinkyWorld::doHammer=false;
				BlinkyWorld::constantFood=true;
				BlinkyWorld::constantHome=true;
				break;
			case 1:
				BlinkyWorld::doHammer=true;
				BlinkyWorld::constantFood=true;
				BlinkyWorld::constantHome=true;
				break;
			case 2:
				BlinkyWorld::doHammer=false;
				BlinkyWorld::constantFood=false;
				BlinkyWorld::constantHome=true;
				break;
		}
		Genome::insertionDeletionP=0.0;
		for(int i=0;i<LOD.size();i++){
			vector<Agent*> agents;
			Agent *A=new Agent((Genome*)LOD[i],nrOfBrainStates);
			agents.push_back(A);
			world->evaluateFitness(agents, true);
			/* testing Mutational Robustness */
			if(condition==0){
				double wtFitness=0.0;
				if(i==LOD.size()-1){
					/* testing Mutational Robustness */
					double mys[6]={0.0,0.01,0.02,0.03,0.04,0.05};
					for(int m=0;m<6;m++){
						double W=0.0;
						vector<Agent*> rob_agents;
						for(int j=0;j<1000;j++)
							rob_agents.push_back(new Agent((Genome*)LOD[i]->makeMutatedOffspring(mys[m]),nrOfBrainStates));
						vector<double> robW=world->evaluateFitness(rob_agents, false);
						for(int j=0;j<rob_agents.size();j++){
							W+=robW[j];
							rob_agents[j]->genome->kill();
							delete rob_agents[j];
						}
						W/=(double)rob_agents.size();
						if(m==0){
							wtFitness=W;
							Data::Add(W/wtFitness, "mr"+to_string(mys[m]), LOD[i]);
						} else {
							Data::Add(W/wtFitness, "mr"+to_string(mys[m]), LOD[i]);
						}
					}
					/* testing cognitive robustness */
					double robMy[6]={0.0,0.002,0.004,0.008,0.016,0.032};
					for(int m=1;m<6;m++){
						double W=0.0;
						vector<Agent*> rob_agents;
						for(int j=0;j<1000;j++)
							rob_agents.push_back(new Agent((Genome*)LOD[i]->makeMutatedOffspring(0.0),nrOfBrainStates));
						BlinkyWorld::externalCognitiveNoise=robMy[m];
						vector<double> robW=world->evaluateFitness(rob_agents, false);
						for(int j=0;j<rob_agents.size();j++){
							W+=robW[j];
							rob_agents[j]->genome->kill();
							delete rob_agents[j];
						}
						W/=(double)rob_agents.size();
						Data::Add(W/wtFitness, "cogR"+to_string(robMy[m]), LOD[i]);
					}
				} else {
					//*
					Data::Add(0.0, "mr0.00", LOD[i]);
					Data::Add(0.0, "mr0.01", LOD[i]);
					Data::Add(0.0, "mr0.02", LOD[i]);
					Data::Add(0.0, "mr0.03", LOD[i]);
					Data::Add(0.0, "mr0.04", LOD[i]);
					Data::Add(0.0, "mr0.05", LOD[i]);
					Data::Add(0.0, "cogR0.002", LOD[i]);
					Data::Add(0.0, "cogR0.004", LOD[i]);
					Data::Add(0.0, "cogR0.008", LOD[i]);
					Data::Add(0.0, "cogR0.016", LOD[i]);
					Data::Add(0.0, "cogR0.032", LOD[i]);
					 //*/
				}
			}
			delete A;
		}
	}
	//*/
	
	//save data
	Data::saveLOD(population[0]->ancestor, LODFileName);
	Data::saveGEN(population[0]->ancestor, GENFileName, intervall);
	return 0;
}
