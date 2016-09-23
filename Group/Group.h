//  MABE is a product of The Hintza Lab @ MSU
//     for general research information:
//         http://hintzelab.msu.edu/
//     for MABE documentation:
//         https://github.com/ahnt/BasicMarkovBrainTemplate/wiki - for
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//          https://github.com/ahnt/BasicMarkovBrainTemplate/wiki/license

#ifndef __BasicMarkovBrainTemplate__Group__
#define __BasicMarkovBrainTemplate__Group__

#include "../Archivist/DefaultArchivist.h"
#include "../Global.h"
#include "../Optimizer/AbstractOptimizer.h"
#include "../Organism/Organism.h"

using namespace std;

class Group {
 public:
	vector<shared_ptr<Organism>> population;
	shared_ptr<DefaultArchivist> archivist;
	shared_ptr<AbstractOptimizer> optimizer;

	Group();
	Group(vector<shared_ptr<Organism>> _population, shared_ptr<AbstractOptimizer> _optimizer, shared_ptr<DefaultArchivist> _archivist);
	~Group();

	bool archive(int flush = 0);
	void optimize();
};

#endif /* defined(__BasicMarkovBrainTemplate__Group__) */
