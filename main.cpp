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
#include "Archivist/SnapshotArchivist.h"
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
	FileManager::outputDirectory = *Global::outputDirectory;

	if (*Global::randomSeed == -1) {
		random_device rd;
		int temp = rd();
		Random::getCommonGenerator().seed(temp);
		cout << "Generating Random Seed\n  " << temp << endl;
	} else {
		Random::getCommonGenerator().seed(*Global::randomSeed);
		cout << "Using Random Seed: " << *Global::randomSeed << endl;
	}

	auto world = make_shared<BerryWorld>();

	//auto world = make_shared<NumeralClassifierWorld>();

	//auto world = make_shared<TestWorld>();

	shared_ptr<Group> group;
	{

		Global::update = -1;  // before there was time, there was a progenitor

		//auto templateGenome = make_shared<CircularGenome<double>>(CircularGenomeParameters::initialGenomeSize,256);
		shared_ptr<AbstractGenome> templateGenome;
		if (*AbstractGenome::genomeTypeStr == "Multi") {
			shared_ptr<AbstractChromosome> templateChromosome;
			if (*AbstractGenome::genomeSitesType == "char") {
				templateChromosome = make_shared<TemplatedChromosome<unsigned char>>(*MultiGenome::initialChromosomeSize, *AbstractGenome::alphabetSize);
			} else if (*AbstractGenome::genomeSitesType == "int") {
				templateChromosome = make_shared<TemplatedChromosome<int>>(*MultiGenome::initialChromosomeSize, *AbstractGenome::alphabetSize);
			} else if (*AbstractGenome::genomeSitesType == "double") {
				templateChromosome = make_shared<TemplatedChromosome<double>>(*MultiGenome::initialChromosomeSize, *AbstractGenome::alphabetSize);
			} else if (*AbstractGenome::genomeSitesType == "bool") {
				templateChromosome = make_shared<TemplatedChromosome<bool>>(*MultiGenome::initialChromosomeSize, *AbstractGenome::alphabetSize);
			} else {
				cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << *AbstractGenome::genomeSitesType << "\" is not defined.\n\nExiting.\n" << endl;
				exit(1);
			}
			templateGenome = make_shared<MultiGenome>(templateChromosome, *MultiGenome::initialChromosomes, *MultiGenome::initialPloidy);
		} else if (*AbstractGenome::genomeTypeStr == "Circular") {
			if (*AbstractGenome::genomeSitesType == "char") {
				templateGenome = make_shared<CircularGenome<unsigned char>>(*CircularGenomeParameters::initialGenomeSize, *AbstractGenome::alphabetSize);
			} else if (*AbstractGenome::genomeSitesType == "int") {
				templateGenome = make_shared<CircularGenome<int>>(*CircularGenomeParameters::initialGenomeSize, *AbstractGenome::alphabetSize);
			} else if (*AbstractGenome::genomeSitesType == "double") {
				templateGenome = make_shared<CircularGenome<double>>(*CircularGenomeParameters::initialGenomeSize, *AbstractGenome::alphabetSize);
			} else if (*AbstractGenome::genomeSitesType == "bool") {
				templateGenome = make_shared<CircularGenome<bool>>(*CircularGenomeParameters::initialGenomeSize, *AbstractGenome::alphabetSize);
			} else {
				cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << *AbstractGenome::genomeSitesType << "\" is not defined.\n\nExiting.\n" << endl;
				exit(1);
			}
		} else {
			cout << "\n\nERROR: Unrecognized genome type in configuration!\n  \"" << *AbstractGenome::genomeTypeStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		shared_ptr<AbstractBrain> templateBrain;
		if (*AbstractBrain::brainTypeStr == "Markov") {
			templateBrain = make_shared<MarkovBrain>(make_shared<ClassicGateListBuilder>(), world->requiredInputs(), world->requiredOutputs(), *AbstractBrain::hiddenNodes);
		} else if (*AbstractBrain::brainTypeStr == "Wire") {
			templateBrain = make_shared<WireBrain>(world->requiredInputs(), world->requiredOutputs(), *AbstractBrain::hiddenNodes);
		} else if (*AbstractBrain::brainTypeStr == "Human") {
			templateBrain = make_shared<HumanBrain>(world->requiredInputs(), world->requiredOutputs(), *AbstractBrain::hiddenNodes);
		} else {
			cout << "\n\nERROR: Unrecognized brain type in configuration!\n  \"" << *AbstractBrain::brainTypeStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenome, templateBrain);  // make a organism with a genome and brain - progenitor serves as an ancestor to all and a template organism

		Global::update = 0;  // the beginning of time - now we construct the first population

		vector<shared_ptr<Organism>> population;

		for (int i = 0; i < *Global::popSize; i++) {
			auto newGenome = templateGenome->makeLike();
			templateBrain->initalizeGenome(newGenome);  // use progenitors brain to prepare genome (add start codons, change ratio of site values, etc)
			auto newOrg = make_shared<Organism>(progenitor, newGenome);
			newOrg->gender = Random::getInt(0, 1);  // assign a random gender to the new org
			population.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}
		progenitor->kill();  // the progenitor has served it's purpose.
		cout << "Population of " << *Global::popSize << " organisms with " << *AbstractGenome::genomeTypeStr << "<" << *AbstractGenome::genomeSitesType << "> genomes and " << *AbstractBrain::brainTypeStr << " brains." << endl;

/////// to test genome to brain conversion and coding regions, set popsize = 1 and uncomment the block below this comment
//		shared_ptr<Organism> test_org = dynamic_pointer_cast<Organism>(population[0]);
//		shared_ptr<Genome> test_genome = dynamic_pointer_cast<Genome>(test_org->genome);
//		shared_ptr<MarkovBrain> test_brain = dynamic_pointer_cast<MarkovBrain>(test_org->brain);
//
//		cout << test_genome->genomeToStr();
//
//		for (int i = 0; i < 6; i++) {
//			shared_ptr<Chromosome<int>> ch = dynamic_pointer_cast<Chromosome<int>>(test_genome->chromosomes[i]);
//			cout << "chromosome " << i << endl;
//			cout << ch->chromosomeToStr() << endl;
//			cout << ch->codingRegionsToString() << endl;
//		}
//
//		cout << endl << endl;
//
//		cout << test_brain->description();
//		exit(1);
/////// to test genome to brain conversion and coding regions, set popsize = 1 and uncomment the block above this comment

		shared_ptr<AbstractOptimizer> optimizer;

		if (*AbstractOptimizer::Optimizer_MethodStr == "GA") {
			optimizer = make_shared<GAOptimizer>();
			cout << "Using GA Optimizer" << endl;
		} else if (*AbstractOptimizer::Optimizer_MethodStr == "Tournament") {
			optimizer = make_shared<TournamentOptimizer>();
			cout << "Using Tournament Optimizer" << endl;
		} else if (*AbstractOptimizer::Optimizer_MethodStr == "Tournament2") {
			optimizer = make_shared<Tournament2Optimizer>();
			cout << "Using Tournament2 Optimizer" << endl;
		} else {
			cout << "\n\nERROR: Unrecognized optimizer type in configuration!\n  \"" << *AbstractOptimizer::Optimizer_MethodStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		vector<string> aveFileColumns;
		aveFileColumns.clear();
		aveFileColumns.push_back("update");
		aveFileColumns.insert(aveFileColumns.end(), world->aveFileColumns.begin(), world->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->genome->aveFileColumns.begin(), population[0]->genome->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->brain->aveFileColumns.begin(), population[0]->brain->aveFileColumns.end());

		shared_ptr<DefaultArchivist> archivist;

		if (*DefaultArchivist::Arch_outputMethodStr == "default") {
			archivist = make_shared<DefaultArchivist>(aveFileColumns);
			cout << "Using Default Archivist" << endl;
		} else if (*DefaultArchivist::Arch_outputMethodStr == "LODwAP") {
			archivist = make_shared<LODwAPArchivist>(aveFileColumns);
			cout << "Using Line of Decent with Aggressive Pruning Archivist" << endl;
		} else if (*DefaultArchivist::Arch_outputMethodStr == "snapshot") {
			archivist = make_shared<SnapshotArchivist>(aveFileColumns);
			cout << "Using Snapshot Archivist" << endl;
		} else if (*DefaultArchivist::Arch_outputMethodStr == "SSwD") {
			archivist = make_shared<SSwDArchivist>(aveFileColumns);
			cout << "Using Snapshot with Delay Archivist" << endl;
		} else {
			cout << "\n\nERROR: Unrecognized archivist type in configuration!\n  \"" << *DefaultArchivist::Arch_outputMethodStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}
		group = make_shared<Group>(population, optimizer, archivist);
	}

//////////////////
// evolution loop
//////////////////

	if (*Global::mode == "run") {
		bool finished = false;  // when the archivist says we are done, we can stop!

		while (!finished) {
			world->evaluateFitness(group->population, false, *AbstractWorld::showOnUpdate);  // evaluate each organism in the population using a World
			//cout << "  evaluation done\n";
			finished = group->archive();  // save data, update memory and delete any unneeded data;
			//cout << "  archive done\n";
			Global::update++;
			group->optimize();  // update the population (reproduction and death)
			//cout << "  optimize done\n";
			cout << "update: " << Global::update - 1 << "   maxFitness: " << group->optimizer->maxFitness << "" << endl;
		}

		group->archive(1);  // flush any data that has not been output yet
	} else if (*Global::mode == "test") {

		vector<shared_ptr<AbstractGenome>> mg;
		group->population[0]->genome->loadGenomeFile("genome_20000.csv", mg);

		vector<shared_ptr<Organism>> testPopulation;
		for (auto g : mg) {
			auto newOrg = make_shared<Organism>(group->population[0], g);
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
		cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << *Global::mode << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}

	//if (Archivist::Arch_outputMethodStr == "LODwAP") {  // if using LODwAP, write out some info about MRCA
	//	shared_ptr<Organism> FinalMRCA = group->population[0]->getMostRecentCommonAncestor(group->population[0]);
	//	cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << endl << FinalMRCA->brain->description() << endl;
	//	//cout << "\n\n" << FinalMRCA->genome->showCodingRegions();
	//}
	return 0;
}
