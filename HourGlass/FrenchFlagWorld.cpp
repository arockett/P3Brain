//
//  FrenchFlagWorld.cpp
//  BasicMarkovBrainTemplate
//
//  Created by Heather Goldsby on 9.10.2015
//  Copyright (c) 2015 Heather Goldsby. All rights reserved.
//

#include "FrenchFlagWorld.h"

int FrenchFlagWorld::max_x = 6;
int FrenchFlagWorld::max_y = 6;

FrenchFlagWorld::FrenchFlagWorld(){
    
}

double FrenchFlagWorld::testIndividual(Agent *agent, bool analyse){
	double fitness=1.0;
	int correct=0;
    
    int grid_size = max_x * max_y;
    
    // Create a grid full of agents!
    // Grow function?
    vector<Agent*> grid_agents;
    
    for(int i=0;i<grid_size;i++) {
        grid_agents.push_back(new Agent(agent->genome, agent->nrOfStates));
        
        // Hook up the brains together -- location? output of neighbors?

        
    }
    

    
    // Run agents for X updates
    for(int t=0;t<1000;t++){
        agent->updateStates();
    }
    
    
    // Check results...
    for (int x=0; x < max_x; ++x) {
        for (int y=0; y < max_y; ++y) {
            int a = x * max_y + y;
            
            if ((x < (floor(max_x) / 3 )) /* and agent state is blue */) {
                ++correct;
            } else if (((x > (floor(max_x) / 3))  && (x < floor(max_x) / 3 * 2)) /* and agent state is white */){
                ++correct;
            } else if ( true/* and agent state is red */) {
                ++correct;
            }
        }
    }
    
    
    
    //delete all agents
    for(int i=0;i<grid_size;i++) {
        delete grid_agents[i];
    }
    
    // for the time being these are equal, prolly a gamma transform is in order
    fitness = correct;
        
                    
                         

	
	if(analyse){
		Data::Add(correct, "correct", (Genome*)agent->genome);
		Data::Add(fitness, "fitness", (Genome*)agent->genome);
	}
	return fitness;
}