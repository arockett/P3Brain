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

shared_ptr<ParameterLink<bool>> Parameters::getBoolLink(const string& name, shared_ptr<ParametersTable> table){
	auto entry = table->lookupBoolEntry(name);
	auto newLink = make_shared<ParameterLink<bool>>(name, entry, table);
	return newLink;
}

shared_ptr<ParameterLink<string>> Parameters::getStringLink(const string& name, shared_ptr<ParametersTable> table){
	auto entry = table->lookupStringEntry(name);
	auto newLink = make_shared<ParameterLink<string>>(name, entry, table);
	return newLink;
}

shared_ptr<ParameterLink<int>> Parameters::getIntLink(const string& name, shared_ptr<ParametersTable> table){
	auto entry = table->lookupIntEntry(name);
	auto newLink = make_shared<ParameterLink<int>>(name, entry, table);
	return newLink;
}

shared_ptr<ParameterLink<double>> Parameters::getDoubleLink(const string& name, shared_ptr<ParametersTable> table){
	auto entry = table->lookupDoubleEntry(name);
	auto newLink = make_shared<ParameterLink<double>>(name, entry, table);
	return newLink;
}

//ParameterLink Parameters::register_parameter(const string& name, const bool& default_value, const string& documentation) {
//	if (root == nullptr){
//		root = ParametersTable::makeTable();
//	}
//	return root->register_parameter(name, default_value, documentation);
//}
//
//shared_ptr<string> Parameters::register_parameter(const string& name, const string& default_value, const string& documentation) {
//	if (root == nullptr){
//		root = ParametersTable::makeTable();
//	}
//	return root->register_parameter(name, default_value, documentation);
//}
//
//shared_ptr<int> Parameters::register_parameter(const string& name, const int& default_value, const string& documentation) {
//	if (root == nullptr){
//		root = ParametersTable::makeTable();
//	}
//	return root->register_parameter(name, default_value, documentation);
//}
//
//shared_ptr<double> Parameters::register_parameter(const string& name, const double& default_value, const string& documentation) {
//	if (root == nullptr){
//		root = ParametersTable::makeTable();
//	}
//	return root->register_parameter(name, default_value, documentation);
//}
