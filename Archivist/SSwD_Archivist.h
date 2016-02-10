//
//  SSwD_Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__SSwD_Archivist__
#define __BasicMarkovBrainTemplate__SSwD_Archivist__

#include "Archivist.h"

using namespace std;

class SSwD_Archivist : public Archivist {  // SnapShot with Delay
 public:
	static int& SSwD_Arch_dataInterval;  // how often to write out data
	static int& SSwD_Arch_genomeInterval;  // how often to write out genomes
	static int& SSwD_Arch_dataIntervalDelay;  // when using SSwD, how long is the delay
	static int& SSwD_Arch_genomeIntervalDelay;  // when using SSwD, how long is the delay
	static int& SSwD_Arch_cleanupInterval;  // how often to attempt to prune the LOD
	static string& SSwD_Arch_DataFilePrefix;  // name of the Data file
	static string& SSwD_Arch_GenomeFilePrefix;  // name of the Genome file (genomes on LOD)
	static bool& SSwD_Arch_writeDataFiles;  // if true, write data file
	static bool& SSwD_Arch_writeGenomeFiles;  // if true, write genome file

	int dataInterval;  // how often to write out data
	int genomeInterval;  // how often to write out genomes
	int dataIntervalDelay;  // when using SSwD, how long is the delay
	int genomeIntervalDelay;  // when using SSwD, how long is the delay
	int cleanupInterval;  // how often to attempt to prune the LOD
	string DataFilePrefix;  // name of the Data file
	string GenomeFilePrefix;  // name of the Genome file (genomes on LOD)
	bool writeDataFiles;  // if true, write data file
	bool writeGenomeFiles;  // if true, write genome file

	//// info about files under management
	int nextDataWrite;  // next time data files will be written to disk
	int nextGenomeWrite;  // next time a genome file will be written to genome.csv
	int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
	int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

	unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
	// key is Global::nextGenomeWrite or Global::nextDataWrite

	SSwD_Archivist();

	virtual ~SSwD_Archivist() = default;

	///// CLEANUP / DELETE STALE CHECKPOINTS
	// if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
	// and all of it's org parents (with clear) assuming org was dead at the time
	// this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.
	void cleanup();

	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0);
};

#endif /* defined(__BasicMarkovBrainTemplate__SSwD_Archivist__) */
