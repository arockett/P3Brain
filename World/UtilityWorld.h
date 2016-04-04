//
//  UtilityWorld.hpp
//  MarkovBrainPWFBias
//
//  Created by Arend Hintze on 11/19/15.
//  Copyright © 2015 Arend Hintze. All rights reserved.
//

#ifndef UtilityWorld_hpp
#define UtilityWorld_hpp

#include <stdio.h>
#include <stdlib.h>

#include "AbstractWorld.h"

using namespace std;

class UtilityWorld: public AbstractWorld {
	static const int& thinkTime;
	static const double& pNoise;
	static const double& piNoise;
	static const double& alpha;
	static const string& DATAFileName;
	static const int& nrOfTests;
	
public:
	UtilityWorld();
    virtual double testIndividual( shared_ptr<Organism> org, bool analyse, bool show = 0 ) override;
	double getPi(double p,double theAlpha);
	//extra methods added:
	double GetUtilityForAnsweringQuestion( shared_ptr<Organism> org, double P[2], double Pay[2]);

    virtual int requiredInputs() override;
    virtual int requiredOutputs() override;
	
};

#endif /* UtilityWorld_hpp */
