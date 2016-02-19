#include "snapshot_Archivist.h"

int& Snapshot_Archivist::SS_Arch_dataInterval = Parameters::register_parameter("dataInterval_SS", 10, "How often to save a data file", "ARCHIVIST_SNAPSHOT");
int& Snapshot_Archivist::SS_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SS", 10, "How often to save a genome file", "ARCHIVIST_SNAPSHOT");
string& Snapshot_Archivist::SS_Arch_DataFilePrefix = Parameters::register_parameter("dataFilePrefix_SS", (string) "data", "name of genome file (stores genomes)", "ARCHIVIST_SNAPSHOT");
string& Snapshot_Archivist::SS_Arch_GenomeFilePrefix = Parameters::register_parameter("genomeFilePrefix_SS", (string) "genome", "name of data file (stores everything but genomes)", "ARCHIVIST_SNAPSHOT");
bool& Snapshot_Archivist::SS_Arch_writeDataFiles = Parameters::register_parameter("writeDataFiles_SS", true, "if true, data files will be written", "ARCHIVIST_SNAPSHOT");
bool& Snapshot_Archivist::SS_Arch_writeGenomeFiles = Parameters::register_parameter("writeGenomeFiles_SS", true, "if true, genome files will be written", "ARCHIVIST_SNAPSHOT");

Snapshot_Archivist::Snapshot_Archivist()
		: Archivist() {
	dataInterval = SS_Arch_dataInterval;
	genomeInterval = SS_Arch_genomeInterval;
	DataFilePrefix = SS_Arch_DataFilePrefix;
	GenomeFilePrefix = SS_Arch_GenomeFilePrefix;
	writeDataFiles = SS_Arch_writeDataFiles;
	writeGenomeFiles = SS_Arch_writeGenomeFiles;
}

void Snapshot_Archivist::saveSnapshotData(vector<shared_ptr<Organism>> population, int update) {
	// write out data
	string dataFileName = DataFilePrefix + "_" + to_string(update) + ".csv";

	if (files.find("data") == files.end()) {  // first make sure that the dataFile has been set up.
		files["data"] = population[0]->dataMap.getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
	}
	for (auto org : population) {
		org->dataMap.writeToFile(dataFileName, files["data"]);  // append new data to the file
	}
}

void Snapshot_Archivist::saveSnapshotGenomes(vector<shared_ptr<Organism>> population, int update) {

	// write out genomes
	string genomeFileName = GenomeFilePrefix + "_" + to_string(update) + ".csv";

	string dataString;
	for (auto org : population) {
		//dataString = to_string(org->ID) + FileManager::separator + "\"[" + org->genome->genomeToStr() + "]\"";  // add interval update, genome ancestors, and genome with padding to string
		//FileManager::writeToFile(genomeFileName, dataString, "ID,genome");  // write data to file
		org->genome->dataMap.Set("sites",org->genome->genomeToStr());
		org->genome->dataMap.Set("ID",org->dataMap.Get("ID"));
		//org->genome->dataMap.writeToFile(genomeFileName, org->genome->dataMap.getKeys());  // append new data to the file
		org->genome->dataMap.writeToFile(genomeFileName,Genome::genomeFileColumns);  // append new data to the file
	}
}

bool Snapshot_Archivist::archive(vector<shared_ptr<Organism>> population, int flush) {
	if (flush != 1) {

		if (Global::update % realtimeFilesInterval == 0) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to dominant and average files
		}
		if ((Global::update % SS_Arch_dataInterval == 0) && (flush == 0) && writeDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotData(population, Global::update);
		}
		if ((Global::update % SS_Arch_genomeInterval == 0) && (flush == 0) && writeGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotGenomes(population, Global::update);
		}
		for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
			org->clearHistory();
		}
	}
	// if we are at the end of the run
	return (Global::update >= Global::updates);
}
