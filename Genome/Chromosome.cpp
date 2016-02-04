#include "Chromosome.h"
#include <limits>

// unsigned char constructors

template<> Chromosome<unsigned char>::Chromosome() {
	alphabetSize = 256;
}

template<> Chromosome<unsigned char>::Chromosome(int chromosomeLength) {
	alphabetSize = 256;
	sites.resize(chromosomeLength);
}

template<> Chromosome<unsigned char>::Chromosome(int chromosomeLength, double _alphabetSize) {
	if (_alphabetSize > 256){
		cout << "ERROR: alphabetSize for unsigned char must be 256 or less!\n";
		exit(1);
	}
	alphabetSize = _alphabetSize;
	sites.resize(chromosomeLength);
}

// bool constructors

template<> Chromosome<bool>::Chromosome() {
	alphabetSize = 2;
}

template<> Chromosome<bool>::Chromosome(int chromosomeLength) {
	alphabetSize = 2;
	sites.resize(chromosomeLength);
}

template<> Chromosome<bool>::Chromosome(int chromosomeLength, double _alphabetSize) {
	if (_alphabetSize !=2){
		cout << "ERROR: alphabetSize for bool must be 2!\n";
		exit(1);
	}
	alphabetSize = 2;
	sites.resize(chromosomeLength);
}

// int constructors

template<> Chromosome<int>::Chromosome(int chromosomeLength) {
	alphabetSize = 0;
	cout << "for Chromosome<int>, you must specify both chromosomeLength and alphabetSize\n";
	exit(1);
}

template<> Chromosome<int>::Chromosome(int chromosomeLength, double _alphabetSize) {
	alphabetSize = _alphabetSize;
	sites.resize(chromosomeLength);
}

// double constructors

template<> Chromosome<double>::Chromosome(int chromosomeLength) {
	alphabetSize = 0;
	cout << "for Chromosome<double>, you must specify both chromosomeLength and alphabetSize\n";
}

template<> Chromosome<double>::Chromosome(int chromosomeLength, double _alphabetSize) {
	alphabetSize = _alphabetSize;
	sites.resize(chromosomeLength);
}
