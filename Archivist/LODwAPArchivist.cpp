#include "LODwAPArchivist.h"

shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_dataIntervalPL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataInterval", 100, "How often to write to data file");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_genomeIntervalPL = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeInterval", 1000, "How often to write genome file");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_pruneIntervalPL = Parameters::register_parameter("ARCHIVIST_LODWAP-pruneInterval", 100, "How often to attempt to prune LOD and actually write out to files");
shared_ptr<ParameterLink<int>> LODwAPArchivist::LODwAP_Arch_terminateAfterPL = Parameters::register_parameter("ARCHIVIST_LODWAP-terminateAfter", 100, "how long to run after updates (to get better coalescence)");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_DataFileNamePL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileName", (string) "data.csv", "name of genome file (stores genomes for line of decent)");
shared_ptr<ParameterLink<string>> LODwAPArchivist::LODwAP_Arch_GenomeFileNamePL = Parameters::register_parameter("ARCHIVIST_LODWAP-genomeFileName", (string) "genome.csv", "name of data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeDataFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeDataFile", true, "if true, a data file will be written");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_writeGenomeFilePL = Parameters::register_parameter("ARCHIVIST_LODWAP-writeGenomeFile", true, "if true, a genome file will be written");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_dataFileShowAllListsPL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileShowAllLists", true, "if true, lists named 'all'* in data map will be saved");
shared_ptr<ParameterLink<bool>> LODwAPArchivist::LODwAP_Arch_dataFileConvertAllListsPL = Parameters::register_parameter("ARCHIVIST_LODWAP-dataFileConvertAllLists", true, "if true, lists named 'all'* in data map will be averaged and added to file");

LODwAPArchivist::LODwAPArchivist(vector<string> aveFileColumns, shared_ptr<ParametersTable> _PT) :
		DefaultArchivist(aveFileColumns,_PT) {

	//realtimeFilesInterval = (PT == nullptr) ? Arch_realtimeFilesIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_DEFAULT-realtimeFilesInterval");

	dataInterval = (PT == nullptr) ? LODwAP_Arch_dataIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-dataInterval");
	genomeInterval = (PT == nullptr) ? LODwAP_Arch_genomeIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-genomeInterval");
	pruneInterval = (PT == nullptr) ? LODwAP_Arch_pruneIntervalPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-pruneInterval");
	terminateAfter = (PT == nullptr) ? LODwAP_Arch_terminateAfterPL->lookup() : PT->lookupInt("ARCHIVIST_LODWAP-terminateAfter");
	DataFileName = (PT == nullptr) ? LODwAP_Arch_DataFileNamePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-dataFileName");
	GenomeFileName = (PT == nullptr) ? LODwAP_Arch_GenomeFileNamePL->lookup() : PT->lookupString("ARCHIVIST_LODWAP-genomeFileName");
	writeDataFile = (PT == nullptr) ? LODwAP_Arch_writeDataFilePL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-writeDataFile");
	writeGenomeFile = (PT == nullptr) ? LODwAP_Arch_writeGenomeFilePL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-writeGenomeFile");
	dataFileShowAllLists = (PT == nullptr) ? LODwAP_Arch_dataFileShowAllListsPL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-dataFileShowAllLists");
	dataFileConvertAllLists = (PT == nullptr) ? LODwAP_Arch_dataFileConvertAllListsPL->lookup() : PT->lookupBool("ARCHIVIST_LODWAP-dataFileConvertAllLists");

	nextDataWrite = 0;
	nextGenomeWrite = 0;
	nextDataCheckPoint = 0;
	nextGenomeCheckPoint = 0;

	lastPrune = 0;
}

bool LODwAPArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {

	if ((Global::update % realtimeFilesInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
		writeRealTimeFiles(population);  // write to dominant and average files
	}
	if ((Global::update % dataInterval == 0) && (flush == 0) && writeSnapshotDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
		saveSnapshotData(population);
	}
	if ((Global::update % genomeInterval == 0) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
		saveSnapshotGenomes(population);
	}

	if ((Global::update % pruneInterval == 0) || (flush == 1)) {

		if (files.find(DataFileName) == files.end()) {  // if file has not be initialized yet
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
			files[DataFileName].push_back("update");
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
			while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updatesPL->lookup())) {  // if there is convergence before the next data interval
				shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
				cout << nextDataWrite << " ::: " << lastPrune << endl;
				current->dataMap.Set("update", nextDataWrite);
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
//					for (auto file : files) {  // for each file in files
//						TempMap.writeToFile(file.first, file.second);  // append new data to the file
//					}
					TempMap.writeToFile(DataFileName,files[DataFileName]);
				} else {
//					for (auto file : files) {  // for each file in files
					current->dataMap.writeToFile(DataFileName,files[DataFileName]);  // append new data to the file
//					}
				}
				nextDataWrite += dataInterval;
			}
		}

		//Save Genomes
		if (writeGenomeFile) {

			while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updatesPL->lookup())) {  // if there is convergence before the next data interval
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
	return (Global::update >= Global::updatesPL->lookup() + terminateAfter || lastPrune >= Global::updatesPL->lookup());
}
