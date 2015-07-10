//
//  main.cpp
//  MadBotAI_simpleDemo
//
//  Created by Arend Hintze on 7/6/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <iostream>
#include <stdlib.h>

/* Mad Bot specific includes */
#include "Genome.h"
#include "Agent.h"
#include "MadBotAI.h"

using namespace std;

int main(int argc, const char * argv[]) {
	//make the genome
	MadBotGenome *genome=new MadBotGenome();
	const int _states=4;
	const int _inputs=10;
	const int _actions=10;
	const int _granularity=4;
	genome->fillRandom(_inputs, _actions, _states, _granularity);
	//translate genome to madbot agent
	MadBotAI *agent=new MadBotAI(genome);
	//demo loop
	for(int i=0;i<100;i++){
		printf("action: %i\n",agent->makeUpdate(rand()%10));
	}
    return 0;
}
