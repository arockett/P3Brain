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


shared_ptr<bool> Parameters::register_parameter(const string& name, const bool& default_value, const string& documentation) {
	if (root == nullptr){
		root = ParametersTable::makeTable();
	}
	return root->register_parameter(name, default_value, documentation);
}

shared_ptr<string> Parameters::register_parameter(const string& name, const string& default_value, const string& documentation) {
	if (root == nullptr){
		root = ParametersTable::makeTable();
	}
	return root->register_parameter(name, default_value, documentation);
}

shared_ptr<int> Parameters::register_parameter(const string& name, const int& default_value, const string& documentation) {
	if (root == nullptr){
		root = ParametersTable::makeTable();
	}
	return root->register_parameter(name, default_value, documentation);
}

shared_ptr<double> Parameters::register_parameter(const string& name, const double& default_value, const string& documentation) {
	if (root == nullptr){
		root = ParametersTable::makeTable();
	}
	return root->register_parameter(name, default_value, documentation);
}
