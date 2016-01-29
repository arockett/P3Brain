
#include "Chromosome.h"
#include <limits>

template <> Chromosome<unsigned char>::Chromosome(){
		alphabetSize = 256;
	}

template <> Chromosome<bool>::Chromosome(){
		alphabetSize = 2;
	}

//template <> void Chromosome<double>::fillRandom(int length) {
//	sites.resize(length);
//	for (int i = 0; i < length; i++) {
//		sites[i] = Random::getDouble(0,alphabetSize);
//	}
//}
