#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <map>
#include <fstream>
#include <cstdlib> // for random!
#include <set>
#include <time.h>       /* clock_t, clock, 1.000 */
#include <iostream>
#include <algorithm>
#include <sstream>

#include "Parameters.h"

using namespace std;

map <string, parameterDesc> Parameters::parameters;
string Parameters::configFileName;
bool Parameters::paramVerbose;


string valToString(int val){
	return to_string(val);
}
string valToString(float val){
	return to_string(val);
}
string valToString(double val){
	return to_string(val);
}
string valToString(string val){
	return val;
}
string valToString(bool val){
	if (val == true){
		return "true";
	}
	else {
		return "false";
	}
}

string get_Type(int parameter){
	return "int";
}
string get_Type(float parameter){
	return "float";
}
string get_Type(double parameter){
	return "double";
}
string get_Type(bool parameter){
	return "bool";
}
string get_Type(string parameter){
	return "string";
}
