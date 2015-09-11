//
//  main.cpp
//  FrenchFlag
//
//  Created by Heather Goldsby on 9.10.2015
//  Copyright (c) 2015 Heather Goldsby. All rights reserved.
//

#include <stdlib.h>
#include <vector>

#include "Genome.h"
#include "Optimizer.h"
#include "World.h"
#include "Data.h"
#include "Agent.h"
#include "FrenchFlagWorld.h"

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
	World *world= (World*)new FrenchFlagWorld();//new World();
	bool gateFlags[8];
	
	//default command line paramters
	Data::setDefaultParameter("updates", &updates, 100);
	Data::setDefaultParameter("popSize", &popSize, 100);
	Data::setDefaultParameter("intervall", &intervall, 100);
	Data::setDefaultParameter("LOD", &LODFileName, "defaultLOD.csv");
	Data::setDefaultParameter("GEN", &GENFileName, "defaultGEN.csv");
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
	
	Data::setDefaultParameter("max_x", &FrenchFlagWorld::max_x, 6);
	Data::setDefaultParameter("max_y", &FrenchFlagWorld::max_y, 6);

	Data::UseCommandLineParameters(argc,argv);
	
	//setup Gates
	for(int i=0;i<256;i++)
		Gate::AddGate(i, NULL);
	if(gateFlags[0]) Gate::AddGate(42,[](ClassicMBGenome* genome,int pos) {return new Gate(genome,pos);});
	if(gateFlags[1]) Gate::AddGate(43,[](ClassicMBGenome* genome,int pos) {return new DeterministicGate(genome,pos);});
	if(gateFlags[2]) Gate::AddGate(44,[](ClassicMBGenome* genome,int pos) {return new FeedbackGate(genome,pos);});
	if(gateFlags[3]) Gate::AddGate(45,[](ClassicMBGenome* genome,int pos) {return new GPGate(genome,pos);});
	if(gateFlags[4]) Gate::AddGate(46,[](ClassicMBGenome* genome,int pos) {return new Thresholdgate(genome,pos);});
	if(gateFlags[5]) Gate::AddGate(47,[](ClassicMBGenome* genome,int pos) {return new FixedEpsilonGate(genome,pos);});
	
	
	//printf("Here!\n");
	//exit(0);
	
	srand(getpid()); // need different includes for windows XPLATFORM
	
	
	//setup population
	for(int i=0;i<popSize;i++){
		ClassicMBGenome *G=new ClassicMBGenome();
		G->fillRandom();
		population.push_back((Genome*)G);
	}
	
	//evolution loop
	for(Data::update=0;Data::update<updates;Data::update++){
		//translate all genomes to agents
		vector<Agent*> agents;
		for(int i=0;i<popSize;i++)
			agents.push_back(new Agent((ClassicMBGenome*)population[i],nrOfBrainStates));
		
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
	Genome *mostRecentCommonAncestor=Data::getMostRecentCommonAncestor(population[0]);
	vector<Genome*> LOD=Data::getLOD(mostRecentCommonAncestor);
	if(true){
		vector<Agent*> agents;
		for(Genome* G:LOD){
			agents.push_back(new Agent((ClassicMBGenome*)G,nrOfBrainStates));
		}
		world->evaluateFitness(agents, true);
	}
	//save data
	Data::saveLOD(mostRecentCommonAncestor, LODFileName);
	Data::saveGEN(mostRecentCommonAncestor, GENFileName, intervall);
	Agent *A=new Agent((ClassicMBGenome*)population[0],16);
	printf("%s\n",A->gateList().c_str());
	return 0;
}
