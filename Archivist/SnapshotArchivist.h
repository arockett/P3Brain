//
//  snapshot_Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__snapshot_Archivist__
#define __BasicMarkovBrainTemplate__snapshot_Archivist__

#include "DefaultArchivist.h"

using namespace std;

class SnapshotArchivist : public DefaultArchivist {  // SnapShot
 public:
	static shared_ptr< int> SS_Arch_dataInterval;  // how often to save data
	static shared_ptr< int> SS_Arch_genomeInterval;  // how often to save genomes
	static shared_ptr< string> SS_Arch_DataFilePrefix;  // name of the Data file
	static shared_ptr< string> SS_Arch_GenomeFilePrefix;  // name of the Genome file (genomes on LOD)
	static shared_ptr< bool> SS_Arch_writeDataFiles;  // if true, write data file
	static shared_ptr< bool> SS_Arch_writeGenomeFiles;  // if true, write genome file

	int dataInterval;
	int genomeInterval;
	string DataFilePrefix;  // name of the Data file
	string GenomeFilePrefix;  // name of the Genome file (genomes on LOD)
	bool writeDataFiles;  // if true, write data file
	bool writeGenomeFiles;  // if true, write genome file

	SnapshotArchivist(vector<string> aveFileColumns = {});

	~SnapshotArchivist() = default;

	void saveSnapshotData(vector<shared_ptr<Organism>> population, int update);

	void saveSnapshotGenomes(vector<shared_ptr<Organism>> population, int update);

	virtual bool archive(vector<shared_ptr<Organism>> population, int flush = 0) override;

};

#endif /* defined(__BasicMarkovBrainTemplate__snapshot_Archivist__) */
