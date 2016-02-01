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

#include "Genome/Genome.h"

#include "Utilities/Parameters.h"
#include "Utilities/Random.h"
#include "Utilities/Data.h"
#include "Utilities/Utilities.h"

using namespace std;

int getTemp(string key) {
	if (key == "moo") {
		return 10;
	} else if (key == "foo") {
		return 2;
	} else {
		throw 20;
	}
	return 0;
}

//class ParameterTable {
//	shared_ptr<map<string, double>> globalTable;
//	shared_ptr<map<string, double>> localTable;
//
// public:
//
//	ParameterTable() {
//		globalTable = make_shared<map<string, double>>();
//		localTable = make_shared<map<string, double>>();
//	}
//
//	ParameterTable(const ParameterTable &PT) {
//		globalTable = PT.globalTable;
//		localTable = make_shared<map<string, double>>();
//		for (auto e : *PT.localTable) {
//			localTable->insert( { e.first, e.second });
//		}
//	}
//
//	void setGlobal(string key, double value) {
//		globalTable->erase(key);
//		globalTable->insert( { key, value });
//		localTable->erase(key);
//	}
//
//	void setLocal(string key, double value) {
//		localTable->erase(key);
//		localTable->insert( { key, value });
//	}
//
//	void eraseLocal(string key) {
//		localTable->erase(key);
//	}
//
//	void eraseGlobal(string key) {
//		globalTable->erase(key);
//	}
//
//	double get(string key) {
//		double returnValue;
//		if (localTable->count(key) > 0) {
//			cout << "HERE1\n";
//			returnValue = localTable->at(key);
//		} else if (globalTable->count(key) > 0) {
//			cout << "HERE2\n";
//			returnValue = globalTable->at(key);
//		} else {
//			throw 51;
//		}
//		return returnValue;
//	}
//
//	void showTables() {
//		cout << "globalTable:\n";
//		for (auto e : *globalTable) {
//			cout << "   " << e.first << " = " << e.second << "\n";
//		}
//		cout << "localTable:\n";
//		for (auto e : *localTable) {
//			cout << "   " << e.first << " = " << e.second << "\n";
//		}
//	}
//
//};

int main(int argc, const char * argv[]) {
	auto chromosome1 = make_shared<Chromosome<double>>(10);
	chromosome1->resize(5);
	cout << chromosome1->chromosomeToStr() << "\n";
	chromosome1->fillRandom();
	cout << chromosome1->chromosomeToStr() << "\n";
	chromosome1->fillRandom(11);
	cout << chromosome1->chromosomeToStr() << "\n";

	vector<string> stats = chromosome1->getStats();
	for (auto stat : stats) {
		cout << stat << "  ";
	}
	cout << "\n\n\n";

	auto genome1 = make_shared<Genome>(make_shared<Chromosome<int>>(10), 3, 5);
	//shared_ptr<AbstractGenome> genome1 = make_shared<AbstractGenome>();
	cout << genome1->genomeToStr() << "\n";
	genome1->fillRandom();
	cout << genome1->genomeToStr() << "\n";

	auto GH = genome1->newHandler(genome1);
	genome1->getStats();
	while (!GH->atEOG()) {
		cout << GH->readInt(0, 99) << "\n";
	}
	cout << "\n";

	auto GH2 = genome1->newHandler(genome1);
	double X;
	int siteIndex = 0;
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << " A\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, false, 0, 12);
	cout << X << " B\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 111120, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, true, 0, 12);
	cout << X << "\n";

	siteIndex = 2;
	genome1->chromosomes[1]->writeDouble(siteIndex, 5.6, 4.6, 6.6,true);
	siteIndex = 2;
	genome1->chromosomes[1]->siteToDouble(siteIndex, X, 0, 10, 0, 12,true);
	cout << X << "\n";

	double A = 20;
	cout << A << "\n";
	string key = "baa";

	try {
		cout << "key: " << key << " \n->";
		cout << getTemp(key) << "\n";
	} catch (int e) {
		cout << "An exception occurred. Exception Nr. " << e << '\n';
		A = 10;
	}
	cout << "\n" << A << "\n";

	ParameterTable PT;
	cout << "111\n";

	PT.setGlobal("moo", 1.0);
	cout << "222\n";
	PT.setLocal("cow", 2.0);

	cout << "AAA\n";
	cout << PT.get("moo") << "\n";
	cout << PT.get("cow") << "\n";
	try {
		cout << PT.get("no") << "\n";
	} catch (const int errorCode) {
		PT.setGlobal("no", 3.0);
		cout << PT.get("no") << "\n";

	}

	ParameterTable PT2(PT);

	cout << PT2.get("moo") << "\n";
	cout << PT2.get("cow") << "\n";
	cout << PT2.get("no") << "\n";

	PT.showTables();
	PT2.showTables();

	int test = 990;

	cout << "------\n";
	PT2.setGlobal("heck", 0);
	PT2.setGlobal("no", 10000);
	PT.setLocal("cow", 100.0);
	PT2.setLocal("cow", 45.0);
	PT.setLocal("test", test);
	cout << PT2.get("moo") << "\n";
	PT2.setLocal("moo", 88787);

	cout << PT2.get("moo") << "\n";

	PT.showTables();
	PT2.showTables();


	genome1->chromosomes[0]->getSegment(5,5);
	genome1->chromosomes[0]->getSegment(1,1);
	genome1->chromosomes[0]->getSegment(1,1);
	genome1->chromosomes[0]->getSegment(1,1);
	genome1->chromosomes[0]->getSegment(1,1);
	genome1->chromosomes[0]->getSegment(1,1);
	genome1->chromosomes[0]->getSegment(1,1);
	auto chromosome2 = genome1->chromosomes[0]->getSegment(1,3);

	genome1->chromosomes[1]->insertSegment(chromosome2);
	genome1->chromosomes[1]->insertSegment(chromosome2);
	genome1->chromosomes[1]->insertSegment(chromosome2);
	genome1->chromosomes[1]->insertSegment(chromosome2);


	genome1->chromosomes[1]->mutatePoint();
	genome1->chromosomes[1]->mutateCopy(2,4);
	genome1->chromosomes[1]->mutateDelete(2,4);


	return 0;
}

