#include "Data.h"
#include <cstring>
#include <memory>

//global variables that should be accessible to all
int Data::update = 0;
int Data::lastSaveUpdate = 0;
set<int> Data::inUseGateTypes;
bool Data::initFiles = false;

// dataMap holds generic data
// key will always be a genome ID (unique) which adresses to a map of type of data to value (for this genome)
map<int, map<string, string>> Data::dataMap;

int& Data::repNumber = Parameters::register_parameter("repNumber", 101, "Replicate ID and seed (if seedWithPID not set true)", "GLOBAL");
bool& Data::seedWithPID = Parameters::register_parameter("seedWithPID", false,
		"if seedWithPID, random number generator will see with process ID", "GLOBAL");
int& Data::updates = Parameters::register_parameter("updates", 200, "how long the program will run", "GLOBAL");
int& Data::popSize = Parameters::register_parameter("popSize", 100, "number of genomes in the populaiton", "GLOBAL");
int& Data::dataInterval = Parameters::register_parameter("dataInterval", 1, "How often to write to data file","DATA");
int& Data::genomeInterval = Parameters::register_parameter("genomeInterval", 1, "How often to write genome file","DATA");
int& Data::pruneInterval = Parameters::register_parameter("pruneInterval", dataInterval, "How often to write to data file","DATA");
string& Data::DataFileName = Parameters::register_parameter("dataFileName", (string)"data.csv",
		"name of genome file (stores genomes for line of decent)","DATA");
string& Data::GenomeFileName = Parameters::register_parameter("genomeFileName", (string)"genome.csv",
		"name of data file (stores data i.e. scores","DATA");


// implementation for LOD
void Data::Add(int value, string key, map<string, string> & dataMap) {
	dataMap[key] = to_string(value);
}

void Data::Add(double value, string key, map<string, string> & dataMap) {
	dataMap[key] = to_string(value);
}

/*
 * takes a genome and a key. returns the value of key for genome from "dataMap"
 */
string Data::Get(string key, map<string, string> dataMap) {
	return dataMap[key];
}

/*
 * Given a genome and a key(to data that has been saved into "dataMap"
 * return a list of the value for key for genome and all genomes ancestors ordered oldest first
 */
vector<string> Data::GetLODItem(string key, Genome * from) {
	vector<string> list;
	Genome* G = from;
	while (G != NULL) {
		list.insert(list.begin(), dataMap[G->ID][key]);
		G = G->ancestor;
	}
	return list;
}

/*
 * Given a genome
 * return a list of genomes containing genome and all genomes ancestors ordered oldest first
 */
vector<Genome*> Data::getLOD(Genome * from) {
	vector<Genome*> list;
	Genome * G = from;
	while (G != NULL) { // which G has an ancestor
		//printf("IN getLOD - %i %i\n",G->ID,G->referenceCounter);
		list.insert(list.begin(), G); // add that ancestor to the front of the LOD list
		G = G->ancestor; // move to the ancestor
	}
	return list;
}

/*
 * find the Most Recent Common Ancestor
 * uses getLOD to get a list of ancestors (oldest first). seaches the list for the first ancestor with a referenceCounter > 1
 * that is the first reference counter with more then one offspring.
 * If none are found, then return "from"
 * Note: a currently active genome has a referenceCounter = 1 (it has not reproduced yet, it only has 1 for it's self)
 *       a dead genome with a referenceCounter = 0 will not be in the LOD (it has no offspring and will have been pruned)
 *       a dead genome with a referenceCounter = 1 has only one offspring.
 *       a dead genome with a referenceCounter > 1 has more then one spring with surviving lines of decent.
 */
Genome* Data::getMostRecentCommonAncestor(Genome * from) {
	vector<Genome*> LOD = getLOD(from); // get line of decent from "from"
	for (auto G : LOD) { // starting at the oldest ancestor, moving to the youngest
		//printf("IN getMRCA - %i %i\n",G->ID,G->referenceCounter);
		if (G->referenceCounter > 1) // the first (oldest) ancestor with more then one surviving offspring is the oldest
			return G;
	}
	return from; // a currently active genome will have referenceCounter = 1 but may be the Most Recent Common Ancestor
}

/*
 * Save the data from dataMap and genomes for one genomes ("from") LOD from the last save until the last convergance
 * this function assumes
 * a) all of the fields in dataMap that genomes will be writting to have been created
 * b) all genomes have the same fields
 * if a) and or b) are not true, the function will still work, but the output will be mislabeled garbage
 */
void Data::saveDataOnLOD(Genome * who) {
	FILE *DATAFILE;
	FILE *GENOMEFILE;

	string local_DataFileName = to_string(Data::repNumber) + "/" + Data::DataFileName;
	string local_GenomeFileName = to_string(Data::repNumber) + "/" + Data::GenomeFileName;

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
		fprintf(GENOMEFILE, "genomeInterval = %i\n", Data::genomeInterval);

	} else { // if files have already been initialized, open them for writing
		DATAFILE = fopen(local_DataFileName.c_str(), "at"); // open the file for writing (w) in text mode (t)
		GENOMEFILE = fopen(local_GenomeFileName.c_str(), "at"); // open the file for writing (w) in text mode (t)

	}

	vector<Genome*> LOD = getLOD(who); // get line of decent for "from"
	Genome* MRCA = getMostRecentCommonAncestor(who);
	// cout << "IN saveDataOnLOD : MRCA = " << MRCA->ID << "\n";

	int outputTime = 0; // this update, but relitive to the last save update

	// while we have not caught up with the current update
	// and we have not passed the MRCA
	// and we have not passed Data::updates (how long we wanted this to run)
	while ((outputTime + Data::lastSaveUpdate < update) && (LOD[outputTime] != MRCA)
			&& (outputTime + Data::lastSaveUpdate < Data::updates)) {

		// write data to DATAFILE if this is a dataInterval
		if ((outputTime + Data::lastSaveUpdate) % Data::dataInterval == 0) {
			fprintf(DATAFILE, "%i", outputTime + Data::lastSaveUpdate);
			//save the data for each element in the list
			for (map<string, string>::iterator genomeData = LOD[outputTime]->data.begin();
					genomeData != LOD[outputTime]->data.end(); genomeData++) {
				fprintf(DATAFILE, ",%s", genomeData->second.c_str());
			}
			fprintf(DATAFILE, "\n");
		}

		// write the Genome to GENOMEFILE if this is a genomeInterval
		if ((outputTime + Data::lastSaveUpdate) % Data::genomeInterval == 0) {
			LOD[outputTime]->saveToFile(GENOMEFILE);
		}
		outputTime++;
	}
	Data::lastSaveUpdate = Data::lastSaveUpdate + outputTime; // update lastSaveUpdate, so we know where to start next time
	fclose(DATAFILE);
	fclose(GENOMEFILE);

}

void Data::showAll() {
	for (auto& A : dataMap)
		for (auto& B : A.second)
			printf("%i %s %s\n", A.first, B.first.c_str(), B.second.c_str());
}

////////////////////////////////////// OLD, will be removed in time /////////////////////////////////////////////
//////////////////////////////////////      DO NOT EDIT BELOW!      /////////////////////////////////////////////
/*
 *
 *

 //Command Line Parameters
 map<string,int*> Data::parameterInt;
 map<string,double*> Data::parameterDouble;
 map<string,string*> Data::parameterString;
 map<string,bool*> Data::parameterBool;
 map<string,double*> Data::defaultDoubles;
 map<string,int*> Data::defaultInts;
 map<string,string*> Data::defaultStrings;
 map<string,bool*> Data::defaultBools;

 //implementation for parameters
 void Data::setDefaultParameter(string key,int *variable,int value){
 parameterInt[key]=variable;
 *variable=value;
 }

 void Data::setDefaultParameter(string key,double *variable,double value){
 parameterDouble[key]=variable;
 *variable=value;
 }

 void Data::setDefaultParameter(string key,string *variable,string value){
 parameterString[key]=variable;
 *variable=value;
 }

 void Data::setDefaultParameter(string key,bool *variable,bool value){
 parameterBool[key]=variable;
 *variable=value;
 }

 void Data::UseCommandLineParameters(int argc, const char * argv[]){
 for(int i=1;i<argc;i+=2){
 string S=string(argv[i]);
 if(parameterInt.find(S)!=parameterInt.end()){
 *parameterInt[S]=atoi(argv[i+1]);
 printf("set int %s to %i\n",S.c_str(),(*parameterInt[S]));
 }
 if(parameterDouble.find(S)!=parameterDouble.end()){
 *parameterDouble[S]=(double)atof(argv[i+1]);
 printf("set double %s to %f\n",S.c_str(),(*parameterDouble[S]));
 }
 if(parameterString.find(S)!=parameterString.end()){
 *parameterString[S]=string(argv[i+1]);
 printf("set string %s to %s\n",S.c_str(),(*parameterString[S]).c_str());
 }
 if(parameterBool.find(S)!=parameterBool.end()){
 *parameterBool[S]=(strcmp(argv[i+1],"true")==0);
 printf("set bool %s to %i\n",S.c_str(),(*parameterBool[S]));
 }
 }
 }


 double Data::getDefaultDouble(string S){
 return (*defaultDoubles[S]);
 }

 int Data::getDefaultInt(string S){
 return (*defaultInts[S]);
 }

 string Data::getDefaultString(string S){
 return (*defaultStrings[S]);
 }

 bool Data::getDefaultBool(string S){
 return (*defaultBools[S]);
 }

 double* Data::makeDefaultDouble(string S){
 double *D=new double;
 defaultDoubles[S]=D;
 return D;
 }

 int* Data::makeDefaultInt(string S){
 int *I=new int;
 defaultInts[S]=I;
 return I;
 }
 string* Data::makeDefaultString(string S){
 string *D=new string();
 defaultStrings[S]=D;
 return D;
 }

 bool* Data::makeDefaultBool(string S){
 bool *B=new bool;
 defaultBools[S]=B;
 return B;
 }

 void Data::setDefaultDouble(string S,double to){
 (*defaultDoubles[S])=to;
 }

 void Data::setDefaultInt(string S,int to){
 (*defaultInts[S])=to;

 }

 void Data::setDefaultString(string S,string to){
 (*defaultStrings[S])=to;

 }

 void Data::setDefaultBool(string S,bool to){
 (*defaultBools[S])=to;
 }
 */

