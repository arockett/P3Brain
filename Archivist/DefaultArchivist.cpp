#include "DefaultArchivist.h"
using namespace std;

shared_ptr<string> DefaultArchivist::Arch_outputMethodStr = Parameters::register_parameter("ARCHIVIST_OUTPUT_METHOD-outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), snapshot, SSwD (SnapShot with Delay)]");  // string parameter for outputMethod;

shared_ptr<int> DefaultArchivist::Arch_realtimeFilesInterval = Parameters::register_parameter("ARCHIVIST-realtimeFilesInterval", 10, "How often to write to realtime data files");
shared_ptr<bool> DefaultArchivist::Arch_writeAveFile = Parameters::register_parameter("ARCHIVIST-writeAveFile", true, "Save data to average file?");
shared_ptr<bool> DefaultArchivist::Arch_writeDominantFile = Parameters::register_parameter("ARCHIVIST-writeDominantFile", true, "Save data to dominant file?");
shared_ptr<string> DefaultArchivist::Arch_AveFileName = Parameters::register_parameter("ARCHIVIST-aveFileName", (string) "ave.csv", "name of average file (saves population averages)");
shared_ptr<string> DefaultArchivist::Arch_DominantFileName = Parameters::register_parameter("ARCHIVIST-dominantFileName", (string) "dominant.csv", "name of dominant file (saves data on dominant organism)");
shared_ptr<string> DefaultArchivist::Arch_DefaultAveFileColumnNames = Parameters::register_parameter("ARCHIVIST-aveFileColumns", (string) "[]", "data to be saved into average file (must be values that can generate an average). If empty, MABE will try to figure it out");
shared_ptr<bool> DefaultArchivist::Arch_DominantFileShowAllLists = Parameters::register_parameter("ARCHIVIST-dominantFileShowAllLists", true, "lists named 'all'* in data map will be averaged and added to file. if true, raw 'all'* lists will also be added to the file");

DefaultArchivist::DefaultArchivist() {
	realtimeFilesInterval = *Arch_realtimeFilesInterval;
	writeAveFile = *Arch_writeAveFile;
	writeDominantFile = *Arch_writeDominantFile;
	AveFileName = *Arch_AveFileName;
	DominantFileName = *Arch_DominantFileName;
	DominantFileShowAllLists = *Arch_DominantFileShowAllLists;
	finished = false;
}

DefaultArchivist::DefaultArchivist(vector<string> aveFileColumns) {
	realtimeFilesInterval = *Arch_realtimeFilesInterval;
	writeAveFile = *Arch_writeAveFile;
	writeDominantFile = *Arch_writeDominantFile;
	AveFileName = *Arch_AveFileName;
	DominantFileName = *Arch_DominantFileName;
	DominantFileShowAllLists = *Arch_DominantFileShowAllLists;
	finished = false;
	convertCSVListToVector(*Arch_DefaultAveFileColumnNames, DefaultAveFileColumns);
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

// save data and manage in memory data
// return true if next save will be > updates + terminate after
bool DefaultArchivist::archive(vector<shared_ptr<Organism>> population, int flush) {
	if (flush != 1) {
		if ((Global::update % realtimeFilesInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to dominant and average files
		}
		for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
			org->clearHistory();
		}
	}
	// if we are at the end of the run
	return (Global::update >= *Global::updates);
}
