#include <cstring>
#include <memory>
#include <iostream>

#include <fstream>
#include <map>
#include <sstream>

#include "Data.h"

//global variables that should be accessible to all
set<string> FileManager::dataFilesCreated;
char FileManager::separator = ',';

/*
 * returns a string value for "key" given a map<string,string>
 */
string DataMap::Get(const string& key) {
	return data[key];
}

void DataMap::writeToFile(const string& fileName, const vector<string>& keys = { }) {
	string headerStr = "";
	string dataStr = "";
	if (keys.size() > 0) {
		for (auto i : keys) {
			headerStr = headerStr + FileManager::separator + i; // make the header string from keys
			dataStr = dataStr + FileManager::separator + data[i]; // make the data string from data[keys]
		}
		headerStr.erase(headerStr.begin()); // clip off the leading separator
		dataStr.erase(dataStr.begin()); // clip off the leading separator
	} else { // if there are no elements in keys
		for (auto i : data) {
			headerStr = headerStr + FileManager::separator + i.first; // get the elements from the elements in this DataMap
			dataStr = dataStr + FileManager::separator + i.second; // get the values from the DataMap
		}
		if (headerStr.size() > 0) { // if there are any elements in the strings...
			headerStr.erase(headerStr.begin()); // clip off the leading separator
			dataStr.erase(dataStr.begin()); // clip off the leading separator
		} else { // if there are no elements in the strings, print a warning!
			cout << "  In DataMap::writeToFile(): This DataMap contains no keys. Writing a blank line to file: "
					<< fileName << "\n";
		}
	}
	FileManager::writeToFile(fileName, dataStr, headerStr); // write the data to file!
}

vector<string> DataMap::GetKeys() {
	vector<string> keys;
	for (auto element : data) {
		cout << element.first << " ";
		keys.push_back(element.first);
	}
	cout << "\n";
	return (keys);
}

void DataMap::Clear(){
	data.clear();
}

void FileManager::writeToFile(const string& fileName, const string& data, const string& header = "") {
	ofstream FILE;
	bool fileClosed = true;
	if (FileManager::dataFilesCreated.find(fileName) == FileManager::dataFilesCreated.end()) { // if file has not be initialized yet
		FileManager::dataFilesCreated.insert(fileName); // make a note that file exists
		FILE.open(fileName); // clear file contents and open in write mode
		fileClosed = false;
		if (header != "") {
			FILE << header << "\n";
		}
	}
	if (fileClosed){
		FILE.open(fileName, ios::out | ios::app); // open file in append mode
	}
	FILE << data << "\n";
	FILE.close();
}
