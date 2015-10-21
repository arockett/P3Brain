#include <cstring>
#include <memory>

#include "Data.h"
#include "Global.h"

//global variables that should be accessible to all
int Data::lastSaveUpdate = 0;
bool Data::initFiles = false;

/*
 * adds an int value at key to a map<string,string>
 */
void Data::Add(int value, const string& key, map<string, string>& dataMap) {
	dataMap[key] = to_string(value);
}

/*
 * adds a double value at key to a map<string,string>
 */
void Data::Add(double value, const string& key, map<string, string>& dataMap) {
	dataMap[key] = to_string(value);
}

/*
 * takes a returns a value for key given a map<string,string>
 */
string Data::Get(string key, map<string, string> dataMap) {
	return dataMap[key];
}





/*
 * Save the data from dataMap and genomes for one genomes ("from") LOD from the last save until the last convergance
 * this function assumes
 * a) all of the fields in dataMap that genomes will be writting to have been created
 * b) all genomes have the same fields
 * if a) and or b) are not true, the function will still work, but the output will be mislabeled garbage
 *
 * if requireMRCA then data will not be output for times were the LOD has not yet converged
 * else set current agent as MRCA and output all data (used for early termination)
 */
void Data::saveDataOnLOD(Genome * who,int requireMRCA) {
	FILE *DATAFILE;
	FILE *GENOMEFILE;

	string local_DataFileName = "OUTPUT" + to_string(Global::repNumber) + "/" + Global::DataFileName;
	string local_GenomeFileName = "OUTPUT" + to_string(Global::repNumber) + "/" + Global::GenomeFileName;

	if (initFiles != true) { // if this file has not been initialized
		initFiles = true; // make a note that it has been initialized
		DATAFILE = fopen(local_DataFileName.c_str(), "w+t"); // open the data file for writing (w) in text mode (t) and overwrite the file if it's already there (+)

		// write the file header (labels)
		fprintf(DATAFILE, "update"); // print update in the file
		for (map<string, string>::iterator I = who->data.begin(); I != who->data.end(); I++) {
			fprintf(DATAFILE, ",%s", I->first.c_str()); // print the names of all the the elements in dataMap as a header
		}
		fprintf(DATAFILE, "\n");

		GENOMEFILE = fopen(local_GenomeFileName.c_str(), "w+t"); // open the genome file for writing (w) in text mode (t) and overwrite the file if it's already there (+)
		fprintf(GENOMEFILE, "genomeInterval = %i\n", Global::genomeInterval);

	} else { // if files have already been initialized, open them for writing
		DATAFILE = fopen(local_DataFileName.c_str(), "at"); // open the file for writing (w) in text mode (t)
		GENOMEFILE = fopen(local_GenomeFileName.c_str(), "at"); // open the file for writing (w) in text mode (t)

	}
	Genome* MRCA;
	vector<Genome*> LOD = who->getLOD(); // get line of decent for "from"
	if (requireMRCA){
		MRCA = who->getMostRecentCommonAncestor();
	} else {
		MRCA = who;
	}
	// cout << "IN saveDataOnLOD : MRCA = " << MRCA->ID << "\n";
	int outputTime = 0; // this update, but relitive to the last save update

	// while we have not caught up with the current update
	// and we have not passed the MRCA
	// and we have not passed Global::updates (how long we wanted this to run)
	while ((outputTime + Data::lastSaveUpdate < Global::update) && (LOD[outputTime] != MRCA)
			&& (outputTime + Data::lastSaveUpdate < Global::updates)) {

		// write data to DATAFILE if this is a dataInterval
		if ((outputTime + Data::lastSaveUpdate) % Global::dataInterval == 0) {
			fprintf(DATAFILE, "%i", outputTime + Data::lastSaveUpdate); // save the update number
			for (map<string, string>::iterator genomeData = LOD[outputTime]->data.begin();
					genomeData != LOD[outputTime]->data.end(); genomeData++) {
				fprintf(DATAFILE, ",%s", genomeData->second.c_str()); //save the data for each element in the list
			}
			fprintf(DATAFILE, "\n");
		}

		if ((outputTime + Data::lastSaveUpdate) % Global::genomeInterval == 0) {
			LOD[outputTime]->saveToFile(GENOMEFILE); // write the Genome to GENOMEFILE if this is a genomeInterval
		}
		outputTime++;
	}

	Data::lastSaveUpdate = Data::lastSaveUpdate + outputTime; // update lastSaveUpdate, so we know where to start next time
	fclose(DATAFILE);
	fclose(GENOMEFILE);

}
