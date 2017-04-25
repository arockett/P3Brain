//
//  UtilityWorld.cpp
//  MarkovBrainPWFBias
//
//  Created by Arend Hintze on 11/19/15.
//  Copyright © 2015 Arend Hintze. All rights reserved.
//

#include "UtilityWorld.h"

shared_ptr<ParameterLink<double>> UtilityWorld::pNoisePL = Parameters::register_parameter("WORLD_UTILITY-pNoise", 0.25, "delta noise on p");
shared_ptr<ParameterLink<double>> UtilityWorld::piNoisePL = Parameters::register_parameter("WORLD_UTILITY-piNoise", 0.25, "delta noise on pi");
shared_ptr<ParameterLink<double>> UtilityWorld::alphaPL = Parameters::register_parameter("WORLD_UTILITY-alpha", 0.0, "alpha parameter");
shared_ptr<ParameterLink<int>> UtilityWorld::thinkTimePL = Parameters::register_parameter("WORLD_UTILITY-thinkTime", 100, "think time allowed");
shared_ptr<ParameterLink<string>> UtilityWorld::DATAFileNamePL = Parameters::register_parameter("WORLD_UTILITY-DATAFileName", (string) "defaultDataFile.csv", "file to save the extra data into");
shared_ptr<ParameterLink<int>> UtilityWorld::nrOfTestsPL = Parameters::register_parameter("WORLD_UTILITY-nrOfTests", 100, "how many times is a brain asked to do teh decision task");

UtilityWorld::UtilityWorld(shared_ptr<ParametersTable> _PT) :
        AbstractWorld(_PT)
{
    pNoise = (PT == nullptr) ? pNoisePL->lookup() : PT->lookupDouble("WORLD_UTILITY-pNoise");
    piNoise = (PT == nullptr) ? piNoisePL->lookup() : PT->lookupDouble("WORLD_UTILITY-piNoise");
    alpha = (PT == nullptr) ? alphaPL->lookup() : PT->lookupDouble("WORLD_UTILITY-alpha");
    thinkTime = (PT == nullptr) ? thinkTimePL->lookup() : PT->lookupInt("WORLD_UTILITY-thinkTime");
    DATAFileName = (PT == nullptr) ? DATAFileNamePL->lookup() : PT->lookupString("WORLD_UTILITY-DATAFileName");
    nrOfTests = (PT == nullptr) ? nrOfTestsPL->lookup() : PT->lookupInt("WORLD_UTILITY-nrOfTests");
}

double UtilityWorld::getPi( double p, double theAlpha )
{
    return ( -0.5*theAlpha + 0.5 ) + ( p*theAlpha );
}


double UtilityWorld::GetUtilityForAnsweringQuestion( shared_ptr<Organism> org, double P[2], double Pay[2] )
{
    double U[2];
    U[0] = P[0] * Pay[0];
    U[1] = P[1] * Pay[1];

    auto& agent = org->brain;
    agent->resetBrain();
    for( int j = 0; j < thinkTime + 10; j++ ) {
        agent->nodes[0] = Random::getDouble( 1.0 ) < P[0] ? 1.0 : 0.0;
        agent->nodes[1] = Random::getDouble( 1.0 ) < P[1] ? 1.0 : 0.0;
        agent->nodes[2] = Random::getDouble( 1.0 ) < Pay[0] ? 1.0 : 0.0;
        agent->nodes[3] = Random::getDouble( 1.0 ) < Pay[1] ? 1.0 : 0.0;
        if( j > thinkTime )
            agent->nodes[4] = 1;
        else
            agent->nodes[4] = 0;
        agent->update();
        if( j > thinkTime ) {
            int action = 0;
            if( Bit( agent->nodes[5] ) == 0 )
                action = Bit( agent->nodes[6] );
            if( Bit( agent->nodes[8] ) == 0 )
                action = Bit( agent->nodes[7] ) * 2;
            switch( action ) {
            case 0: case 3: // no decicive answer
                break;
            case 1:
                return U[0];
                break;
            case 2:
                return U[1];
                break;
            }
        }
    }
    return 0.0;
}

void UtilityWorld::runWorldSolo( shared_ptr<Organism> org, bool analyse, bool visualize, bool debug )
{
    auto& agent = org->brain;
    double fitness = 1.0;
    int correct = 0;
    int incorrect = 0;
    double fixedP[10] = { 0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95 };
    if( true ) {
        //FILE *F = fopen( DATAFileName.c_str(), "w+t" );
        //fprintf( F, "p1,pay1,p2,pay2,U1,U2,C1,C2,undec\n" );
        for( int t1 = 0; t1 < 100; t1++ ) {
            for( int t2 = 0; t2 < 100; t2++ ) {
                agent->resetBrain();
                double P[2], Pay[2], U[2];
                P[0] = fixedP[t1 % 10];
                Pay[0] = fixedP[(int)( t1 / 10 )];
                P[1] = fixedP[t2 % 10];
                Pay[1] = fixedP[(int)( t2 / 10 )];
                U[0] = P[0] * Pay[0];
                U[1] = P[1] * Pay[1];
                double answer = GetUtilityForAnsweringQuestion( org, P, Pay );
                fitness *= 1.0 + answer;
                // if( answer == 0.0 ) {
                //     if( U[0] < U[1] ) {
                //         fprintf( F, "%f,%f,%f,%f,%f,%f,1,0,1\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                //     }
                //     else {
                //         fprintf( F, "%f,%f,%f,%f,%f,%f,0,1,1\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                //     }
                // }
                // else {
                //     if( U[0] == answer ) {
                //         fprintf( F, "%f,%f,%f,%f,%f,%f,1,0,0\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                //     }
                //     else {
                //         fprintf( F, "%f,%f,%f,%f,%f,%f,0,1,0\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                //     }
                // }
            }
        }
        //fclose( F );
    }
    else {
        // normal evolution
        for( int i = 0; i < nrOfTests; i++ ) {
            agent->resetBrain();
            double P[2], Pay[2], U[2];
            for( int k = 0; k < 2; k++ ) {
                double nx, ny;
                //this creates random variables from the defined distribution
                //this needs to become the predefined question matrix
                do {
                    nx = ( Random::getDouble( 1.0 )*pNoise ) - ( pNoise / 2.0 );
                    ny = ( Random::getDouble( 1.0 )*piNoise ) - ( piNoise / 2.0 );
                    P[k] = Random::getDouble( 1.0 ) + nx;
                    Pay[k] = getPi( P[k], alpha ) + ny;
                } while( ( P[k]<0.0 ) || ( P[k]>1.0 ) || ( Pay[k]<0.0 ) || ( Pay[k]>1.0 ) );
            }
            U[0] = P[0] * Pay[0];
            U[1] = P[1] * Pay[1];
            double answer = GetUtilityForAnsweringQuestion( org, P, Pay );
            fitness *= 1.0 + answer;
            if( answer == 0.0 ) {
                incorrect++;
            }
            else {
                if( U[0] > U[1] ) {
                    if( U[0] == answer )
                        correct++;
                    else
                        incorrect++;
                }
                else {
                    if( U[1] == answer )
                        correct++;
                    else
                        incorrect++;
                }
            }
        }
        org->dataMap.Set( "correct", correct );
        org->dataMap.Set( "incorrect", incorrect );
        org->dataMap.Set( "score", fitness );
    }

    org->score = fitness;
}
