//
//  main.cpp
//
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Archivist/DefaultArchivist.h"
#include "Archivist/LODwAPArchivist.h"
#include "Archivist/SSwDArchivist.h"
#include "Global.h"

#include "Brain/HumanBrain/HumanBrain.h"
#include "Brain/MarkovBrain/MarkovBrain.h"
#include "Brain/WireBrain/WireBrain.h"

#include "Genome/CircularGenome.h"
#include "Genome/MultiGenome.h"
#include "Genome/TemplatedChromosome.h"

#include "Group/Group.h"
#include "Optimizer/GAOptimizer.h"

#include "Optimizer/Tournament2Optimizer.h"
#include "Optimizer/TournamentOptimizer.h"

#include "Organism/Organism.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

#include "World/BerryWorld.h"
#include "World/TestWorld.h"
#include "World/NumeralClassifierWorld/NumeralClassifierWorld.h"

using namespace std;

int main(int argc, const char * argv[]) {

	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n" << "\tModular    Agent      Based    Evolver\n\n\n\thttp://hintzelab.msu.edu/MABE\n\n" << endl;

	Parameters::initializeParameters(argc, argv);  // loads command line and configFile values into registered parameters
												   // also writes out a config file if requested

//	shared_ptr<ParametersTable> pt = make_shared<ParametersTable>();
//
//	for (int i = 0; i < 100; i++) {
//		pt->setGlobal(to_string(i) + "_keyabcdefghijlmnopqrstuvwxyz", 202.222);
//		pt->setLocal(to_string(i) + "_key12345678901234567890", 101.111);
//	}
//
//	for (int i = 0; i < 1000000; i++) {
//		for (int j = 0; j < 100; j++) {
//
//			//double moo = Parameters::lookupDouble("GATES_FIXED_EPSILON_fixedEpsilonGate_Probability");
//			//double moo = pt->lookup("GATES_FIXED_EPSILON_fixedEpsilonGate_Probability");
//			double moo = FixedEpsilonGate::FixedEpsilonGate_Probability;
//			moo++;
//			if (i % 100000000 == 0) {
//				cout << moo << " " << flush;
//			}
//		}
//	}
//	cout << "done";
//	exit(1);

	//Gate_Builder::setupGates();  // determines which gate types will be in use.

	// outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be writen.
	FileManager::outputDirectory = Global::outputDirectoryPL->lookup();

	if (Global::randomSeedPL->lookup() == -1) {
		random_device rd;
		int temp = rd();
		Random::getCommonGenerator().seed(temp);
		cout << "Generating Random Seed\n  " << temp << endl;
	} else {
		Random::getCommonGenerator().seed(Global::randomSeedPL->lookup());
		cout << "Using Random Seed: " << Global::randomSeedPL->lookup() << endl;
	}

	auto world = make_shared<BerryWorld>();

	//auto world = make_shared<NumeralClassifierWorld>();

	//auto world = make_shared<TestWorld>();

	vector<string> groupNameSpaces;
	convertCSVListToVector(Global::groupNameSpacesPL->lookup(), groupNameSpaces);
	groupNameSpaces.push_back("");
	map<string, shared_ptr<Group>> groups;
	shared_ptr<ParametersTable> PT;

	for (auto NS : groupNameSpaces) {
		if (NS == "") {
			PT = Parameters::root;
			NS = "default";
		} else {
			PT = Parameters::root->getTable(NS);
		}

		Global::update = -1;  // before there was time, there was a progenitor

		shared_ptr<AbstractGenome> templateGenome;
		if (PT->lookupString("GENOME-type") == "Multi") {
			shared_ptr<AbstractChromosome> templateChromosome;
			if (PT->lookupString("GENOME-sitesType") == "char") {
				templateChromosome = make_shared<TemplatedChromosome<unsigned char>>(PT->lookupInt("GENOME_MULTI-chromosomeSizeInitial"), PT->lookupDouble("GENOME-alphabetSize"));
			} else if (PT->lookupString("GENOME-sitesType") == "int") {
				templateChromosome = make_shared<TemplatedChromosome<int>>(PT->lookupInt("GENOME_MULTI-chromosomeSizeInitial"), PT->lookupDouble("GENOME-alphabetSize"));
			} else if (PT->lookupString("GENOME-sitesType") == "double") {
				templateChromosome = make_shared<TemplatedChromosome<double>>(PT->lookupInt("GENOME_MULTI-chromosomeSizeInitial"), PT->lookupDouble("GENOME-alphabetSize"));
			} else if (PT->lookupString("GENOME-sitesType") == "bool") {
				templateChromosome = make_shared<TemplatedChromosome<bool>>(PT->lookupInt("GENOME_MULTI-chromosomeSizeInitial"), PT->lookupDouble("GENOME-alphabetSize"));
			} else {
				cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << PT->lookupString("GENOME-sitesType") << "\" is not defined.\n\nExiting.\n" << endl;
				exit(1);
			}
			templateGenome = make_shared<MultiGenome>(templateChromosome, PT->lookupInt("GENOME_MULTI-chromosomes"), PT->lookupInt("GENOME_MULTI-ploidy"), PT);

		} else if (PT->lookupString("GENOME-type") == "Circular") {
			if (PT->lookupString("GENOME-sitesType") == "char") {
				templateGenome = make_shared<CircularGenome<unsigned char>>(PT->lookupDouble("GENOME-alphabetSize"), PT->lookupInt("GENOME_CIRCULAR-sizeInitial"), PT);
			} else if (PT->lookupString("GENOME-sitesType") == "int") {
				templateGenome = make_shared<CircularGenome<int>>(PT->lookupDouble("GENOME-alphabetSize"), PT->lookupInt("GENOME_CIRCULAR-sizeInitial"), PT);
			} else if (PT->lookupString("GENOME-sitesType") == "double") {
				templateGenome = make_shared<CircularGenome<double>>(PT->lookupDouble("GENOME-alphabetSize"), PT->lookupInt("GENOME_CIRCULAR-sizeInitial"), PT);
			} else if (PT->lookupString("GENOME-sitesType") == "bool") {
				templateGenome = make_shared<CircularGenome<bool>>(PT->lookupDouble("GENOME-alphabetSize"), PT->lookupInt("GENOME_CIRCULAR-sizeInitial"), PT);
			} else {
				cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << PT->lookupString("GENOME-sitesType") << "\" is not defined.\n\nExiting.\n" << endl;
				exit(1);
			}

		} else {
			cout << "\n\nERROR: Unrecognized genome type in configuration!\n  \"" << PT->lookupString("GENOME-type") << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		shared_ptr<AbstractBrain> templateBrain;
		if (PT->lookupString("BRAIN-brainType") == "Markov") {
			templateBrain = make_shared<MarkovBrain>(make_shared<ClassicGateListBuilder>(PT), world->requiredInputs(), world->requiredOutputs(), PT->lookupInt("BRAIN-hiddenNodes"), PT);
		} else if (PT->lookupString("BRAIN-brainType") == "Wire") {
			templateBrain = make_shared<WireBrain>(world->requiredInputs(), world->requiredOutputs(), PT->lookupInt("BRAIN-hiddenNodes"), PT);
		} else if (PT->lookupString("BRAIN-brainType") == "Human") {
			templateBrain = make_shared<HumanBrain>(world->requiredInputs(), world->requiredOutputs(), PT->lookupInt("BRAIN-hiddenNodes"));
		} else {
			cout << "\n\nERROR: Unrecognized brain type in configuration!\n  \"" << PT->lookupString("BRAIN-brainType") << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenome, templateBrain);  // make a organism with a genome and brain - progenitor serves as an ancestor to all and a template organism

		Global::update = 0;  // the beginning of time - now we construct the first population

		vector<shared_ptr<Organism>> population;

		for (int i = 0; i < Global::popSizePL->lookup(); i++) {
			auto newGenome = templateGenome->makeLike();
			templateBrain->initalizeGenome(newGenome);  // use progenitors brain to prepare genome (add start codons, change ratio of site values, etc)
			auto newOrg = make_shared<Organism>(progenitor, newGenome);
			newOrg->gender = Random::getInt(0, 1);  // assign a random gender to the new org
			population.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}
		progenitor->kill();  // the progenitor has served it's purpose.
		cout << "The " << NS << " group is a population of " << Global::popSizePL->lookup() << " organisms with " << PT->lookupString("GENOME-type") << "<" << PT->lookupString("GENOME-sitesType") << "> genomes and " << PT->lookupString("BRAIN-brainType") << " brains." << endl;

		shared_ptr<AbstractOptimizer> optimizer;

		if (PT->lookupString("OPTIMIZER-optimizer") == "GA") {
			optimizer = make_shared<GAOptimizer>(PT);
			cout << "Using GA Optimizer" << endl;
		} else if (PT->lookupString("OPTIMIZER-optimizer") == "Tournament") {
			optimizer = make_shared<TournamentOptimizer>(PT);
			cout << "Using Tournament Optimizer" << endl;
		} else if (PT->lookupString("OPTIMIZER-optimizer") == "Tournament2") {
			optimizer = make_shared<Tournament2Optimizer>(PT);
			cout << "Using Tournament2 Optimizer" << endl;
		} else {
			cout << "\n\nERROR: Unrecognized optimizer type in configuration!\n  \"" << PT->lookupString("OPTIMIZER-optimizer") << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		vector<string> aveFileColumns;
		aveFileColumns.clear();
		aveFileColumns.push_back("update");
		aveFileColumns.insert(aveFileColumns.end(), world->aveFileColumns.begin(), world->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->genome->aveFileColumns.begin(), population[0]->genome->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->brain->aveFileColumns.begin(), population[0]->brain->aveFileColumns.end());

		shared_ptr<DefaultArchivist> archivist;

		if (PT->lookupString("ARCHIVIST-outputMethod") == "default") {
			archivist = make_shared<DefaultArchivist>(aveFileColumns, PT);
			cout << "Using Default Archivist" << endl;
		} else if (PT->lookupString("ARCHIVIST-outputMethod") == "LODwAP") {
			archivist = make_shared<LODwAPArchivist>(aveFileColumns, PT);
			cout << "Using Line of Decent with Aggressive Pruning Archivist" << endl;
		} else if (PT->lookupString("ARCHIVIST-outputMethod") == "SSwD") {
			archivist = make_shared<SSwDArchivist>(aveFileColumns, PT);
			cout << "Using Snapshot with Delay Archivist" << endl;
		} else {
			cout << "\n\nERROR: Unrecognized archivist type in configuration!\n  \"" << PT->lookupString("ARCHIVIST-outputMethod") << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		groups[NS] = make_shared<Group>(population, optimizer, archivist);
		cout << endl;
	}

//////////////////
// evolution loop
//////////////////

	string defaultGroup = "default";
	if (groups.find(defaultGroup) == groups.end()){
		cout << "Default group " << defaultGroup << " not found in groups.\nExiting."<<endl;
		exit(1);
	}

	if (Global::modePL->lookup() == "run") {
		bool finished = false;  // when the archivist says we are done, we can stop!

		while (!finished) {
			world->evaluateFitness(groups[defaultGroup]->population, false, AbstractWorld::showOnUpdatePL->lookup());  // evaluate each organism in the population using a World
			//cout << "  evaluation done\n";
			finished = groups[defaultGroup]->archive();  // save data, update memory and delete any unneeded data;
			//cout << "  archive done\n";
			Global::update++;
			groups[defaultGroup]->optimize();  // update the population (reproduction and death)
			//cout << "  optimize done\n";
			cout << "update: " << Global::update - 1 << "   maxFitness: " << groups[defaultGroup]->optimizer->maxFitness << "" << endl;
		}

		groups[defaultGroup]->archive(1);  // flush any data that has not been output yet
	} else if (Global::modePL->lookup() == "test") {

		vector<shared_ptr<AbstractGenome>> mg;
		groups[defaultGroup]->population[0]->genome->loadGenomeFile("genome_20000.csv", mg);

		vector<shared_ptr<Organism>> testPopulation;
		for (auto g : mg) {
			auto newOrg = make_shared<Organism>(groups[defaultGroup]->population[0], g);
			newOrg->brain->setRecordActivity(true);
			newOrg->brain->setRecordFileName("wireBrain.run");
			testPopulation.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}

		//shared_ptr<Group> testGroup = make_shared<Group>(testPopulation, group->optimizer, group->archivist);
		//world->worldUpdates = 400;
		world->evaluateFitness( { testPopulation[1] }, false);

		for (auto o : testPopulation) {
			cout << o->score << " " << o->genome->dataMap.Get("ID") << endl;
		}

	} else {
		cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << Global::modePL->lookup() << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}

	//if (Archivist::Arch_outputMethodStr == "LODwAP") {  // if using LODwAP, write out some info about MRCA
	//	shared_ptr<Organism> FinalMRCA = group->population[0]->getMostRecentCommonAncestor(group->population[0]);
	//	cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << endl << FinalMRCA->brain->description() << endl;
	//	//cout << "\n\n" << FinalMRCA->genome->showCodingRegions();
	//}
	return 0;
}
