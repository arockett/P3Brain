//
//  Group.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm
//

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
