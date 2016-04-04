//
//  UtilityWorld.cpp
//  MarkovBrainPWFBias
//
//  Created by Arend Hintze on 11/19/15.
//  Copyright © 2015 Arend Hintze. All rights reserved.
//

#include "UtilityWorld.h"

const double& UtilityWorld::pNoise = Parameters::register_parameter( "UTILITY_pNoise", 0.25, "delta noise on p", "WORLD - UTILITY" );
const double& UtilityWorld::piNoise = Parameters::register_parameter( "UTILITY_piNoise", 0.25, "delta noise on pi", "WORLD - UTILITY" );
const double& UtilityWorld::alpha = Parameters::register_parameter( "UTILITY_alpha", 0.0, "alpha parameter", "WORLD - UTILITY" );
const int& UtilityWorld::thinkTime = Parameters::register_parameter( "UTILITY_thinkTime", 100, "think time allowed", "WORLD - UTILITY" );
const string& UtilityWorld::DATAFileName = Parameters::register_parameter( "UTILITY_DATAFileName", ( string )"defaultDataFile.csv", "file to save the extra data into", "WORLD - UTILITY" );
const int& UtilityWorld::nrOfTests = Parameters::register_parameter( "UTILITY_nrOfTests", 100, "how many times is a brain asked to do teh decision task", "WORLD - UTILITY" );

//int DecisionMakerWorld::thinkTime=1;
//double DecisionMakerWorld::pNoise=0.25;
//double DecisionMakerWorld::piNoise=0.25;
//double DecisionMakerWorld::alpha=0.0;
//string DecisionMakerWorld::DATAFileName="";

double UtilityWorld::getPi( double p, double theAlpha ) {
    return ( -0.5*theAlpha + 0.5 ) + ( p*theAlpha );
}

UtilityWorld::UtilityWorld() {
}


double UtilityWorld::GetUtilityForAnsweringQuestion( shared_ptr<Organism> org, double P[2], double Pay[2] ) {
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

double UtilityWorld::testIndividual( shared_ptr<Organism> org, bool analyse, bool show ) {
    auto& agent = org->brain;
    double fitness = 1.0;
    int correct = 0;
    int incorrect = 0;
    double fixedP[10] = { 0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95 };
    if( analyse ) {
        FILE *F = fopen( DATAFileName.c_str(), "w+t" );
        fprintf( F, "p1,pay1,p2,pay2,U1,U2,C1,C2,undec\n" );
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
                if( answer == 0.0 ) {
                    if( U[0] < U[1] ) {
                        fprintf( F, "%f,%f,%f,%f,%f,%f,1,0,1\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                    }
                    else {
                        fprintf( F, "%f,%f,%f,%f,%f,%f,0,1,1\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                    }
                }
                else {
                    if( U[0] == answer ) {
                        fprintf( F, "%f,%f,%f,%f,%f,%f,1,0,0\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                    }
                    else {
                        fprintf( F, "%f,%f,%f,%f,%f,%f,0,1,0\n", P[0], Pay[0], P[1], Pay[1], U[0], U[1] );
                    }
                }
            }
        }
        fclose( F );
    }
    else {
        // normal evolution
        for( int i = 0; i < nrOfTests; i++ ) {
            agent->resetBrain();
            double P[2], Pay[2], U[2];
            for( int l = 0; l < 2; l++ ) {
                double nx, ny;
                //this creates random variables from the defined distribution
                //this needs to become the predefined question matrix
                do {
                    nx = ( Random::getDouble( 1.0 )*pNoise ) - ( pNoise / 2.0 );
                    ny = ( Random::getDouble( 1.0 )*piNoise ) - ( piNoise / 2.0 );
                    P[l] = Random::getDouble( 1.0 ) + nx;
                    Pay[l] = getPi( P[l], alpha ) + ny;
                } while( ( P[l]<0.0 ) || ( P[l]>1.0 ) || ( Pay[l]<0.0 ) || ( Pay[l]>1.0 ) );
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
    return fitness;
}
