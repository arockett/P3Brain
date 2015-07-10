//
//  main.cpp
//
//  Mad Bot AI
//		tested in:
//  Red Berry Blue Berry World
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <stdlib.h>
#include <vector>

#include "Genome.h"
#include "Optimizer.h"
#include "World.h"
#include "Data.h"
#include "Agent.h"
#include "RedBlueBerryWorld.h"
#include "MadBotAI.h"

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

using namespace std;

int main(int argc, const char * argv[]) {
	int update,updates=100;
	int popSize=100;
	int intervall=100;
	int nrOfBrainStates=16;
	vector<Genome*> population,newPopulation;
	vector<double> W;
	string LODFileName="";
	string GENFileName="";
	Optimizer *optimizer=(Optimizer*)new GA();
	int MadBotAI_inputs=256;
	int MadBotAI_actions=8;
	int MadBotAI_states=2;//32;
	int MadBotAI_granularity=2;

	//	Optimizer *optimizer=(Optimizer*)new Tournament();
	World *world= new RedBlueBerryWorld();//new World();
	
	//default command line paramters
	Data::setDefaultParameter("updates", &updates, 10);
	Data::setDefaultParameter("popSize", &popSize, 100);
	Data::setDefaultParameter("intervall", &intervall, 100);
	Data::setDefaultParameter("LOD", &LODFileName, "defaultLOD.csv");
	Data::setDefaultParameter("GEN", &GENFileName, "defaultGEN.csv");
	Data::setDefaultParameter("repeats", &World::repeats, 1);
	Data::setDefaultParameter("elitism", &Optimizer::elitism, -1);
	Data::setDefaultParameter("pointMutationRate", Data::makeDefaultDouble("pointMutationRate"), 0.005);
	Data::setDefaultParameter("brainSize", &nrOfBrainStates, 16);
	Data::setDefaultParameter("MBAI_states", &MadBotAI_states, 2);
	Data::setDefaultParameter("MBAI_granularity", &MadBotAI_granularity, 2);
	
	printf("Here! MadBot\n");
	//exit(0);

	srand(getpid()); // need different includes for windows XPLATFORM

	//TSK is extra
	Data::setDefaultParameter("TSK", &RedBlueBerryWorld::TSK, 0.0);
	
	
	//Mutation Scheme alteration
	int mutRedInt=0,mutRedCount=0;
	Data::setDefaultParameter("mutRedInt", &mutRedInt, 10000);
	Data::UseCommandLineParameters(argc,argv);
	
	//setup population
	for(int i=0;i<popSize;i++){
//		ClassicMBGenome *G=new ClassicMBGenome();
		MadBotGenome *G=new MadBotGenome();
		G->fillRandom(MadBotAI_inputs,MadBotAI_actions,MadBotAI_states,MadBotAI_granularity);
		population.push_back((Genome*)G);
	}
	
	//evolution loop
	for(update=0;update<updates;update++){
		//translate all genomes to agents
		vector<Agent*> agents;
		for(int i=0;i<popSize;i++){
//			agents.push_back(new Agent((ClassicMBGenome*)population[i],nrOfBrainStates));
			Agent *A=new MadBotAI((MadBotGenome*)population[i],nrOfBrainStates);
			agents.push_back(A);
		}
		//evaluate each organism in the population using a World
		W=world->evaluateFitness(agents,false);
		
		//delete all agents
		for(int i=0;i<popSize;i++)
			delete agents[i];
		
		//make next generation using an optimizer
		newPopulation=optimizer->makeNextGeneration(population,W);
		printf("update: %i maxFitness:%f %f\n",update,optimizer->maxFitness,Data::getDefaultDouble("pointMutationRate"));
		for(int i=0;i<population.size();i++){
			population[i]->kill();
			population[i]=newPopulation[i];
		}
		newPopulation.clear();
		mutRedCount++;
		if(mutRedCount>=mutRedInt){
			mutRedCount=0;
			double m=Data::getDefaultDouble("pointMutationRate");
			Data::setDefaultDouble("pointMutationRate", m/2.0);
		}
	}
	//Add additional Data through analyze mode
	//not needed in the basic example here
	//*
	 //get the LOD
	/*
	 vector<Genome*> LOD=Data::getLOD(population[0]->ancestor);
	printf("p %p %i\n",population[0]->ancestor,LOD.size());
	 //iterate over the LOD and have analyze on
	 //world will add all parameters you typically need to Data
	 for(int i=0;i<LOD.size();i++){
		 vector<Agent*> agents;
		 Agent *A=new Agent((ClassicMBGenome*)LOD[i],nrOfBrainStates);
		 agents.push_back(A);
		 world->evaluateFitness(agents, true);
		 delete A;
	 }
	 //*/
	//Data::showAll();

	//save data
	Data::saveLOD(population[0]->ancestor, LODFileName);
	Data::saveGEN(population[0]->ancestor, GENFileName, intervall);
	//Agent *A=new Agent((ClassicMBGenome*)population[0],16);
	//printf("%s\n",A->gateList().c_str());
	return 0;
}

