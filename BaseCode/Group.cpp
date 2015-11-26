/*
 * Group.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: cliff
 */

#include "Group.h"

Group::Group() {  // create an empty Group
}

Group::Group(vector<shared_ptr<Organism>> _population, shared_ptr<Optimizer> _optimizer) {
  population = _population;
  optimizer = _optimizer;
  shared_ptr<Archivist> _archivist(new Archivist());
  archivist = _archivist;
}

Group::~Group() {
}

void Group::archive(int flush) {
  archivist->archive(population, flush);
}

void Group::optimize() {
  optimizer->makeNextGeneration(population);
}

