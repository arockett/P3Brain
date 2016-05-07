#include "LODwAPArchivist.h"

shared_ptr<int> LODwAPArchivist::LODwAP_Arch_dataInterval = Parameters::register_parameter("ARCHIVIST_LODWAP-dataInterval", 100, "How often to write to data file");
shared_ptr<int> LODwAPArchivist::LODwAP_Arch_genomeInterval = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeInterval", 1000, "How often to write genome file");
shared_ptr<int> LODwAPArchivist::LODwAP_Arch_pruneInterval = Parameters::register_parameter("ARCHIVIST_LODWAP-pruneInterval", 100, "How often to attempt to prune LOD and actually write out to files");
shared_ptr<int> LODwAPArchivist::LODwAP_Arch_terminateAfter = Parameters::register_parameter("ARCHIVIST_LODWAP-terminateAfter", 100, "how long to run after updates (to get better coalescence)");
shared_ptr<string> LODwAPArchivist::LODwAP_Arch_DataFileName = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileName", (string) "data.csv", "name of genome file (stores genomes for line of decent)");
shared_ptr<string> LODwAPArchivist::LODwAP_Arch_GenomeFileName = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeFileName", (string) "genome.csv", "name of data file (stores everything but genomes)");
shared_ptr<bool> LODwAPArchivist::LODwAP_Arch_writeDataFile = Parameters::register_parameter("ARCHIVIST_LODWAP-writeDataFile", true, "if true, a data file will be written");
shared_ptr<bool> LODwAPArchivist::LODwAP_Arch_writeGenomeFile = Parameters::register_parameter("ARCHIVIST_LODWAP-writeGenomeFile", true, "if true, a genome file will be written");
shared_ptr<bool> LODwAPArchivist::LODwAP_Arch_dataFileShowAllLists = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileShowAllLists", true, "if true, lists named 'all'* in data map will be saved");
shared_ptr<bool> LODwAPArchivist::LODwAP_Arch_dataFileConvertAllLists = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileConvertAllLists", true, "if true, lists named 'all'* in data map will be averaged and added to file");

LODwAPArchivist::LODwAPArchivist(vector<string> aveFileColumns) :
		DefaultArchivist(aveFileColumns) {

	dataInterval = *LODwAP_Arch_dataInterval;
	genomeInterval = *LODwAP_Arch_genomeInterval;
	pruneInterval = *LODwAP_Arch_pruneInterval;
	terminateAfter = *LODwAP_Arch_terminateAfter;
	DataFileName = *LODwAP_Arch_DataFileName;
	GenomeFileName = *LODwAP_Arch_GenomeFileName;
	writeDataFile = *LODwAP_Arch_writeDataFile;
	writeGenomeFile = *LODwAP_Arch_writeGenomeFile;
	dataFileShowAllLists = *LODwAP_Arch_dataFileShowAllLists;
	dataFileConvertAllLists = *LODwAP_Arch_dataFileConvertAllLists;

	nextDataWrite = 0;
	nextGenomeWrite = 0;
	nextDataCheckPoint = 0;
	nextGenomeCheckPoint = 0;

	lastPrune = 0;
}

bool LODwAPArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {

	if ((Global::update % dataInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
		writeRealTimeFiles(population);  // write to dominant and average files
	}
	if ((Global::update % pruneInterval == 0) || (flush == 1)) {

		if (files.find("data.csv") == files.end()) {  // if file has not be initialized yet
			if (dataFileConvertAllLists) {
				DataMap TempMap;
				for (auto key : population[0]->dataMap.getKeys()) {
					if (key[0] == 'a' && key[1] == 'l' && key[2] == 'l') {
						double temp = 0;
						vector<double> values;
						convertCSVListToVector(population[0]->dataMap.Get(key), values);
						for (auto v : values) {
							temp += v;
							//cout << key << " " << allKey << " " << v << " " << temp << endl;
						}
						temp /= (double) values.size();
						TempMap.Set(key.substr(3, key.size() - 1), temp);
						if (dataFileShowAllLists) {
							TempMap.Set(key, population[0]->dataMap.Get(key));
						}
					} else {
						TempMap.Set(key, population[0]->dataMap.Get(key));
					}
				}
				files[DataFileName] = TempMap.getKeys();  // store keys from data map associated with file name
			} else {
				files[DataFileName] = population[0]->dataMap.getKeys();  // store keys from data map associated with file name
			}
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
			while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= *Global::updates)) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
				if (dataFileConvertAllLists) {
					DataMap TempMap;
					for (auto key : current->dataMap.getKeys()) {
						if (key[0] == 'a' && key[1] == 'l' && key[2] == 'l') {
							double temp = 0;
							vector<double> values;
							convertCSVListToVector(current->dataMap.Get(key), values);
							for (auto v : values) {
								temp += v;
								//cout << key << " " << allKey << " " << v << " " << temp << endl;
							}
							temp /= (double) values.size();
							TempMap.Set(key.substr(3, key.size() - 1), temp);
							if (dataFileShowAllLists) {
								TempMap.Set(key, current->dataMap.Get(key));
							}
						} else {
							TempMap.Set(key, current->dataMap.Get(key));
						}
					}
					for (auto file : files) {  // for each file in files
						TempMap.writeToFile(file.first, file.second);  // append new data to the file
					}
				} else {
					for (auto file : files) {  // for each file in files
						current->dataMap.writeToFile(file.first, file.second);  // append new data to the file
					}
				}
				nextDataWrite += dataInterval;
			}
		}

		//Save Genomes
		if (writeGenomeFile) {

			while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= *Global::updates)) {  // if there is convergence before the next data interval
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
	return (Global::update >= *Global::updates + terminateAfter || lastPrune >= *Global::updates);
}
