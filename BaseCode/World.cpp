//
//  World.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <math.h>

#include "World.h"

#include "../Utilities/Random.h"
#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"


int& World::repeats = Parameters::register_parameter("repeats", 1, "Number of times to test each Genome per generation","WORLD");


World::World(){
}

World::~World(){
}

vector<double> World::evaluateFitness(vector<Agent*> agents,bool analyse){
	vector<double> W;
	for(size_t i=0;i<agents.size();i++){
		double w=0.0;
		for(int r=0;r<World::repeats;r++){
			w+=testIndividual(agents[i],analyse);
		}
        W.push_back(w/(double)World::repeats);
//		W.push_back(pow(w,1.0/(double)World::repeats)); // change to this line if you want geometic mean?
	}
	return W;
}

double World::testIndividual(Agent *agent,bool analyse){
	agent->genome->dataMap.Set("W",0.0);
    return 1.0;
}

/* *** Example World Implementation *** */
ExampleWorld::ExampleWorld(){
}
ExampleWorld::~ExampleWorld(){
}

vector<double> ExampleWorld::evaluateFitness(vector<Agent*> agents,bool analyse){
	vector<double> W;
	for(size_t i=0;i<agents.size();i++){
		double w=0.0;
		for(int r=0;r<World::repeats;r++){
			w+=testIndividual(agents[i],analyse);
		}
        W.push_back(w/(double)World::repeats);
//		W.push_back(pow(w,1.0/(double)World::repeats)); // Arend, What does this mean?
	}
	return W;
}

double ExampleWorld::testIndividual(Agent *agent,bool analyse){
	double fitness=0.0;
	agent->resetBrain();
	for(int t=0;t<100;t++){
		int I[8];
		for(int i=0;i<3;i++){
			I[i]=Random::getInt(1);
			agent->states[i]=(double)I[i];
		}
		
		agent->updateStates();
		for(int i=0;i<3;i++){
			if(Bit(agent->states[8+i])==(int)I[i]){
				fitness+=1.0;
			}
		}
	}
	agent->genome->dataMap.Set("W", fitness);
	
	return fitness;
}

/* *** ADVANCED IMPLEMENTATION FOR MULTITHREADED RUNS *** */
/* *** CODE FOR INTEL PHI COPROCESSOR RUNS *** */
/* if you want to use this functionality you need to include pThreads 
 and derive your world from this class! Don't overload evaluate Fitness,
 use the Replicates variable to run multiple replicates for each
 agent if necessary */

/*
void threadedEvaluateFitness(int chunkBegin, int chunkEnd, const vector<Agent*>& agents, Game& game, int& evaluations){
    for (int chunk_index=chunkBegin; chunk_index<chunkEnd; chunk_index++) {
		for (int replicate_i=0; replicate_i < World::repeats; replicate_i++) {
			game.executeAgent(agents[chunk_index]);
			agents[chunk_index]->fitnesses.push_back(agents[chunk_index]->fitness);
		}
	}
}
*/

MultiThreadWorld::MultiThreadWorld(){
	nthreads=thread::hardware_concurrency();
	printf("Available Cores: %i\n",nthreads);
}

MultiThreadWorld::~ MultiThreadWorld(){
    
}

void threadedEvaluateFitness(int chunkBegin, int chunkEnd, const vector<Agent*>& agents, int& evaluations){
	for (int chunk_index=chunkBegin; chunk_index<chunkEnd; chunk_index++) {
		MultiThreadWorld::staticTestIndividual(agents[chunk_index],false);
	}
}

vector<double> MultiThreadWorld::evaluateFitness(vector<Agent*> agents,bool analyse){
    vector<thread> threads;
	vector<double> W;
    threads.clear();
    int chunksize=(int)agents.size()/nthreads;
    for (int threadid=0; threadid < nthreads; threadid++)
        threads.push_back(thread(threadedEvaluateFitness, chunksize*threadid, chunksize*threadid+chunksize, ref(agents), ref(World::repeats)));
    
    if (agents.size()%nthreads != 0) {// take care of any uneven division of workload
        threads.push_back(thread(threadedEvaluateFitness, nthreads*chunksize, agents.size(), ref(agents), ref(World::repeats)));
    }
    for (thread& t : threads) t.join(); // wait for all threads to finish
	for(size_t i=0;i<agents.size();i++){
		double thisW;
		std::stringstream ss(agents[i]->genome->dataMap.Get("W"));
		ss >> thisW; // use a string stream to convert string to double
		W.push_back(thisW);
	}
	return W;
}

void MultiThreadWorld::staticTestIndividual(Agent *agent,bool analyse){
	agent->genome->dataMap.Set("W",Random::getDouble(1));
}

