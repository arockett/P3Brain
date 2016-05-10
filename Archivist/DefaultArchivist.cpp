#include "DefaultArchivist.h"
using namespace std;

shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_outputMethodStrPL = Parameters::register_parameter("ARCHIVIST_OUTPUT_METHOD-outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), SSwD (SnapShot with Delay)]");  // string parameter for outputMethod;

shared_ptr<ParameterLink<int>> DefaultArchivist::Arch_realtimeFilesIntervalPL = Parameters::register_parameter("ARCHIVIST-realtimeFilesInterval", 10, "How often to write to realtime data files");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeAveFilePL = Parameters::register_parameter("ARCHIVIST-writeAveFile", true, "Save data to average file?");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_writeDominantFilePL = Parameters::register_parameter("ARCHIVIST-writeDominantFile", true, "Save data to dominant file?");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_AveFileNamePL = Parameters::register_parameter("ARCHIVIST-aveFileName", (string) "ave.csv", "name of average file (saves population averages)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DominantFileNamePL = Parameters::register_parameter("ARCHIVIST-dominantFileName", (string) "dominant.csv", "name of dominant file (saves data on dominant organism)");
shared_ptr<ParameterLink<string>> DefaultArchivist::Arch_DefaultAveFileColumnNamesPL = Parameters::register_parameter("ARCHIVIST-aveFileColumns", (string) "[]", "data to be saved into average file (must be values that can generate an average). If empty, MABE will try to figure it out");
shared_ptr<ParameterLink<bool>> DefaultArchivist::Arch_DominantFileShowAllListsPL = Parameters::register_parameter("ARCHIVIST-dominantFileShowAllLists", true, "lists named 'all'* in data map will be averaged and added to file. if true, raw 'all'* lists will also be added to the file");

shared_ptr<ParameterLink<int>> DefaultArchivist::SS_Arch_dataIntervalPL = Parameters::register_parameter("ARCHIVIST-snapshotDataInterval", 100, "How often to save a snapshot data file");
shared_ptr<ParameterLink<int>> DefaultArchivist::SS_Arch_genomeIntervalPL = Parameters::register_parameter("ARCHIVIST-snapshotGenomeInterval", 1000, "How often to save a snapshot genome file");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_DataFilePrefixPL = Parameters::register_parameter("ARCHIVIST-snapshotDataFilePrefix", (string) "snapshotData", "prefix for name of snapshot genome file (stores genomes)");
shared_ptr<ParameterLink<string>> DefaultArchivist::SS_Arch_GenomeFilePrefixPL = Parameters::register_parameter("ARCHIVIST-snapshotGenomeFilePrefix", (string) "snapshotGenome", "prefix for name of snapshot data file (stores everything but genomes)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeDataFilesPL = Parameters::register_parameter("ARCHIVIST-writeSnapshotDataFiles", false, "if true, snapshot data files will be written (with all non genome data for entire population)");
shared_ptr<ParameterLink<bool>> DefaultArchivist::SS_Arch_writeGenomeFilesPL = Parameters::register_parameter("ARCHIVIST-writeSnapshotGenomeFiles", false, "if true, snapshot genome files will be written (with all genomes for entire population)");

DefaultArchivist::DefaultArchivist(shared_ptr<ParametersTable> _PT) {
	PT = _PT;
	realtimeFilesInterval = (PT == nullptr) ? Arch_realtimeFilesIntervalPL->lookup() : PT->lookupInt("ARCHIVIST-realtimeFilesInterval");
	writeAveFile = (PT == nullptr) ? Arch_writeAveFilePL->lookup() : PT->lookupBool("ARCHIVIST-writeAveFile");
	writeDominantFile = (PT == nullptr) ? Arch_writeDominantFilePL->lookup() : PT->lookupBool("ARCHIVIST-writeDominantFile");
	AveFileName = (PT == nullptr) ? Arch_AveFileNamePL->lookup() : PT->lookupString("ARCHIVIST-aveFileName");
	DominantFileName = (PT == nullptr) ? Arch_DominantFileNamePL->lookup() : PT->lookupString("ARCHIVIST-dominantFileName");
	AveFileColumnNames = (PT == nullptr) ? Arch_DefaultAveFileColumnNamesPL->lookup() : PT->lookupString("ARCHIVIST-aveFileColumns");
	DominantFileShowAllLists = (PT == nullptr) ? Arch_DominantFileShowAllListsPL->lookup() : PT->lookupBool("ARCHIVIST-dominantFileShowAllLists");

	dataInterval = SS_Arch_dataIntervalPL->lookup();
	genomeInterval = SS_Arch_genomeIntervalPL->lookup();
	DataFilePrefix = SS_Arch_DataFilePrefixPL->lookup();
	GenomeFilePrefix = SS_Arch_GenomeFilePrefixPL->lookup();
	writeSnapshotDataFiles = SS_Arch_writeDataFilesPL->lookup();
	writeSnapshotGenomeFiles = SS_Arch_writeGenomeFilesPL->lookup();

	finished = false;
}

DefaultArchivist::DefaultArchivist(vector<string> aveFileColumns, shared_ptr<ParametersTable> _PT) : DefaultArchivist(){
	convertCSVListToVector(AveFileColumnNames, DefaultAveFileColumns);
	if (DefaultAveFileColumns.size() <= 0) {
		DefaultAveFileColumns = aveFileColumns;
	}
}

//save dominant and average file data
//keys named all* will be converted to *. These should key for lists of values. These values will be averaged (used to average world repeats)
void DefaultArchivist::writeRealTimeFiles(vector<shared_ptr<Organism>> &population) {
	// write out Average data
	if (writeAveFile) {
		double aveValue, temp;
		DataMap AveMap;
		for (auto key : DefaultAveFileColumns) {
			temp = 0;
			aveValue = 0;
			for (auto org : population) {
				if (org->dataMap.fieldExists(key)) {
					stringstream ss(org->dataMap.Get(key));
					ss >> temp;
				} else {  // if field not found, check if there is an all()s version
					string allKey = "all" + key;
					//cout << allKey << endl;
					if (org->dataMap.fieldExists(allKey)) {
						string dataList = org->dataMap.Get(allKey);
						//cout << dataList << endl;
						temp = 0;
						vector<double> values;
						convertCSVListToVector(dataList, values);
						for (auto v : values) {
							temp += v;
							//cout << key << " " << allKey << " " << v << " " << temp << endl;
						}
						temp /= (double) values.size();
					} else {
						cout << "WARNING:  In Archivist::writeRealTimeFiles(vector<shared_ptr<Organism>> &population) key \"" << key << "\" could not be found in dataMap!" << endl;
					}
				}
				aveValue += temp;
			}
			aveValue /= population.size();
			AveMap.Set(key, aveValue);
		}
		AveMap.writeToFile(AveFileName, DefaultAveFileColumns);
	}
	// write out Dominant data
	if (writeDominantFile) {
		vector<double> Scores;
		for (auto org : population) {
			Scores.push_back(org->score);
		}

		int best = findGreatestInVector(Scores);
		DataMap DomMap;
		for (auto key : population[best]->dataMap.getKeys()) {
			if (key[0] == 'a' && key[1] == 'l' && key[2] == 'l') {
				double temp = 0;
				vector<double> values;
				convertCSVListToVector(population[best]->dataMap.Get(key), values);
				for (auto v : values) {
					temp += v;
					//cout << key << " " << allKey << " " << v << " " << temp << endl;
				}
				temp /= (double) values.size();
				DomMap.Set(key.substr(3, key.size() - 1), temp);
			}
			if (DominantFileShowAllLists) {
				DomMap.Set(key, population[best]->dataMap.Get(key));
			}
		}
		DomMap.writeToFile(DominantFileName);
	}
}


void DefaultArchivist::saveSnapshotData(vector<shared_ptr<Organism>> population, int update) {
	// write out data
	string dataFileName = DataFilePrefix + "_" + to_string(update) + ".csv";

	if (files.find("data") == files.end()) {  // first make sure that the dataFile has been set up.
		population[0]->dataMap.Set("ancestors", "placeHolder");  // add ancestors so it will be in files (holds columns to be output for each file)
		files["data"] = population[0]->dataMap.getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
	}
	for (auto org : population) {
		population[0]->dataMap.Clear("ancestors");
		for (auto ancestor : org->ancestors) {
			org->dataMap.Append("ancestors", ancestor);
		}
		org->ancestors.clear();
		org->ancestors.insert(org->ID);  // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
		org->dataMap.writeToFile(dataFileName, files["data"]);  // append new data to the file
	}
}

void DefaultArchivist::saveSnapshotGenomes(vector<shared_ptr<Organism>> population, int update) {

	// write out genomes
	string genomeFileName = GenomeFilePrefix + "_" + to_string(update) + ".csv";

	string dataString;
	for (auto org : population) {
		//dataString = to_string(org->ID) + FileManager::separator + "\"[" + org->genome->genomeToStr() + "]\"";  // add interval update, genome ancestors, and genome with padding to string
		//FileManager::writeToFile(genomeFileName, dataString, "ID,genome");  // write data to file

		org->genome->dataMap.Set("sites", org->genome->genomeToStr());
		org->genome->dataMap.Set("ID", org->dataMap.Get("ID"));
		org->genome->dataMap.Set("update", org->dataMap.Get("update"));

		//org->genome->dataMap.writeToFile(genomeFileName, org->genome->dataMap.getKeys());  // append new data to the file
		org->genome->dataMap.writeToFile(genomeFileName, org->genome->genomeFileColumns);  // append new data to the file
		org->genome->dataMap.Clear("sites");  // this is large, clean it up now!
	}
}
// save data and manage in memory data
// return true if next save will be > updates + terminate after
bool DefaultArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {
	if (flush != 1) {
		if ((Global::update % realtimeFilesInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to dominant and average files
		}

		if ((Global::update % dataInterval == 0) && (flush == 0) && writeSnapshotDataFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotData(population, Global::update);
		}
		if ((Global::update % genomeInterval == 0) && (flush == 0) && writeSnapshotGenomeFiles) {  // do not write files on flush - these organisms have not been evaluated!
			saveSnapshotGenomes(population, Global::update);
		}

		for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
			org->clearHistory();
		}
	}
	// if we are at the end of the run
	return (Global::update >= Global::updatesPL->lookup());
}
