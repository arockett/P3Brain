#include "Parameters.h"
using namespace std;

shared_ptr<ParametersTable> Parameters::root = ParametersTable::makeTable();


long long ParametersTable::nextTableID = 0;

const string ParametersTable::emptyNameSpace = "'";

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
