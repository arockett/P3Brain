#include "LODwAP_Archivist.h"

const int& LODwAP_Archivist::LODwAP_Arch_dataInterval = Parameters::register_parameter("dataInterval_LODwAP", 100, "How often to write to data file", "ARCHIVIST_LODWAP");
const int& LODwAP_Archivist::LODwAP_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_LODwAP", 1000, "How often to write genome file", "ARCHIVIST_LODWAP");
const int& LODwAP_Archivist::LODwAP_Arch_pruneInterval = Parameters::register_parameter("pruneInterval_LODwAP", 100, "How often to attempt to prune LOD and actually write out to files", "ARCHIVIST_LODWAP");
const int& LODwAP_Archivist::LODwAP_Arch_terminateAfter = Parameters::register_parameter("terminateAfter_LODwAP", 100, "how long to run after updates (to get better coalescence)", "ARCHIVIST_LODWAP");
const string& LODwAP_Archivist::LODwAP_Arch_DataFileName = Parameters::register_parameter("dataFileName_LODwAP", (string) "data.csv", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_LODWAP");
const string& LODwAP_Archivist::LODwAP_Arch_GenomeFileName = Parameters::register_parameter("genomeFileName_LODwAP", (string) "genome.csv", "name of data file (stores everything but genomes)", "ARCHIVIST_LODWAP");
const bool& LODwAP_Archivist::LODwAP_Arch_writeDataFile = Parameters::register_parameter("writeDataFile_LODwAP", true, "if true, a data file will be written", "ARCHIVIST_LODWAP");
const bool& LODwAP_Archivist::LODwAP_Arch_writeGenomeFile = Parameters::register_parameter("writeGenomeFile_LODwAP", true, "if true, a genome file will be written", "ARCHIVIST_LODWAP");

LODwAP_Archivist::LODwAP_Archivist(vector<string> aveFileColumns) :
		Archivist(aveFileColumns) {

	dataInterval = LODwAP_Arch_dataInterval;
	genomeInterval = LODwAP_Arch_genomeInterval;
	pruneInterval = LODwAP_Arch_pruneInterval;
	terminateAfter = LODwAP_Arch_terminateAfter;
	DataFileName = LODwAP_Arch_DataFileName;
	GenomeFileName = LODwAP_Arch_GenomeFileName;
	writeDataFile = LODwAP_Arch_writeDataFile;
	writeGenomeFile = LODwAP_Arch_writeGenomeFile;

	nextDataWrite = 0;
	nextGenomeWrite = 0;
	nextDataCheckPoint = 0;
	nextGenomeCheckPoint = 0;

	lastPrune = 0;
}

bool LODwAP_Archivist::archive(vector<shared_ptr<Organism>> population, int flush) {

	if ((Global::update % dataInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
		writeRealTimeFiles(population);  // write to dominant and average files
	}
	if ((Global::update % pruneInterval == 0) || (flush == 1)) {

		if (files.find("data.csv") == files.end()) {  // if file has not be initialized yet
			files[DataFileName] = population[0]->dataMap.getKeys();  // store keys from data map associated with file name
		}

		// get the MRCA
		vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
		shared_ptr<Organism> effective_MRCA;
		if (flush) {  // if flush then we don't care about coalescence
			cout << "flushing LODwAP: using population[0] as Most Recent Common Ancestor (MRCA)" << endl;
			effective_MRCA = population[0]->parents[0];  // this assumes that a population was created, but not tested at the end of the evolution loop!
		} else {
			effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
		}

		// Save Data
		if (writeDataFile) {
			while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updates)) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
				for (auto file : files) {  // for each file in files
					current->dataMap.writeToFile(file.first, file.second);  // append new data to the file
				}
				nextDataWrite += dataInterval;
			}
		}

		//Save Genomes
		if (writeGenomeFile) {

			while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updates)) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
				//string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->genomeToStr() + "]\"";  // add write update and padding to genome string
				//FileManager::writeToFile(GenomeFileName, dataString, "update,genome");  // write data to file
				current->genome->dataMap.Set("sites", current->genome->genomeToStr());
				current->genome->dataMap.Set("update", nextGenomeWrite);
				current->genome->dataMap.Set("ID", current->dataMap.Get("ID"));
				//current->genome->dataMap.writeToFile(GenomeFileName, current->genome->dataMap.getKeys());  // append new data to the file
				current->genome->dataMap.writeToFile(GenomeFileName, current->genome->genomeFileColumns);  // append new data to the file
				nextGenomeWrite += genomeInterval;
			}
		}
		// data and genomes have now been written out up till the MRCA
		// so all data and genomes from before the MRCA can be deleted
		effective_MRCA->parents.clear();
		lastPrune = effective_MRCA->timeOfBirth;  // this will hold the time of the oldest genome in RAM
	}

	// if we have reached the end of time OR we have pruned past updates (i.e. written out all data up to updates), then we ae done!
	return (Global::update >= Global::updates + terminateAfter || lastPrune >= Global::updates);
}
