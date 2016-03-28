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

#include "Global.h"

#include "Archivist/Archivist.h"
#include "Archivist/LODwAP_Archivist.h"
#include "Archivist/snapshot_Archivist.h"
#include "Archivist/SSwD_Archivist.h"
#include "Brain/MarkovBrain.h"
#include "Brain/WireBrain.h"

#include "Genome/Genome.h"
#include "Genome/MultiGenome.h"
#include "Genome/CircularGenome.h"

#include "GateListBuilder/GateListBuilder.h"

#include "Group/Group.h"
#include "Optimizer/GA_Optimizer.h"
#include "Optimizer/Optimizer.h"
#include "Optimizer/Tournament2Optimizer.h"
#include "Optimizer/TournamentOptimizer.h"
#include "Organism/Organism.h"
#include "World/World.h"
#include "World/BerryWorld.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

using namespace std;

int main(int argc, const char * argv[]) {

	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n" << "\tModular    Agent      Based    Evolver\n\n\n\thttp://hintzelab.msu.edu/MABE\n\n" << endl;
	Parameters::initialize_parameters(argc, argv);  // loads command line and configFile values into registered parameters
													// also writes out a config file if requested
	Gate_Builder::setupGates();  // determines which gate types will be in use.

	// outputDirectory must exist. If outputDirectory does not exist, no error will occur, but no data will be writen.
	FileManager::outputDirectory = Global::outputDirectory;

	if (Global::randomSeed == -1) {
		random_device rd;
		int temp = rd();
		Random::getCommonGenerator().seed(temp);
		cout << "Generating Random Seed\n  " << temp << endl;
	} else {
		Random::getCommonGenerator().seed(Global::randomSeed);
		cout << "Using Random Seed: " << Global::randomSeed << endl;
	}

	auto world = make_shared<BerryWorld>();  //new World();
	//World *world = (World*) new World();  //new World();

// test chromosome crossover speed
//	auto C1 = make_shared<Chromosome<bool>>(10000, 2);
//	C1->fillRandom();
//	auto C2 = make_shared<Chromosome<bool>>(10000, 2);
//	C2->fillRandom();
//	auto C3 = make_shared<Chromosome<bool>>(0, 2);
//
//	for (int k = 0; k < 100; k++) {
//		for (int j = 0; j < 500; j++) {
//			for (int i = 0; i < 6; i++) {
//				//cout << "." << flush;
//				C3->crossover( { C1, C2 }, 3);
//				//cout << "*" << flush;
//			}
//			cout << "--" << flush;
//		}
//		cout << k << endl;
//	}
//
//	exit(0);

	// test other speed

//	auto testChromosome = make_shared<Chromosome<bool>>(5000, 2);
//	auto testGenome = make_shared<Genome>(testChromosome, 3, 2);
//	auto GLB = make_shared<Classic_GateListBuilder>();  // make a organism with a genome and brain (if you need to change the types here is where you do it)
//	shared_ptr<Organism> testOrg = make_shared<Organism>(make_shared<Genome>(testChromosome, 3, 2), make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>()));  // make a organism with a genome and brain (if you need to change the types here is where you do it)
//
//	for (int k = 0; k < 100; k++) {
//		for (int j = 0; j < 500; j++) {
//			//GLB->buildGateList(testGenome, 16);
//			//auto testMB = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>());
//			auto testGenomeMany = make_shared<Genome>(testChromosome, 3, 2);
//			//auto testOrgMany = make_shared<Organism>(testOrg, testGenome);
//			cout << "*." << flush;
//		}
//		cout << k << endl;
//	}
//
//	exit(0);

////  ///// to show org in world
//  shared_ptr<Genome> testGenome = make_shared<Genome>();
//  testGenome->loadSites("genome.csv",1000);
//  shared_ptr<Organism> testOrg = make_shared<Organism>(testGenome, make_shared<Brain>());
//  world->testIndividual(testOrg,0,1);
//  exit(0);
////  ///// end to show org in world

	//////////////////
	// define population
	//////////////////

//	///////////////////////// test reading a writing to genomes ///////////////////////////////////////////
// for mutigenome...
//	auto initalChromosome = make_shared<Chromosome<int>>(200, 10);
//	auto initalGenome = make_shared<MultiGenome>(initalChromosome, 2, 1);
//	auto initalGenome2 = make_shared<MultiGenome>(initalChromosome, 2, 1);
// for circularGenome:
//	auto initalGenome = make_shared<CircularGenome<int>>(100, 10);
//	auto initalGenome2 = make_shared<CircularGenome<int>>(100, 10);
//	shared_ptr<AbstractGenome> initalGenome3;
//	auto initalBrain = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>(), 2, 3, 4);
//	initalGenome->fillAcending();
//	initalGenome->printGenome();
//
//	auto handler = initalGenome->newHandler(initalGenome, 1);
//
//	handler->writeInt(5, 0, 9);
//	cout << "\n";
//	initalGenome->printGenome();
//	handler->writeInt(89, 0, 99);
//	cout << "\n";
//	initalGenome->printGenome();
//	handler->setReadDirection(0);
//	handler->writeInt(54, 0, 99);
//	cout << "\n";
//	initalGenome->printGenome();
//	handler->writeInt(321, 0, 999);
//	initalGenome->printGenome();
//	cout << "\n";
//	handler->writeInt(321, 0, 999);
//	initalGenome->printGenome();
//	cout << "\n";
//
//	handler->setReadDirection(1);
//
//	handler->writeInt(987, 0, 999);
//	initalGenome->printGenome();
//	cout << "\n";
//
//	handler->writeInt(987, 0, 999);
//	initalGenome->printGenome();
//	cout << "\n";
//
//	handler->writeInt(987, 0, 999);
//	initalGenome->printGenome();
//	cout << "\n\n\n";
//
//
//	handler->resetHandler();
//	initalGenome->printGenome();
//
//	cout << "\n\n\n\n";
//	cout << handler->readInt(182, 255) << endl;
//	cout << handler->readInt(7, 25) << endl;
//	cout << handler->readInt(2, 8) << endl;
//	cout << handler->readInt(1, 9) << endl;
//
//	initalGenome->printGenome();
//
//	initalGenome->fillAcending();
//	initalGenome2->fillConstant(5);
//	initalGenome->printGenome();
//	initalGenome2->printGenome();
//
//	initalGenome3 = initalGenome->makeMutatedGenomeFromMany( { initalGenome, initalGenome2 });
//	initalGenome3->printGenome();
//
//	exit(1);
//	///////////////////////// end test reading a writing to genomes ///////////////////////////////////////////

//	///////////////////////// test genome to gate translation /////////////////////////////////////////////////
//	auto initalChromosome = make_shared<Chromosome<int>>(400, 2);
//	auto initalGenome = make_shared<Genome>(initalChromosome, 2, 2);
//	auto initalGenome = make_shared<CircularGenome<int>>(1000,256);
//	auto initalBrain = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>(),2,3,4);
//	int testStartCode = 43;
//	initalGenome->fillAcending();
//	initalGenome->printGenome();
//
//	auto handler = initalGenome->newHandler(initalGenome,1);
//
//	handler->writeInt(200,0,255);
//	handler->writeInt(201,0,255);
//	handler->writeInt(testStartCode,0,255);
//	handler->writeInt(255-testStartCode,0,255);
//
//	handler->writeInt(4,1,4); // 4 in
//	handler->writeInt(4,1,4); // 4 out
//
//	handler->writeInt(3,0,255); // in 1
//	handler->writeInt(2,0,255); // in 2
//	handler->writeInt(1,0,255); // ..
//	handler->writeInt(0,0,255); // ..
//	handler->writeInt(2,0,255); // out 1
//	handler->writeInt(4,0,255); // ..
//	handler->writeInt(6,0,255); // ..
//	handler->writeInt(8,0,255); // ..
//
//	handler->advanceIndex(180);
//	handler->writeInt(testStartCode,0,255);
//	handler->writeInt(255-testStartCode,0,255);
//	handler->writeInt(1,1,4); // 1 in
//	handler->writeInt(1,1,4); // 1 out
//
//	handler->advanceIndex(2);
//	handler->writeInt(testStartCode,0,255);
//	handler->writeInt(255-testStartCode,0,255);
//	handler->writeInt(4,1,4); // 4 in
//	handler->writeInt(4,1,4); // 4 out
//
//	handler->writeInt(7,0,255); // in 1
//	handler->writeInt(6,0,255); // in 2
//	handler->writeInt(5,0,255); // ..
//	handler->writeInt(4,0,255); // ..
//	handler->writeInt(4,0,255); // out 1
//	handler->writeInt(5,0,255); // ..
//	handler->writeInt(6,0,255); // ..
//	handler->writeInt(7,0,255); // ..
//
//	handler->advanceIndex(100);
//	handler->writeInt(testStartCode,0,255);
//	handler->writeInt(255-testStartCode,0,255);
//	handler->writeInt(4,1,4); // 4 in
//	handler->writeInt(4,1,4); // 4 out
//
//	handler->writeInt(1,0,255); // in 1
//	handler->writeInt(3,0,255); // in 2
//	handler->writeInt(5,0,255); // ..
//	handler->writeInt(7,0,255); // ..
//	handler->writeInt(5,0,255); // out 1
//	handler->writeInt(3,0,255); // ..
//	handler->writeInt(1,0,255); // ..
//	handler->writeInt(9,0,255); // ..
//
//	initalGenome->printGenome();
//		shared_ptr<Organism> testSubject = make_shared<Organism>(initalGenome, initalBrain);  // make a organism with a genome and brain (if you need to change the types here is where you do it)
//
//	cout << "brain" << endl;
//	cout << testSubject->brain->description() << endl;
//
//	cout << "coding regions" << endl;
//	auto test_genome = dynamic_pointer_cast<Genome>(testSubject->genome);
//	for (auto c : test_genome->chromosomes){
//		auto convertedC = dynamic_pointer_cast<Chromosome<int>>(c);
//		cout << convertedC->codingRegionsToString() << endl<< endl;
//	}
//	exit (100);
//	///////////////////////// test end ///////////////////////////////////////////

	shared_ptr<Group> group;
	{

		Global::update = -1;  // before there was time, there was a progenitor

		//auto templateGenome = make_shared<CircularGenome<double>>(CircularGenomeParameters::initialGenomeSize,256);
		shared_ptr<AbstractGenome> templateGenome;
		if (AbstractGenome::genomeTypeStr == "Multi") {
			shared_ptr<AbstractChromosome> templateChromosome;
			if (AbstractGenome::genomeSitesType == "char") {
				templateChromosome = make_shared<Chromosome<unsigned char>>(MultiGenome::initialChromosomeSize, AbstractGenome::alphabetSize);
			} else if (AbstractGenome::genomeSitesType == "int") {
				templateChromosome = make_shared<Chromosome<int>>(MultiGenome::initialChromosomeSize, AbstractGenome::alphabetSize);
			} else if (AbstractGenome::genomeSitesType == "double") {
				templateChromosome = make_shared<Chromosome<double>>(MultiGenome::initialChromosomeSize, AbstractGenome::alphabetSize);
			} else if (AbstractGenome::genomeSitesType == "bool") {
				templateChromosome = make_shared<Chromosome<bool>>(MultiGenome::initialChromosomeSize, AbstractGenome::alphabetSize);
			} else {
				cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << AbstractGenome::genomeSitesType << "\" is not defined.\n\nExiting.\n" << endl;
				exit(1);
			}
			templateGenome = make_shared<MultiGenome>(templateChromosome, MultiGenome::initialChromosomes, MultiGenome::initialPloidy);
		} else if (AbstractGenome::genomeTypeStr == "Circular") {
			if (AbstractGenome::genomeSitesType == "char") {
				templateGenome = make_shared<CircularGenome<unsigned char>>(CircularGenomeParameters::initialGenomeSize, AbstractGenome::alphabetSize);
			} else if (AbstractGenome::genomeSitesType == "int") {
				templateGenome = make_shared<CircularGenome<int>>(CircularGenomeParameters::initialGenomeSize, AbstractGenome::alphabetSize);
			} else if (AbstractGenome::genomeSitesType == "double") {
				templateGenome = make_shared<CircularGenome<double>>(CircularGenomeParameters::initialGenomeSize, AbstractGenome::alphabetSize);
			} else if (AbstractGenome::genomeSitesType == "bool") {
				templateGenome = make_shared<CircularGenome<bool>>(CircularGenomeParameters::initialGenomeSize, AbstractGenome::alphabetSize);
			} else {
				cout << "\n\nERROR: Unrecognized genomeSitesType in configuration!\n  \"" << AbstractGenome::genomeSitesType << "\" is not defined.\n\nExiting.\n" << endl;
				exit(1);
			}
		} else {
			cout << "\n\nERROR: Unrecognized genome type in configuration!\n  \"" << AbstractGenome::genomeTypeStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		shared_ptr<AbstractBrain> templateBrain;
		if (AbstractBrain::brainTypeStr == "Markov") {
			templateBrain = make_shared<MarkovBrain>(make_shared<Classic_GateListBuilder>(), world->requiredInputs(), world->requiredOutputs(), AbstractBrain::hiddenNodes);
		} else if (AbstractBrain::brainTypeStr == "Wire") {
			templateBrain = make_shared<WireBrain>(world->requiredInputs(), world->requiredOutputs(), AbstractBrain::hiddenNodes);
		} else {
			cout << "\n\nERROR: Unrecognized brain type in configuration!\n  \"" << AbstractBrain::brainTypeStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		shared_ptr<Organism> progenitor = make_shared<Organism>(templateGenome, templateBrain);  // make a organism with a genome and brain - progenitor serves as an ancestor to all and a template organism

		Global::update = 0;  // the beginning of time - now we construct the first population

		vector<shared_ptr<Organism>> population;

		for (int i = 0; i < Global::popSize; i++) {
			auto newGenome = templateGenome->makeLike();
			templateBrain->initalizeGenome(newGenome);  // use progenitors brain to prepare genome (add start codons, change ratio of site values, etc)
			auto newOrg = make_shared<Organism>(progenitor, newGenome);
			newOrg->gender = Random::getInt(0, 1);  // assign a random gender to the new org
			population.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}
		progenitor->kill();  // the progenitor has served it's purpose.
		cout << "Population of " << Global::popSize << " organisms with " << AbstractGenome::genomeTypeStr << "<" << AbstractGenome::genomeSitesType << "> genomes and " << AbstractBrain::brainTypeStr << " brains." << endl;

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

		shared_ptr<BaseOptimizer> optimizer;

		if (BaseOptimizer::Optimizer_MethodStr == "GA") {
			optimizer = make_shared<GA_Optimizer>();
			cout << "Using GA Optimizer" << endl;
		} else if (BaseOptimizer::Optimizer_MethodStr == "Tournament") {
			optimizer = make_shared<TournamentOptimizer>();
			cout << "Using Tournament Optimizer" << endl;
		} else if (BaseOptimizer::Optimizer_MethodStr == "Tournament2") {
			optimizer = make_shared<Tournament2Optimizer>();
			cout << "Using Tournament2 Optimizer" << endl;
		} else {
			cout << "\n\nERROR: Unrecognized optimizer type in configuration!\n  \"" << BaseOptimizer::Optimizer_MethodStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		vector<string> aveFileColumns;
		aveFileColumns.clear();
		aveFileColumns.push_back("update");
		aveFileColumns.insert(aveFileColumns.end(), world->aveFileColumns.begin(), world->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->genome->aveFileColumns.begin(), population[0]->genome->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->brain->aveFileColumns.begin(), population[0]->brain->aveFileColumns.end());

		shared_ptr<Archivist> archivist;

		if (Archivist::Arch_outputMethodStr == "default") {
			archivist = make_shared<Archivist>(aveFileColumns);
			cout << "Using Default Archivist" << endl;
		} else if (Archivist::Arch_outputMethodStr == "LODwAP") {
			archivist = make_shared<LODwAP_Archivist>(aveFileColumns);
			cout << "Using Line of Decent with Aggressive Pruning Archivist" << endl;
		} else if (Archivist::Arch_outputMethodStr == "snapshot") {
			archivist = make_shared<Snapshot_Archivist>(aveFileColumns);
			cout << "Using Snapshot Archivist" << endl;
		} else if (Archivist::Arch_outputMethodStr == "SSwD") {
			archivist = make_shared<SSwD_Archivist>(aveFileColumns);
			cout << "Using Snapshot with Delay Archivist" << endl;
		} else {
			cout << "\n\nERROR: Unrecognized archivist type in configuration!\n  \"" << Archivist::Arch_outputMethodStr << "\" is not defined.\n\nExiting.\n" << endl;
			exit(1);
		}

		group = make_shared<Group>(population, optimizer, archivist);
	}

//////////////////
// evolution loop
//////////////////

	if (Global::mode == "run") {
		bool finished = false;  // when the archivist says we are done, we can stop!

		while (!finished) {
			world->evaluateFitness(group->population, false);  // evaluate each organism in the population using a World
			//cout << "  evaluation done\n";
			finished = group->archive();  // save data, update memory and delete any unneeded data;
			//cout << "  archive done\n";
			Global::update++;
			group->optimize();  // update the population (reproduction and death)
			//cout << "  optimize done\n";
			cout << "update: " << Global::update - 1 << "   maxFitness: " << group->optimizer->maxFitness << "" << endl;
		}

		group->archive(1);  // flush any data that has not been output yet
	} else if (Global::mode == "test") {

		vector<shared_ptr<AbstractGenome>> mg;
		group->population[0]->genome->loadGenomeFile("genome_20000.csv", mg);

		vector<shared_ptr<Organism>> testPopulation;
		for (auto g : mg){
			auto newOrg = make_shared<Organism>(group->population[0], g);
			newOrg->brain->setRecordActivity(true);
			newOrg->brain->setRecordFileName("wireBrain.run");
			testPopulation.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}

		//shared_ptr<Group> testGroup = make_shared<Group>(testPopulation, group->optimizer, group->archivist);
		world->worldUpdates = 400;
		world->evaluateFitness({testPopulation[1]},false);

		for (auto o : testPopulation){
			cout << o->score << " " << o->genome->dataMap.Get("ID") << endl;
		}

	} else {
		cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << Global::mode << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}

	//if (Archivist::Arch_outputMethodStr == "LODwAP") {  // if using LODwAP, write out some info about MRCA
	//	shared_ptr<Organism> FinalMRCA = group->population[0]->getMostRecentCommonAncestor(group->population[0]);
	//	cout << "MRCA - ID: " << FinalMRCA->ID << " born on: " << FinalMRCA->timeOfBirth << endl << FinalMRCA->brain->description() << endl;
	//	//cout << "\n\n" << FinalMRCA->genome->showCodingRegions();
	//}
	return 0;
}
