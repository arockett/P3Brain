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

#include "../AbstractWorld.h"

using namespace std;

class UtilityWorld: public AbstractWorld {

	// Parameters
    static shared_ptr<ParameterLink<int>> thinkTimePL;
    static shared_ptr<ParameterLink<double>> pNoisePL;
    static shared_ptr<ParameterLink<double>> piNoisePL;
    static shared_ptr<ParameterLink<double>> alphaPL;
    static shared_ptr<ParameterLink<string>> DATAFileNamePL;
    static shared_ptr<ParameterLink<int>> nrOfTestsPL;
	// End parameters

    int thinkTime;
    double pNoise;
    double piNoise;
    double alpha;
    string DATAFileName;
    int nrOfTests;

  public:
    UtilityWorld(shared_ptr<ParametersTable> _PT = nullptr);

    virtual void runWorldSolo( shared_ptr<Organism> org, bool analyse, bool visualize, bool debug ) override;

	double getPi(double p,double theAlpha);
	//extra methods added:
	double GetUtilityForAnsweringQuestion( shared_ptr<Organism> org, double P[2], double Pay[2]);

    virtual int requiredInputs() override
    {
        return 5;
    }

    virtual int requiredOutputs() override
    {
        return 4;
    }

    virtual int maxOrgsAllowed() override
    {
		return 1;
    }

    virtual int minOrgsAllowed() override
    {
		return 1;
    }
};

#endif /* UtilityWorld_hpp */
