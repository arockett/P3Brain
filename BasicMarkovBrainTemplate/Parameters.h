#ifndef __BasicMarkovBrainTemplate__Parameters__
#define __BasicMarkovBrainTemplate__Parameters__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <map>
#include <fstream>
#include <cstdlib> // for rand!
#include <set>
#include <time.h>       /* clock_t, clock, 1.000 */
#include <iostream>
#include <algorithm>
#include <typeinfo>
#include <sstream>

using namespace std;

string valToString(int val);
string valToString(float val);
string valToString(double val);
string valToString(string val);
string valToString(bool val);

string get_Type(int parameter);
string get_Type(float parameter);
string get_Type(double parameter);
string get_Type(bool parameter);
string get_Type(string parameter);

/* assign takes a reference varable and a string and attepts to assign
 * the value of the string to the variable.
 * assign() returns true if the assignment was succesful.
 */

template<class T>
bool assign(T & testType, string testString) {
	if (testString == "true") {
		testString = "1";
	}
	if (testString == "false") {
		testString = "0";
	}

	stringstream ss;
	string emptyString = "";
	ss << testString;
	ss >> testType;
	if (ss.fail()) {
		return false;
	} else {
		ss >> emptyString;
		if (ss.fail()) {
			return true;
		} else {
			return false;
		}
	}
}

class parameterDesc {
public:
	string name, type;
	string commandLineValue, configFileValue, defaultValue; // values defined in various sources
	int registeredCount, commandLineCount, configFileCount; // counts number of times parameters are defined in various sources
	string description; // a short description of what this parameter is used for
};

class Parameters {
private:
	Parameters() {
	} // to make sure noone makes an instance of this class

public:
	static map<string, parameterDesc> parameters;

	static string configFileName;
	static bool paramVerbose; // if true: report when values are assigned

	/*	loads command line name/value pairs
	 */
	static void loadArguments(int argc, const char** argv) {

		cout << "  - loading Command Line.\n";

		int argCount = argc;
		if ((argc & 1) == 0) {
			argCount = argc - 1;
			cout
					<< "  - Command Line: too many arguments on command line... ignoring last argument.\n";
		}
		for (int i = 1; i < argCount; i = i + 2) {
			if (parameters.find(string(argv[i])) == parameters.end()) { // if this parameter does not exist
				parameters[string(argv[i])].name = string(argv[i]);
				parameters[string(argv[i])].commandLineValue = string(
						argv[i + 1]);
				parameters[string(argv[i])].registeredCount = 0;
				parameters[string(argv[i])].commandLineCount = 1; // it came from the commandLine
				parameters[string(argv[i])].configFileCount = 0;
			} else if (parameters[string(argv[i])].commandLineCount == 0) { // else if this parameter does exist, but it does not have a command line value
				//parameters[string(argv[i])].name = string(argv[i]);
				parameters[string(argv[i])].commandLineValue = string(
						argv[i + 1]);
				parameters[string(argv[i])].commandLineCount = 1;
			} else { // else parameter exists and already has a command line value
				cout << "  - Command Line: \"" << string(argv[i])
						<< "\" defined more then once. Ignoring newer definition.\n";
				parameters[string(argv[i])].commandLineCount++;
			}
		}
	}
	/* load name value pairs from config file
	 */
	static void loadConfigFile() {
		set<char> nameFirstLegalChars = { // characters that can be used as the first "letter" of a name
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
						'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
						'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
						'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
						'U', 'V', 'W', 'X', 'Y', 'Z', '_' };
		set<char> nameLegalChars = { // characters that can be used in the body of a name
				'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
						'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
						'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
						'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
						'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5',
						'6', '7', '8', '9', '0', '_', ':' };
		set<char> whiteSpaceChars = { ' ', '\t' };

		string line;
		int line_number = 0;
		unsigned int index;
		string parameterName;
		string parameterValue;

		cout << "  - loading Config File \"" << configFileName << "\"\n";

		ifstream configFile(configFileName); // open file named by configFileName
		if (configFile.is_open()) // if the file named by configFileName can be opened
		{
			while (getline(configFile, line)) // keep loading one line from the file at a time into "line" until we get to the end of the file
			{
				line_number++;
				index = 0;
				parameterName = "";
				parameterValue = "";
				// move past any leading white space
				while (index < line.length()
						&& whiteSpaceChars.find(line[index])
								!= whiteSpaceChars.end()) {
					index++;
				}
				// comment can start before name, if it does, we are done with this line... move to end of line
				if (index < line.length() && line[index] == '#') {
					index = line.length();
				}
				// get name (must start with letter or "_")
				if (index < line.length()
						&& nameFirstLegalChars.find(line[index])
								!= nameFirstLegalChars.end()) {
					parameterName += line[index];
					index++;
				} else if (index < line.length()) { // if the first character on the line is not white space, # or a name start character
					parameterName = "__"; // set the paramterName to something
					index = line.length(); // move to end of line (this will create a error since there is a name, but no value)
				}
				// get rest of name
				while (index < line.length()
						&& nameLegalChars.find(line[index])
								!= nameLegalChars.end()) {
					parameterName += line[index];
					index++;
				}
				// move past white space between name and "="
				while (index < line.length()
						&& whiteSpaceChars.find(line[index])
								!= whiteSpaceChars.end()) {
					index++;
				}
				// the next character must be "="
				if (index < line.length() && line[index] == '=') {
					index++;
					// move past white space between "=" and value
					while (index < line.length()
							&& whiteSpaceChars.find(line[index])
									!= whiteSpaceChars.end()) {
						index++;
					}
					// get value : values can be made up of any characters
					while (index < line.length()
							&& whiteSpaceChars.find(line[index])
									== whiteSpaceChars.end()
							&& line[index] != '#') {
						parameterValue += line[index];
						index++;
					}
					// move though whitespace till "#" or EoL
					while (index < line.length()
							&& (whiteSpaceChars.find(line[index])
									!= whiteSpaceChars.end()
									|| line[index] == '#')) {
						if (line[index] == '#') { // if "#" move to end of line
							index = line.length();
						} else { // move past whitespace
							index++;
						}
					}
				}
				if (index != line.length()) { // if not at end of line, there was a problem
					cout << "  - Config File: SYNTAX ERROR on line "
							<< line_number << " in file: \"" << configFileName
							<< "\". Ignoring line.\n";
				} else {
					// if there is a name and a value, add the name to Parameters with the found configFileValue
					if (parameterName != "" && parameterValue != "") {
						if (parameters.find(parameterName)
								== parameters.end()) { // if this parameter does not exist, make it
							parameters[parameterName].name = parameterName;
							parameters[parameterName].configFileValue =
									parameterValue;
							parameters[parameterName].registeredCount = 0;
							parameters[parameterName].commandLineCount = 0;
							parameters[parameterName].configFileCount = 1;
						} else if (parameters[parameterName].configFileCount
								== 0) { // if this parameter exists, but has no config file value, add the configFileVale
							parameters[parameterName].name = parameterName;
							parameters[parameterName].configFileValue =
									parameterValue;
							parameters[parameterName].configFileCount = 1;
						} else { // if this parameter exists and already has a config file value, throw warning
							cout << "  - Config File: \"" << parameterName
									<< "\" defined more then once. Ignoring newer definition.\n";
							parameters[parameterName].configFileCount++;
						}
					} else if (parameterName != "" && parameterValue == "") { // if there is a name but no value, throw warning
						cout << "  - Config File: SYNTAX ERROR on line "
								<< line_number << " in file: \""
								<< configFileName << "\". Ignoring line.\n";
					}
				}
			}
			configFile.close();
		}

		else
			cout << "  - Config File: WARNING unable to open file \""
					<< configFileName << "\". Proceeding without.\n";
	}

	/* registers a parameter name - parameter names are registed when there is
	 * a request in the code for the value associated with the parameter name.
	 * returns "true" if the value was succesfuly registered (i.e. it was not already
	 * registered)
	 */
	static bool registerParameter(string parameterName) {
		if (parameters.find(parameterName) == parameters.end()) {
			// if this paramater has not already been defined add to paramaters
			parameters[parameterName].name = parameterName;
			parameters[parameterName].registeredCount = 1;
			parameters[parameterName].commandLineCount = 0;
			parameters[parameterName].configFileCount = 0;
			return true;
		} else {
			// if paramater is already defined (i.e. it was defined in the command line or config file)
			if (parameters[parameterName].registeredCount > 0) {
				// if paramater is already defined & if parameter has already been registerd... this may be a problem!
				parameters[parameterName].registeredCount++;
				return false;
			} else {
				// if paramater is already defined & if the parameter has _not_ already been registed
				parameters[parameterName].registeredCount = 1;
				return true;
			}
		}
	}

	/* takes parameter and checks to see if it is in the command line or config file.
	 * if in command line, use command line value, else config, else defaultValue
	 */
	template<class T>
	static void setupParameter(string parameterName, T& parameter,
			T defaultValue, string parameterDesc) {
		if (!registerParameter(parameterName)) { // register the parameterName - if this fails then parameter name has already been registered
			cout << "  - getParameterValue: WARNING :: \"" << parameterName
					<< "\" has been registered more then once. Using values from first attempt...\n";
			if (get_Type(parameter) != parameters[parameterName].type) {
				cout
						<< "    ... but the new type is different!\n Cannot recover. Exiting.\n";
				exit(10);
			}
		} else { // if parameterName is successfully registered...
			parameters[parameterName].description = parameterDesc;
			parameters[parameterName].type = get_Type(parameter);
			parameters[parameterName].defaultValue = valToString(defaultValue);
		}

		// check if this parameter is in the command line
		if (parameters[parameterName].commandLineCount > 0) {
			if (assign(parameter, parameters[parameterName].commandLineValue)) {
				if (Parameters::paramVerbose) {
					cout << "  - Parameter: \"" << parameterName << "\" = "
							<< parameter << " :: set by command line.\n";
				}
			} else { // if the assign fails then the value is of the wrong type! this is very bad!
				cout << "  - Parameter: \"" << parameterName
						<< "\" ERROR!! command line value \""
						<< parameters[parameterName].commandLineValue
						<< "\" does not match type \""
						<< parameters[parameterName].type
						<< "\".\nCannot recover. Exiting.\n";
				exit(10);
			}
		}
		// if it's not on the command line, check if it's in the config file
		else if (parameters[parameterName].configFileCount > 0) {
			if (assign(parameter, parameters[parameterName].configFileValue)) {
				if (Parameters::paramVerbose) {
					cout << "  - Parameter: \"" << parameterName << "\" = "
							<< parameter << " :: set by file " << configFileName
							<< ".\n";
				}
			} else { // if the assign fails then the value is of the wrong type! this is very bad!
				cout << "  - Parameter: \"" << parameterName
						<< "\" ERROR!! config file value \""
						<< parameters[parameterName].configFileValue
						<< "\" does not match type \""
						<< parameters[parameterName].type
						<< "\".\nCannot recover. Exiting.\n";
				exit(10);
			}
		} else { // if it's not on the command line or in the config file, use the default value
			parameter = defaultValue; // don't need to check the type here : if it was wrong the compiler would have thown and error
			if (Parameters::paramVerbose) {
				cout << "  - Parameter: \"" << parameterName << "\" = "
						<< parameter << " :: set by default value.\n";
			}
		}
	}

	/* this version of the method is needed for strings so that the initial call does not need to cast 3rd argument to string
	 */
	static void setupParameter(string parameterName, string& parameter,
			const char* defaultValue, string parameterDesc) {
		setupParameter(parameterName, parameter, string(defaultValue),
				parameterDesc);
	}

	/* print list of inuse parameter names and values that were defined from various sources
	 * will also show if parameters are set in command line or config file and not used
	 * will also show if parameters are being defined or registered multiple times
	 * verbose 0 = no output
	 * verbose 1 = number of unused values (if any)
	 * verbose 2 = also show unused values
	 * verbose 3 = also show in use values
	 */
	static int printParameterInfo(int verbose) {
		int num_unused = 0; // counts number of paramaters set by command line or config file, but not used
		if (verbose > 1) {
			cout << "\n  - printParameterInfo\n";
			cout
					<< "  --------------------------------------------------------------------------------- \n";
			cout
					<< "     \"NAME\"  (number of times \"NAME\" has been registered)\n        * source: \"value\" (number of times \"NAME\" was defined in source)\n";
			cout << "              * denotes which is active source.\n";
			cout
					<< "  ---------------------------------------------------------------------------------\n";
		}
		for (auto var : parameters) { // for each parameter in parameters
			if (var.second.registeredCount == 0) { // if not used
				if (verbose > 1) {
					cout << "      \"" << var.first
							<< "\" -- Warning!! :: not in use!\n";
				}
				num_unused++;
			} else { // if in use
				if (verbose > 2) {
					cout << "     \"" << var.first << "\" ("
							<< var.second.registeredCount << ")\n"; // print name
				}
			}
			if (var.second.commandLineCount != 0) { // if there is a command line value
				if (verbose > 2) {
					cout << "\t* command line value: \""
							<< var.second.commandLineValue << "\" " << "("
							<< var.second.commandLineCount << ")" << "\n";
				}
			}
			if (var.second.configFileCount != 0) { // if there is a config file value
				if (verbose > 2) {
					if (var.second.commandLineCount == 0) {
						cout << "\t* ";
					} else {
						cout << "\t  ";
					}
					cout << "config file value: \""
							<< var.second.configFileValue << "\" " << "("
							<< var.second.configFileCount << ")" << "\n";
				}
			}
			if (verbose > 2) {	// there is always a default value
				if (var.second.commandLineCount == 0
						&& var.second.configFileCount == 0) {
					cout << "\t* ";
				} else {
					cout << "\t  ";
				}
				cout << "default value: \"" << var.second.defaultValue
						<< "\"\n";
			}
		}
		if (verbose > 0 && num_unused > 0) { // if there are an unused parameters
			cout << "  - checkRegistries: " << num_unused
					<< " paramaters defined on command line and/or in config file are not used.\n";
		}
		return num_unused;
	}

	/* print list of registed parameters names with their descriptions (basicly the manual)
	 * will also show if parameters are set in command line or config file and not used
	 */
	static void printParameterDescriptions() {
		cout << "\nDisplaying all setable parameters...\nRegistered values\n\n";
		for (auto parameter : parameters) {
			if (parameter.second.registeredCount > 0) {
				cout << "  \"" << parameter.first << "\" - "
						<< parameter.second.type << "\n    - "
						<< parameter.second.description << "\n";
			}
		}

		cout
				<< "\nUnregistered values\n  (defined on the command line or in config file, but not used in the program)\n\n";

		for (auto parameter : parameters) {
			if (parameter.second.registeredCount == 0) {
				cout << "  \"" << parameter.first << "\"\n";
			}
		}
	}

	/* output all registered parameters to a file named by "fileName".
	 * If "showDescriptions" == true then also print description to file.
	 */
	static void makeConfigFile(string fileName, bool showDescriptions) {

		cout << "\n  - making new config file: \"" << fileName << "\"\n";

		ofstream newConfigFile;
		newConfigFile.open(fileName);
		const int descOffset = 45;
		int charCount;

		for (auto var : parameters) { // for each parameter in parameters
			if (var.second.registeredCount == 0) { // if not used
				cout << "      \"" << var.first
						<< " was not registered. It will not be added to \""
						<< fileName << "\"\n";
			} else if (var.first == "makeConfigFile"){
				cout << "      skipping \"makeConfigFile\" so that we don't make a new config file every time we run!\n";
			} else { // if in use
				newConfigFile << var.first << " = "; // print: "name = "
				if (var.second.commandLineCount > 0) {
					newConfigFile << var.second.commandLineValue;
					charCount = var.first.length() + 3
							+ var.second.commandLineValue.length();
				} else if (var.second.configFileCount > 0) {
					newConfigFile << var.second.configFileValue;
					charCount = var.first.length() + 3
							+ var.second.configFileValue.length();
				} else {
					newConfigFile << var.second.defaultValue;
					charCount = var.first.length() + 3
							+ var.second.defaultValue.length();
				}
				if (showDescriptions == true) {
					if (charCount < descOffset) {
						for (; charCount < descOffset; charCount++) {
							newConfigFile << " ";
						}
					} else {
						newConfigFile << "  ";
					}
					newConfigFile << "# " << var.second.description;
				}
				newConfigFile << "\n";
			}
		}
		newConfigFile.close();

	}

};

class ParametersSettings {
public:
	static int rep;
	static int populationSize;
	static int updates;
	static int update;
	static bool paramVerbose;

	static void initializeParameters(); // fuction of set values for modual specific parameters
};

#endif // __BasicMarkovBrainTemplate__Parameters__
