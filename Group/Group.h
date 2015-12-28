//
//  Group.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm
//

#ifndef __BasicMarkovBrainTemplate__Group__
#define __BasicMarkovBrainTemplate__Group__

#include "../Archivist/Archivist.h"
#include "../Global.h"
#include "../Optimizer/Optimizer.h"
#include "../Organism/Organism.h"

class Group {
 public:
	vector<shared_ptr<Organism>> population;
	shared_ptr<Archivist> archivist;
	shared_ptr<BaseOptimizer> optimizer;

	Group();
	Group(vector<shared_ptr<Organism>> _population, shared_ptr<BaseOptimizer> _optimizer, shared_ptr<Archivist> _archivist);
	~Group();

	bool archive(int flush = 0);
	void optimize();
};

#endif /* defined(__BasicMarkovBrainTemplate__Group__) */
