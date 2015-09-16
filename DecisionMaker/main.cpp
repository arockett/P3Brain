//
//  main.cpp
//  DecisionMaker
//
//  Created by Arend Hintze on 7/27/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <stdlib.h>
#include <vector>

#include "Genome.h"
#include "Optimizer.h"
#include "World.h"
#include "Data.h"
#include "Agent.h"
#include "DecisionMakerWorld.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

int main(int argc, const char * argv[]) {
	int updates;
	int popSize;
	int intervall;
	int nrOfBrainStates;
	vector<Genome*> population,newPopulation;
	vector<double> W;
	string LODFileName="";
	string GENFileName="";
	Optimizer *optimizer=(Optimizer*)new GA();
	//	Optimizer *optimizer=(Optimizer*)new Tournament();
	World *world= (World*)new DecisionMakerWorld();//new World();
	bool gateFlags[8];
	
	//default command line paramters
	Data::setDefaultParameter("updates", &updates, 100);
	Data::setDefaultParameter("popSize", &popSize, 100);
	Data::setDefaultParameter("intervall", &intervall, 100);
	Data::setDefaultParameter("LOD", &LODFileName, "defaultLOD.csv");
	Data::setDefaultParameter("GEN", &GENFileName, "defaultGEN.csv");
	Data::setDefaultParameter("DATA", &DecisionMakerWorld::DATAFileName, "defaultDATA.csv");
	Data::setDefaultParameter("repeats", &World::repeats, 1);
	Data::setDefaultParameter("elitism", &Optimizer::elitism, -1);
	Data::setDefaultParameter("pointMutationRate", Data::makeDefaultDouble("pointMutationRate"), 0.005);
	Data::setDefaultParameter("phiFileName", Data::makeDefaultString("phiFileName"), "defaultPHIfile.csv");
	Data::setDefaultParameter("brainSize", &nrOfBrainStates, 16);
	Data::setDefaultParameter("probGate", &gateFlags[0], true);
	Data::setDefaultParameter("detGate", &gateFlags[1], true);
	Data::setDefaultParameter("fbGate", &gateFlags[2], true);
	Data::setDefaultParameter("gpGate", &gateFlags[3], false);
	Data::setDefaultParameter("thGate", &gateFlags[4], false);
	Data::setDefaultParameter("epsilonGate", &gateFlags[5], false);
	Data::setDefaultParameter("epsilon", &FixedEpsilonGate::epsilon, 0.1);
	
	Data::setDefaultParameter("thinkTime", &DecisionMakerWorld::thinkTime, 10);
	Data::setDefaultParameter("pNoise", &DecisionMakerWorld::pNoise, 0.5);
	Data::setDefaultParameter("payNoise", &DecisionMakerWorld::piNoise, 0.5);
	Data::setDefaultParameter("alpha", &DecisionMakerWorld::alpha, 0.0);
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
	
	
	//setup population
	for(int i=0;i<popSize;i++){
		Genome *G=new Genome();
		G->fillRandom();
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
	Genome *mostRecentCommonAncestor=Data::getMostRecentCommonAncestor(population[0]->ancestor);
	vector<Genome*> LOD=Data::getLOD(population[0]->ancestor);//mostRecentCommonAncestor);
	if(true){
		vector<Agent*> agents;
		/*
		for(Genome* G:LOD){
			agents.push_back(new Agent((Genome*)G,nrOfBrainStates));
		}
		 */
		agents.push_back(new Agent((Genome*)mostRecentCommonAncestor,nrOfBrainStates));
		world->evaluateFitness(agents, true);
	}
	//save data
	Data::saveLOD(population[0]->ancestor, LODFileName);
	Data::saveGEN(population[0]->ancestor, GENFileName, intervall);
	Agent *A=new Agent((Genome*)population[0]->ancestor,16);
	printf("%s\n",A->gateList().c_str());
	return 0;
}
