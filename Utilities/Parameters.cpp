//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#include "Parameters.h"
using namespace std;

shared_ptr<ParametersTable> Parameters::root;

long long ParametersTable::nextTableID = 0;

template<>
const bool ParametersEntry<bool>::getBool() {
	return get();
}

template<>
const string ParametersEntry<string>::getString() {
	return get();
}

template<>
const int ParametersEntry<int>::getInt() {
	return get();
}

template<>
const double ParametersEntry<double>::getDouble() {
	return get();
}

shared_ptr<ParameterLink<bool>> Parameters::getBoolLink(const string& name, shared_ptr<ParametersTable> table) {
	auto entry = table->lookupBoolEntry(name);
	auto newLink = make_shared<ParameterLink<bool>>(name, entry, table);
	return newLink;
}

shared_ptr<ParameterLink<string>> Parameters::getStringLink(const string& name, shared_ptr<ParametersTable> table) {
	auto entry = table->lookupStringEntry(name);
	auto newLink = make_shared<ParameterLink<string>>(name, entry, table);
	return newLink;
}

shared_ptr<ParameterLink<int>> Parameters::getIntLink(const string& name, shared_ptr<ParametersTable> table) {
	auto entry = table->lookupIntEntry(name);
	auto newLink = make_shared<ParameterLink<int>>(name, entry, table);
	return newLink;
}

shared_ptr<ParameterLink<double>> Parameters::getDoubleLink(const string& name, shared_ptr<ParametersTable> table) {
	auto entry = table->lookupDoubleEntry(name);
	auto newLink = make_shared<ParameterLink<double>>(name, entry, table);
	return newLink;
}

void Parameters::parseFullParameterName(const string& fullName, string& nameSpace, string& category, string& parameterName) {
	int i = fullName.size() - 1;
	nameSpace = "";
	category = "";
	parameterName = "";
	string workingString = "";
	while (i > -1) {
		if (fullName[i] == '-') {
			parameterName = workingString;
			workingString = "";
			i--;
			break;
		}
		workingString = fullName[i] + workingString;
		i--;
	}
	while (i > -1) {
		if (i < (int) fullName.size() - 1 && (fullName[i] == ':' && fullName[i + 1] == ':')) {
			category = workingString.substr(1, workingString.size());
			workingString = ":";
			break;
		} else {
			workingString = fullName[i] + workingString;
		}
		i--;
	}

	while (i > -1) {
		workingString = fullName[i] + workingString;
		i--;
	}

	if (category == "") {
		category = workingString;
	} else {
		nameSpace = workingString;
	}
}

void Parameters::readCommandLine(int argc, const char** argv, unordered_map<string, string>& comand_line_list, vector<string>& fileList, bool& saveFiles) {
	int argCount = 1;
	while (argCount < argc) {
		if (argv[argCount][0] == '-' && argv[argCount][1] == 'f') {  // denotes that file names will follow
			argCount++;
			if (argCount >= argc) {
				cout << "ERROR! -f found on command line with no file name.\nExiting\n";
				exit(1);
			} else {
				while (argCount < argc && argv[argCount][0] != '-') {
					fileList.push_back(argv[argCount++]);
				}
			}
		} else if (argv[argCount][0] == '-' && argv[argCount][1] == 'p') {  // denotes that parameter/value pairs will follow
			argCount++;
			if (argCount > argc) {
				cout << "ERROR! -p found on command line with no parameters.\nExiting\n";
				exit(1);
			} else {
				while (argCount < argc && argv[argCount][0] != '-') {
					if (argCount + 1 < argc) {
						if (comand_line_list.find(string(argv[argCount])) == comand_line_list.end()) {
							comand_line_list[string(argv[argCount])] = string(argv[argCount + 1]);
						} else {
							cout << "  ERROR :: Parameter \"" << string(argv[argCount]) << "\" is defined more then once on the command line.\nExiting.\n";
							exit(1);
						}
						argCount += 2;
					} else {
						cout << "  ERROR :: Parameter \"" << string(argv[argCount]) << "\" is defined on command line with out a value.\nExiting.\n";
						exit(1);
					}
				}
			}
		} else if (argv[argCount][0] == '-' && argv[argCount][1] == 's') {
			saveFiles = true;
			argCount++;
		} else if (argv[argCount][0] == '-' && argv[argCount][1] == 'h') {
			cout << "Usage: " << argv[0] << " [-f <file1> <file2> ...] [-p <parameter name/value pairs>] [-s]\n\n" << "  -f, \"load files\" - list of settings files to be loaded.\n" << "         Parameters in later files overwrite parameters in earlier files.\n\n" << "  -p, \"set parameters\" - list of parameter/name pairs \n" << "         (i.e. \"-p GLOBAL-updates 100 GLOBAL-popSize 200\" would set MABE to run\n" << "         for 100 updates with a population size of 200. Parameters set on the\n" << "         command line overwrite parameters from files\n\n" << "  -s, \"save\" - save settings files.\n" << endl;
			exit(1);
		} else {
			cout << "  Error on command line. Unrecognized option. Exiting." << endl;
			exit(1);
		}
	}
}

unordered_map<string, string> Parameters::readParametersFile(string fileName) {
	unordered_map<string, string> config_file_list;
	set<char> nameFirstLegalChars = {  // characters that can be used as the first "letter" of a name
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '_' };
	set<char> nameLegalChars = {  // characters that can be used in the body of a name
			'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '_' };
	set<char> whiteSpaceChars = { ' ', '\t' };

	string line;
	int line_number = 0;
	unsigned int index;
	string parameterName;
	string parameterValue;

	cout << "  - loading file \"" << fileName << "\"\n";

	ifstream configFile(fileName);  // open file named by fileName
	if (configFile.is_open())  // if the file named by fileName can be opened
	{
		string categoryName = "";
		string nameSpace = "";
		while (getline(configFile, line))  // keep loading one line from the file at a time into "line" until we get to the end of the file
		{
			line_number++;
			//cout << "line #: " << line_number << endl;
			index = 0;
			parameterValue = "";
			parameterName = "";
			while (index < line.length() && iswspace(line[index])) {
				index++;
			}

			if (index < line.length() && line[index] == '+') {  //if a line starts with % parse a catagory name
				string newNestingName = "";
				index++;
				if (line[index] != ' ') {
					cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
					cout << "    While reading a nameSpace name, was expecting a blank space after \"+\".\n" << endl;
					exit(1);
				}
				index++;
				// get name (must start with letter)
				if (index < line.length() && isalpha(line[index])) {
					newNestingName += line[index];
					index++;
				} else if (index < line.length()) {  // if the not a name start character
					cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
					cout << "    While reading a nameSpace name, found invalid first character after \"+\".\n" << endl;
					exit(1);
				}
				// get rest of name, must be numbers or letters (no '_'s)
				while (index < line.length() && isalnum(line[index])) {
					newNestingName += line[index];
					index++;
				}
				while (index < line.length() && iswspace(line[index])) {
					index++;
				}
				if (index < line.length() && line[index] != '#') {
					cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
					cout << "    While reading nameSpace name \"" << newNestingName << "\", found unused/invalid characters after nameSpace define outside of comment." << endl;
					exit(1);
				}
				if (newNestingName != "") {
					nameSpace = nameSpace + newNestingName + "::";
				}
				index = line.length();  // move to end of line
			}

			if (index < line.length() && line[index] == '-') {  //if a line starts with - move up one name space
				if (nameSpace == "") {
					cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\". Exiting.\n";
					cout << "    attempt to leave nameSpace, when not in nameSpace" << endl;
					exit(1);
				} else {
					int cutPoint = nameSpace.size() - 2;
					while ((nameSpace[cutPoint] != ':' || nameSpace[cutPoint - 1] != ':') && cutPoint > 0) {
						//cout << cutPoint << " ";
						cutPoint--;
						//cout << cutPoint << endl;
					}
					//cout << "b: " << nameSpace << endl;
					if (cutPoint == 0) {
						nameSpace = "";
					} else {
						nameSpace = nameSpace.substr(0, cutPoint + 1);
					}
					//cout << "a: " << nameSpace << endl;
				}
				index = line.length();  // move to end of line
			}

			if (index < line.length() && line[index] == '%') {  //if a line starts with % parse a catagory name
				categoryName = "";
				index++;
				while (index < line.length() && iswspace(line[index])) {
					index++;
				}
				// get name (must start with letter or "_")
				if (index < line.length() && (isalpha(line[index]) || line[index] == '_')) {
					categoryName += line[index];
					index++;
				} else if (index < line.length()) {  // if the first character is not a letter or "_"
					cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\". Exiting.\n";
					cout << "    While category name found invalid character after \"%\"\n";
					exit(1);
				}
				// get rest of name, must be numbers or letters or '_'s
				while (index < line.length() && (isalnum(line[index]) || line[index] == '_')) {
					categoryName += line[index];
					//cout << newCategoryName << endl;
					index++;
				}
				while (index < line.length() && iswspace(line[index])) {
					index++;
				}
				if (index < line.length() && line[index] != '#') {
					cout << "  - Config File: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\". Exiting.\n";
					cout << "    While reading category name \"" << categoryName << "\", found unused/invalid characters after category define outside of comment." << endl;
					exit(1);
				}
				index = line.length();  // move to end of line
			}

			// move past any leading white space
			while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
				index++;
			}
			// comment can start before name, if it does, we are done with this line... move to end of line
			if (index < line.length() && line[index] == '#') {
				index = line.length();
			}
			// get name (must start with letter or "_")
			if (index < line.length() && nameFirstLegalChars.find(line[index]) != nameFirstLegalChars.end()) {
				parameterName += line[index];
				index++;
			} else if (index < line.length()) {  // if the first non whitespace character is not "#" or a name start character
				parameterName = "BAD_PARAMETER_NAME";  // set the paramterName to tell later error not to print
				index = line.length();
			}
			// get rest of name
			while (index < line.length() && ((nameLegalChars.find(line[index]) != nameLegalChars.end()) || (line[index] == '-') || (line[index] == ':'))) {
				parameterName += line[index];
				index++;
			}
			if (parameterName != "") {
				if (categoryName != "") {
					parameterName = categoryName + "-" + parameterName;
				}

				if (nameSpace != "") {
					parameterName = nameSpace + parameterName;
				}
			}
			//cout << parameterName << "   " << nameSpace << " " << categoryName << "\n";
			// move past white space between name and "="
			while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
				index++;
			}
			// the next character must be "="
			if (index < line.length() && line[index] == '=') {
				index++;
				// move past white space between "=" and value
				while (index < line.length() && whiteSpaceChars.find(line[index]) != whiteSpaceChars.end()) {
					index++;
				}
				// get value : values can be made up of any characters
				while (index < line.length() && whiteSpaceChars.find(line[index]) == whiteSpaceChars.end() && line[index] != '#') {
					parameterValue += line[index];
					index++;
				}
				// move though whitespace till "#" or EoL
				while (index < line.length() && (whiteSpaceChars.find(line[index]) != whiteSpaceChars.end() || line[index] == '#')) {
					if (line[index] == '#') {  // if "#" move to end of line
						index = line.length();
					} else {  // move past whitespace
						index++;
					}
				}
			}
			if (index != line.length()) {  // if not at end of line, there was a problem
				cout << "  ERROR :: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting\n";
				exit(1);
			} else {
				// if there is a name and a value
				if (parameterName != "" && parameterValue != "") {
					if (config_file_list.find(parameterName) == config_file_list.end()) {
						config_file_list[parameterName] = string(parameterValue);
					} else {
						cout << "  - \"" << string(parameterName) << "\" is defined more then once in file: \"" << fileName << "\".\n exiting.\n";
						exit(1);
					}

				} else if (parameterName != "" && parameterValue == "") {  // if there is a name but no value, throw warning
					if (parameterName != "BAD_PARAMETER_NAME") {  // if "BAD_PARAMETER_NAME" then we already printed a warning.
						cout << "  ERROR :: SYNTAX ERROR on line " << line_number << " in file: \"" << fileName << "\".\nExiting.\n";
						exit(1);
					}
				}
			}
		}
		configFile.close();
	} else {
		cout << "  ERROR! unable to open file \"" << fileName << "\".\nExiting.\n";
		exit(1);
	}
	return config_file_list;
}

bool Parameters::initializeParameters(int argc, const char * argv[]) {

	if (root == nullptr) {
		root = ParametersTable::makeTable();
	}

	unordered_map<string, string> command_line_list;
	vector<string> fileList;

	bool saveFiles = false;
	Parameters::readCommandLine(argc, argv, command_line_list, fileList, saveFiles);

	string workingNameSpace, workingCategory, workingParameterName;

	for (auto fileName : fileList) {  // load all files in order
		unordered_map<string, string> file_list = Parameters::readParametersFile(fileName);
		for (auto i : file_list) {
			parseFullParameterName(i.first, workingNameSpace, workingCategory, workingParameterName);
			ASSERT(workingParameterName != "", "  ERROR! :: reading from file \"" << fileName << "\" found misformatted parameter \"" << i.first << "\"\n  Parameters must have format: [category]-[name] or [name space][category]-[name]");
			string parameterType = root->getParameterType(workingCategory + "-" + workingParameterName);
			if (parameterType == "bool") {
				bool value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else if (parameterType == "string") {
				string value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else if (parameterType == "int") {
				int value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else if (parameterType == "double") {
				double value;
				stringToValue(i.second, value);
				root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
			} else {
				if (saveFiles) {
					cout << "   WARRNING";
				} else {
					cout << "  ERROR";
				}
				cout << " :: while reading file \"" << fileName << "\" found \"" << workingNameSpace + workingCategory + "-" + workingParameterName << ".\n      But \"" << workingCategory + "-" + workingParameterName << "\" is not a registered parameter!" << endl;
				if (saveFiles) {
					cout << "      This parameter will not be saved to new files." << endl;
				} else {
					cout << "  Exiting." << endl;
					exit(1);
				}
			}
		}
	}
	for (auto i : command_line_list) {  // load command line parameters last
		parseFullParameterName(i.first, workingNameSpace, workingCategory, workingParameterName);
		ASSERT(workingParameterName != "", "  ERROR! :: reading from command line found misformatted parameter \"" << i.first << "\"\n  Parameters must have format: [category]-[name] or [name space][category]-[name]");
		string parameterType = root->getParameterType(workingCategory + "-" + workingParameterName);
		if (parameterType == "bool") {
			bool value;
			stringToValue(i.second, value);
			root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
		} else if (parameterType == "string") {
			string value;
			stringToValue(i.second, value);
			root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
		} else if (parameterType == "int") {
			int value;
			stringToValue(i.second, value);
			root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
		} else if (parameterType == "double") {
			double value;
			stringToValue(i.second, value);
			root->setParameter(workingCategory + "-" + workingParameterName, value, workingNameSpace, true);
		} else {
			if (saveFiles) {
				cout << "   WARRNING";
			} else {
				cout << "  ERROR";
			}
			cout << " :: while reading command line found \"" << workingNameSpace + workingCategory + "-" + workingParameterName << ".\n      But \"" << workingCategory + "-" + workingParameterName << "\" is not a registered parameter!" << endl;
			if (saveFiles) {
				cout << "      This parameter will not be saved to new files." << endl;
			} else {
				cout << "  Exiting." << endl;
				exit(1);
			}
		}
	}
	return saveFiles;
//	if (saveFiles) {
//
//		Parameters::saveSettingsFiles(_maxLineLength, _commentIndent, { "*" }, { { "settings_organism.cfg", { "GATE*", "GENOME*", "BRAIN*" } }, { "settings_world.cfg", { "WORLD*" } }, { "settings.cfg", { "" } } });
//		cout << "Saving config Files and Exiting." << endl;
//		exit(0);
//	}
}

void Parameters::saveSettingsFile(const string& nameSpace, stringstream& FILE, vector<string> categoryList, int _maxLineLength, int _commentIndent, bool alsoChildren, int nameSpaceLevel) {
	map<string, vector<string>> sortedParameters;
	root->lookupTable(nameSpace)->parametersToSortedList(sortedParameters);
	if (!root->lookupTable(nameSpace)->neverSave) {
		string currentIndent = "";
		vector<string> nameSpaceParts = nameSpaceToNameParts(nameSpace);

		for (int i = 0; i < nameSpaceLevel; i++) {
			currentIndent += "  ";
			nameSpaceParts.erase(nameSpaceParts.begin());
		}

		if (nameSpaceParts.size() > 0) {
			for (auto p : nameSpaceParts) {
				FILE << currentIndent << "+ " << p.substr(0, p.size() - 2) << "\n";
				nameSpaceLevel++;
				currentIndent += "  ";
			}
		}
		if (categoryList.size() > 0 && categoryList[0] == "-") {
			if (sortedParameters.find("GLOBAL") != sortedParameters.end() && !(find(categoryList.begin(), categoryList.end(), "GLOBAL") != categoryList.end())) {
				FILE << currentIndent << "% GLOBAL" << "\n";
				for (auto parameter : sortedParameters["GLOBAL"]) {
					printParameterWithWraparound(FILE, currentIndent + "  ", parameter, _maxLineLength, _commentIndent);
//					FILE << currentIndent << "  " << parameter << "\n";
				}
				FILE << "\n";
			}
		} else {  // write parameters to file.
			if (sortedParameters.find("GLOBAL") != sortedParameters.end() && find(categoryList.begin(), categoryList.end(), "GLOBAL") != categoryList.end()) {
				FILE << currentIndent << "% GLOBAL" << "\n";
				for (auto parameter : sortedParameters["GLOBAL"]) {
					printParameterWithWraparound(FILE, currentIndent + "  ", parameter, _maxLineLength, _commentIndent);
//					FILE << currentIndent << "  " << parameter << "\n";
				}
				FILE << "\n";
			}

		}
		sortedParameters.erase("GLOBAL");

		for (auto group : sortedParameters) {
			bool saveThis = false;
			if (categoryList.size() > 0 && categoryList[0] != "-") {
				for (auto cat : categoryList) {
					if ((int) group.first.size() >= ((int) cat.size()) - 1) {
						if (group.first == cat) {
							saveThis = true;
						} else {
							if ((int) cat.size() > 0 && cat[((int) cat.size()) - 1] == '*') {
								if (group.first.substr(0, cat.size() - 1) == cat.substr(0, cat.size() - 1)) {
									saveThis = true;
								}
							}
						}
					}

				}
			} else {
				saveThis = true;
				for (auto cat : categoryList) {
					if ((int) group.first.size() >= ((int) cat.size()) - 1) {
						if (group.first == cat) {
							saveThis = false;
						} else {
							if ((int) cat.size() > 0 && cat[((int) cat.size()) - 1] == '*') {
								if (group.first.substr(0, cat.size() - 1) == cat.substr(0, cat.size() - 1)) {
									saveThis = false;
								}
							}
						}
					}
				}
			}
			if (saveThis) {
				FILE << currentIndent << "% " << group.first << "\n";
				for (auto parameter : group.second) {
					printParameterWithWraparound(FILE, currentIndent + "  ", parameter, _maxLineLength, _commentIndent);
//					FILE << currentIndent << "  " << parameter << "\n";
				}
				FILE << "\n";
			}
		}
		if (alsoChildren) {
			vector<shared_ptr<ParametersTable>> checklist = root->lookupTable(nameSpace)->getChildren();
			sort(checklist.begin(), checklist.end());
			for (auto c : checklist) {
				saveSettingsFile(c->getTableNameSpace(), FILE, categoryList, _maxLineLength, _commentIndent, true, nameSpaceLevel);
			}
		}

		while (nameSpaceParts.size() > 0) {
			currentIndent = currentIndent.substr(2, currentIndent.size());
			FILE << currentIndent << "- (" << nameSpaceParts[nameSpaceParts.size() - 1].substr(0, nameSpaceParts[nameSpaceParts.size() - 1].size() - 2) << ")\n";
			nameSpaceParts.pop_back();
		}
		//cout << "  - \"" << fileName << "\" has been created.\n";
	}
}

void Parameters::printParameterWithWraparound(stringstream& FILE, string _currentIndent, string _parameter, int _maxLineLength, int _commentIndent) {
	int currentLineLength = _currentIndent.size();
	FILE << _currentIndent;
	int beforeIndentLength = currentLineLength + _parameter.find("@@@#");
	string betweenString = "";
	for (int i = beforeIndentLength; i < _commentIndent; i++) {
		betweenString.append(" ");
	}
	betweenString.append("#");
	_parameter.replace(_parameter.find("@@@#"), 4, betweenString);
	if (currentLineLength + (int) _parameter.size() < _maxLineLength && (int)_parameter.find("\n") == -1) {
		FILE << _parameter << endl;
	} else {
		string indent = "";
		for (int i = 0; i < _commentIndent; i++) {
			indent.append(" ");
		}
		indent.append("#    ");

		string parameterName = _parameter.substr(0, _parameter.find("#"));
		string parameterRemainder = _parameter.substr(_parameter.find("#"));
		FILE << parameterName;
		currentLineLength += parameterName.size();
		int remainderMaxLength = _maxLineLength - _commentIndent;
		while (parameterRemainder.size() > 0) {
			int newLine = parameterRemainder.substr(0, remainderMaxLength).find("\n");
			if (newLine == -1) {
				int lastSpace = parameterRemainder.substr(0, remainderMaxLength).find_last_of(" ");
				if (lastSpace == -1) {
					if (currentLineLength == 0) {
						FILE << indent;
						currentLineLength += _commentIndent;
					}
					if ((int) parameterRemainder.size() > remainderMaxLength) {
						string takenPart = parameterRemainder.substr(0, remainderMaxLength - 1);
						parameterRemainder = parameterRemainder.substr(remainderMaxLength - 1);
						takenPart.append("-");
						FILE << takenPart << endl;
					} else {
						FILE << parameterRemainder << endl;
						parameterRemainder = "";
					}
				} else {
					if (currentLineLength == 0) {
						FILE << indent;
						currentLineLength += _commentIndent;
					}
					if ((int) parameterRemainder.size() > remainderMaxLength) {
						string remainderBeforeSpace = parameterRemainder.substr(0, lastSpace);
						parameterRemainder = parameterRemainder.substr(lastSpace + 1);
						FILE << remainderBeforeSpace << endl;
					} else {
						FILE << parameterRemainder << endl;
						parameterRemainder = "";
					}
				}
			} else {
				if (currentLineLength == 0) {
					FILE << indent;
					currentLineLength += _commentIndent;
				}
				string remainderBeforeNewLine = parameterRemainder.substr(0, newLine);
				parameterRemainder = parameterRemainder.substr(newLine + 1);
				FILE << remainderBeforeNewLine << endl;
			}
			currentLineLength = 0;
		}
	}
}

void Parameters::saveSettingsFiles(int _maxLineLength, int _commentIndent, vector<string> nameSpaceList, vector<pair<string, vector<string>>> categoryLists) {
	bool alsoChildren;
	string fileName;
	vector<string> otherCategoryList;
	for (auto nameSpace : nameSpaceList) {
		for (auto cList : categoryLists) {
			otherCategoryList.insert(otherCategoryList.end(), cList.second.begin(), cList.second.end());
			if ((int)nameSpace.size() > 0 && nameSpace[nameSpace.size()-1] == '*') {
				nameSpace.pop_back();
				alsoChildren = true;
			} else {
				alsoChildren = false;
			}
			fileName = "";
			bool lastCharWasCol = false;
			for (auto c : nameSpace) {
				if (c != ':') {
					fileName += c;
					lastCharWasCol = false;
				} else {
					if (lastCharWasCol == true) {
						fileName += '_';
					}
					lastCharWasCol = true;
				}
			}
			if (fileName != "") {
				fileName = fileName.substr(0,fileName.size()-1)+"-";
			}
			stringstream ss;
			if (cList.second.size() == 1 && cList.second[0]=="") {
				otherCategoryList.insert(otherCategoryList.begin(),"-");
				saveSettingsFile(nameSpace, ss, otherCategoryList, _maxLineLength, _commentIndent, alsoChildren);
			} else {
				saveSettingsFile(nameSpace, ss, cList.second, _maxLineLength, _commentIndent, alsoChildren);
			}
			string workingString = ss.str();
			workingString.erase (remove (workingString.begin(), workingString.end(), ' '), workingString.end());
			workingString.erase (remove (workingString.begin(), workingString.end(), 11), workingString.end());
			bool lastCharEnter = false;
			bool fileEmpty = true;
			for (auto c : workingString) {
				if (c == 10) {
					lastCharEnter = true;
				} else {
					if (lastCharEnter == true) {
						if (!(c == '+' || c == '-' || c == 10)) {
							fileEmpty = false;
						}
					}
					lastCharEnter = false;
				}
			}
			if (!fileEmpty) {
				ofstream FILE(fileName+cList.first);
				FILE << ss.str();
				FILE.close();
			}
		}
	}
}
