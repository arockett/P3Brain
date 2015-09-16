//
//  main.cpp
//  BasicMarkovBrainTemplate
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
#include "MutationAnalyse.h"

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
    int nrOfBrainStates=16;
	vector<Genome*> population,newPopulation;
	vector<double> W;
	string LODFileName="";
	string GENFileName="";
	Optimizer *optimizer=(Optimizer*)new GA();
//	Optimizer *optimizer=(Optimizer*)new Tournament();
	World *world= (World*)new ExampleWorld();//new World();
	bool gateFlags[8];
	
	//default command line paramters
	Data::setDefaultParameter("updates", &updates, 1000);
	Data::setDefaultParameter("popSize", &popSize, 100);
	Data::setDefaultParameter("intervall", &intervall, 100);
	Data::setDefaultParameter("LOD", &LODFileName, "defaultLOD.csv");
	Data::setDefaultParameter("GEN", &GENFileName, "defaultGEN.csv");
	Data::setDefaultParameter("repeats", &World::repeats, 1);
	Data::setDefaultParameter("elitism", &Optimizer::elitism, -1);
	Data::setDefaultParameter("pointMutationRate", Data::makeDefaultDouble("pointMutationRate"), 0.005);
    Data::setDefaultParameter("brainSize", &nrOfBrainStates, 16);
	Data::setDefaultParameter("probGate", &gateFlags[0], true);
	Data::setDefaultParameter("detGate", &gateFlags[1], true);
	Data::setDefaultParameter("fbGate", &gateFlags[2], true);
	Data::setDefaultParameter("gpGate", &gateFlags[3], false);
	Data::setDefaultParameter("thGate", &gateFlags[4], false);
	Data::setDefaultParameter("epsilonGate", &gateFlags[5], false);
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
    vector<Genome*> LOD=Data::getLOD(population[0]->ancestor);
    //iterate over the LOD and have analyze on
    //world will add all parameters you typically need to Data
    for(int i=0;i<LOD.size();i++){
        vector<Agent*> agents;
        Agent *A=new Agent((Genome*)LOD[i],nrOfBrainStates);
        agents.push_back(A);
        world->evaluateFitness(agents, true);
        delete A;
    }
    //*/

	//save data
	Data::saveLOD(population[0]->ancestor, LODFileName);
	Data::saveGEN(population[0]->ancestor, GENFileName, intervall);
	Agent *A=new Agent((Genome*)population[0],16);
	printf("%s\n",A->gateList().c_str());
    return 0;
}
