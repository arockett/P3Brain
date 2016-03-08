//
//  LODwAP_Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__LODwAP_Archivist__
#define __BasicMarkovBrainTemplate__LODwAP_Archivist__

#include "Archivist.h"

using namespace std;

class LODwAP_Archivist : public Archivist {  // Line of Decent with Active Pruning
 public:

	static int& LODwAP_Arch_dataInterval;  // how often to write out data
	static int& LODwAP_Arch_genomeInterval;  // how often to write out genomes
	static int& LODwAP_Arch_pruneInterval;  // how often to attempt to prune the LOD
	static int& LODwAP_Arch_terminateAfter;  // how long to run after updates (to get better coalescence)
	static string& LODwAP_Arch_DataFileName;  // name of the Data file
	static string& LODwAP_Arch_GenomeFileName;  // name of the Genome file (genomes on LOD)
	static bool& LODwAP_Arch_writeDataFile;  // if true, write data file
	static bool& LODwAP_Arch_writeGenomeFile;  // if true, write genome file

	int dataInterval;  // how often to write out data
	int genomeInterval;  // how often to write out genomes
	int pruneInterval;  // how often to attempt to prune the LOD
	int terminateAfter;  // how long to run after updates (to get better coalescence)
	string DataFileName;  // name of the Data file
	string GenomeFileName;  // name of the Genome file (genomes on LOD)
	bool writeDataFile;  // if true, write data file
	bool writeGenomeFile;  // if true, write genome file

	int lastPrune;  // last time Genome was Pruned

	//// info about files under management
	int nextDataWrite;  // next time data files will be written to disk
	int nextGenomeWrite;  // next time a genome file will be written to genome.csv
	int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
	int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

	//unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
	//// key is Global::nextGenomeWrite or Global::nextDataWrite

	LODwAP_Archivist();

	virtual ~LODwAP_Archivist() = default;

	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) override;
};

#endif /* defined(__BasicMarkovBrainTemplate__LODwAP_Archivist__) */
