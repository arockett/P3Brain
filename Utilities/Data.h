//
//  Data.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/31/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Data__
#define __BasicMarkovBrainTemplate__Data__

#include <iostream>

#include <map>
#include <memory>
#include <set>
#include <stdlib.h>
#include <string>
#include <vector>

#include "Utilities.h"

using namespace std;

class DataMap {
private:
	map<string, string> data;

public:
	/*
	 * sets a value at "key" to a DataMap.data
	 * Template Functions must be in a header
	 */
	template<typename Type>
	void Set(const string& key, const Type& value) { // sets a value in a DataMap with "key"
		data[key] = mkString(value);
	}

	/*
	 * appends a value at "key" to a DataMap.data
	 * Template Functions must be in a header
	 */
	template<typename Type>
	void Append(const string& key, const Type& value) {
		// is this value in the DataMap already?
		if (data.find(key) == data.end()) { // if this key is not already in the DataMap
			data[key] = "[" + mkString(value) + "]";
		} else { // the key already exists
			if (data[key][0] == '[') { // if this key is already associated with a list
				string workingString = data[key];
				workingString.pop_back(); // strip off trailing ']'
				workingString = workingString + "," + mkString(value) + "]"; // add ",value]"
				data[key] = workingString; // put new list back in DataMap
			} else { // this key exists, but the associated element is not a list
				cout
						<< "  In DataMap::Append(key, value)\n  ERROR: attempted to append to a non list DataMap entry. \n    Exiting!\n";
				exit(1);
			}
		}
	}

	string Get(const string& key); // retrieve the string from a dataMap with "key"
	void writeToFile(const string& fileName, const vector<string>& keys); // write from this DataMap to fileName the data associated with keys
	vector<string> GetKeys();
	void Clear();
};

class FileManager {
public:
	static set<string> dataFilesCreated; // list of files, this allows us to track if headers must be written
	static char separator;
	void static writeToFile(const string& fileName, const string& data, const string& header); //fileName, data, header - used when you want to output formatted data (i.e. genomes)
};

#endif /* defined(__BasicMarkovBrainTemplate__Data__) */
