#include "Archivist.h"

string& Archivist::Arch_outputMethodStr = Parameters::register_parameter("outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), snapshot, SSwD (SnapShot with Delay)]", "ARCHIVIST - OUTPUT METHOD");  // string parameter for outputMethod;

int& Archivist::Arch_realtimeFilesInterval = Parameters::register_parameter("realtimeFilesInterval", 10, "How often to write to realtime data files", "ARCHIVIST");
bool& Archivist::Arch_writeAveFile = Parameters::register_parameter("writeAveFile", true, "Save data to average file?", "ARCHIVIST");
bool& Archivist::Arch_writeDominantFile = Parameters::register_parameter("writeDominantFile", true, "Save data to dominant file?", "ARCHIVIST");
string& Archivist::Arch_AveFileName = Parameters::register_parameter("aveFileName", (string) "ave.csv", "name of average file (saves population averages)", "ARCHIVIST");
string& Archivist::Arch_DominantFileName = Parameters::register_parameter("dominantFileName", (string) "dominant.csv", "name of dominant file (saves data on dominant organism)", "ARCHIVIST");
string& Archivist::Arch_DefaultAveFileColumnNames = Parameters::register_parameter("aveFileColumns", (string) "[update,score]", "data to be saved into average file (must be values that can generate an average)", "ARCHIVIST");

Archivist::Archivist() {
	realtimeFilesInterval = Arch_realtimeFilesInterval;
	writeAveFile = Arch_writeAveFile;
	writeDominantFile = Arch_writeDominantFile;
	AveFileName = Arch_AveFileName;
	DominantFileName = Arch_DominantFileName;
	convertCSVListToVector(Arch_DefaultAveFileColumnNames, DefaultAveFileColumns);
	finished = false;
}

//save dominant and average file data
void Archivist::writeRealTimeFiles(vector<shared_ptr<Organism>> &population) {
	// write out Average data
	if (writeAveFile) {
		double aveValue, temp;
		DataMap AveMap;
		for (auto key : DefaultAveFileColumns) {
			aveValue = 0;
			for (auto org : population) {
				stringstream ss(org->dataMap.Get(key));
				ss >> temp;
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
		population[best]->dataMap.writeToFile(DominantFileName);
	}
}

// save data and manage in memory data
// return true if next save will be > updates + terminate after
bool Archivist::archive(vector<shared_ptr<Organism>> population, int flush) {
	if (flush != 1) {
		if ((Global::update % realtimeFilesInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
			writeRealTimeFiles(population);  // write to dominant and average files
		}
		for (auto org : population) {  // we don't need to worry about tracking parents or lineage, so we clear out this data every generation.
			org->clearHistory();
		}
	}
	// if we are at the end of the run
	return (Global::update >= Global::updates);
}
