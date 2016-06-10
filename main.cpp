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

#include "Group/Group.h"

#include "Organism/Organism.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

#include "modules.h"
using namespace std;

int main(int argc, const char * argv[]) {

	cout << "\n\n" << "\tMM   MM      A       BBBBBB    EEEEEE\n" << "\tMMM MMM     AAA      BB   BB   EE\n" << "\tMMMMMMM    AA AA     BBBBBB    EEEEEE\n" << "\tMM M MM   AAAAAAA    BB   BB   EE\n" << "\tMM   MM  AA     AA   BBBBBB    EEEEEE\n" << "\n" << "\tModular    Agent      Based    Evolver\n\n\n\thttp://hintzelab.msu.edu/MABE\n\n" << endl;

	cout << "\tfor help run MABE with the \"-h\" flag (i.e. ./MABE -h)." << endl << endl;
	Parameters::initializeParameters(argc, argv);  // loads command line and configFile values into registered parameters
												   // also writes out a config file if requested


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


	//auto world = make_shared<NumeralClassifierWorld>();
	auto world = makeWorld();

	vector<string> groupNameSpaces;
	convertCSVListToVector(Global::groupNameSpacesPL->lookup(), groupNameSpaces);
	groupNameSpaces.push_back("");
	map<string, shared_ptr<Group>> groups;
	shared_ptr<ParametersTable> PT;

	for (auto NS : groupNameSpaces) {
		if (NS == "") {
			PT = nullptr;//Parameters::root;
			NS = "default";
		} else {
			PT = Parameters::root->getTable(NS);
		}

		Global::update = -1;  // before there was time, there was a progenitor

		shared_ptr<AbstractGenome> templateGenome = makeTemplateGenome(PT);
		shared_ptr<AbstractBrain> templateBrain = makeTemplateBrain(world, PT);
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

		if (PT == nullptr){
			PT = Parameters::root;
		}
		cout << "The " << NS << " group is a population of " << Global::popSizePL->lookup() << " organisms with " << PT->lookupString("GENOME-genomeType") << "<" << PT->lookupString("GENOME-sitesType") << "> genomes and " << PT->lookupString("BRAIN-brainType") << " brains." << endl;

		shared_ptr<AbstractOptimizer> optimizer = makeOptimizer(PT);

		vector<string> aveFileColumns;
		aveFileColumns.clear();
		aveFileColumns.push_back("update");
		aveFileColumns.insert(aveFileColumns.end(), world->aveFileColumns.begin(), world->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->genome->aveFileColumns.begin(), population[0]->genome->aveFileColumns.end());
		aveFileColumns.insert(aveFileColumns.end(), population[0]->brain->aveFileColumns.begin(), population[0]->brain->aveFileColumns.end());

		shared_ptr<DefaultArchivist> archivist = makeArchivist(aveFileColumns, PT);

		groups[NS] = make_shared<Group>(population, optimizer, archivist);
		cout << endl;
	}

//////////////////
// evolution loop
//////////////////

	string defaultGroup = "default";
	if (groups.find(defaultGroup) == groups.end()) {
		cout << "Group " << defaultGroup << " not found in groups.\nExiting." << endl;
		exit(1);
	}

	if (Global::modePL->lookup() == "run") {
		bool finished = false;  // when the archivist says we are done, we can stop!

		while (!finished) {
			world->evaluate(groups[defaultGroup], AbstractWorld::groupEvaluationPL->lookup(), false, false, AbstractWorld::debugPL->lookup());  // evaluate each organism in the population using a World
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
		groups[defaultGroup]->population[0]->genome->loadGenomeFile(Global::visualizePopulationFilePL->lookup(), mg);
		cout << "file loaded";
		int num_genomes = (int) mg.size();
		int counter = 0;
		while ((int) mg.size() < Global::popSizePL->lookup()) {
			cout << ".";
			mg.push_back(mg[counter]);
			counter++;
			if (counter >= num_genomes) {
				counter = 0;
			}
		}
		vector<shared_ptr<Organism>> testPopulation;
		for (auto g : mg) {
			auto newOrg = make_shared<Organism>(groups[defaultGroup]->population[0], g);
			//newOrg->brain->setRecordActivity(true);
			//newOrg->brain->setRecordFileName("wireBrain.run");
			testPopulation.push_back(newOrg);  // add a new org to population using progenitors template and a new random genome
		}
		cout << "\npopulation created." << endl;

//////////////////////////////////////////////
// rebuild testGroup with one particular org.
		testPopulation.clear();
		auto newOrg = make_shared<Organism>(groups[defaultGroup]->population[0], mg[0]);
		testPopulation.push_back(newOrg);
//////////////////////////////////////////////

		shared_ptr<Group> testGroup = make_shared<Group>(testPopulation, groups[defaultGroup]->optimizer, groups[defaultGroup]->archivist);
		cout << "\ngroup created." << endl;
		world->runWorld(testGroup, false, true, false);

		for (auto o : testGroup->population) {
			cout << o->score << " " << o->genome->dataMap.Get("ID") << endl;
		}

	} else {
		cout << "\n\nERROR: Unrecognized mode set in configuration!\n  \"" << Global::modePL->lookup() << "\" is not defined.\n\nExiting.\n" << endl;
		exit(1);
	}

	return 0;
}
