/*
 * Utilities.h
 *
 *  Created on: Oct 6, 2015
 *      Author: cliff
 */

#ifndef __BasicMarkovBrainTemplate__ParametersTable__
#define __BasicMarkovBrainTemplate__ParametersTable__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;
// Parameters tables is used to store experimental parameters.
// The class contains shared pointers to 2 maps<string,double> where:
// the string is a look up key and the double is th value
// i.e. pointMutationRate, .05
// Generally speaking, there will be many instances of ParameterTables where they share
// the globalTable (via a shared_ptr), but each have their own localTable.
// i.e. pointMutationRate is set in globalTable to .05, but in one ParameterTables localTable it is .075
// When the get method is called, ParameterTable attempts to return a value from the localTable, if the
// key is not in the local table, it will attempt to return a value from the globalTable. If the key does
// not exist in either local or global tables, then an exception is thrown.

class ParameterTable {
	shared_ptr<map<string, double>> globalTable;
	shared_ptr<map<string, double>> localTable;

 public:

	ParameterTable() {
		globalTable = make_shared<map<string, double>>();
		localTable = make_shared<map<string, double>>();
	}

	ParameterTable(const ParameterTable &PT) {
		globalTable = PT.globalTable;
		localTable = make_shared<map<string, double>>();
		for (auto e : *PT.localTable) {
			localTable->insert( { e.first, e.second });
		}
	}

	void setGlobal(string key, double value) {
		globalTable->erase(key);
		globalTable->insert( { key, value });
		localTable->erase(key);
	}

	// add
	void setLocal(string key, double value) {
		localTable->erase(key);
		localTable->insert( { key, value });
	}

	// remove a key / value from the global table (the global value will now be used)
	void eraseLocal(string key) {
		localTable->erase(key);
	}

	// remove a key / value from the global table (the may be part of forcing this value to be reset to default)
	void eraseGlobal(string key) {
		globalTable->erase(key);
	}

	// read a value from the ParameterTable. If the key is in the local table, use that. If not,
	// use the global table. If neither table contains the key, throw an exception.
	double get(string key) {
		double returnValue;
		if (localTable->count(key) > 0) {
			returnValue = localTable->at(key);
		} else if (globalTable->count(key) > 0) {
			returnValue = globalTable->at(key);
		} else {
			throw 51;
		}
		return returnValue;
	}

	// display all contents of global and local tables
	// useful for debugging!
	void showTables() {
		cout << "globalTable:\n";
		for (auto e : *globalTable) {
			cout << "   " << e.first << " = " << e.second << "\n";
		}
		cout << "localTable:\n";
		for (auto e : *localTable) {
			cout << "   " << e.first << " = " << e.second << "\n";
		}
	}

};

#endif // __BasicMarkovBrainTemplate__ParametersTable__
