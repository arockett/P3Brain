#include "Archivist.h"

string& Archivist::outputMethodStr = Parameters::register_parameter("outputMethod", (string) "LODwAP", "output method, either LODwAP (Line of Decent with Aggressive Pruning) or SSwD (SnapShot with Delay)", "ARCHIVIST");  // string parameter for outputMethod;
int Archivist::outputMethod = -1;  // this will be set the first time archive is called. -1 indicates that it has not been set.

int& Archivist::dataInterval = Parameters::register_parameter("dataInterval", 1, "How often to write to data file", "ARCHIVIST");
int& Archivist::genomeInterval = Parameters::register_parameter("genomeInterval", 1, "How often to write genome file", "ARCHIVIST");
int& Archivist::intervalDelay = Parameters::register_parameter("intervalDelay", 3, "when using Snap Shot with Delay output Method, how long is the delay", "ARCHIVIST");
int& Archivist::pruneInterval = Parameters::register_parameter("pruneInterval", dataInterval, "How often to attempt to prune LOD and actually write out to files", "ARCHIVIST");
string& Archivist::DataFileName = Parameters::register_parameter("dataFileName", (string) "data.csv", "name of genome file (stores genomes for line of decent)", "ARCHIVIST");
string& Archivist::GenomeFileName = Parameters::register_parameter("genomeFileName", (string) "genome.csv", "name of data file (stores data i.e. scores", "ARCHIVIST");

string& Archivist::AveFileName = Parameters::register_parameter("aveFileName", (string) "ave.csv", "name of the Averages file (averages for all brains when file is written to)", "ARCHIVIST");
string& Archivist::DominantFileName = Parameters::register_parameter("dominantFileName", (string) "dominant.csv", "name of the Dominant file (all stats for best genome when file is written to)", "ARCHIVIST");

int Archivist::lastPrune = 0;  // last time Genome was Pruned

int Archivist::nextDataWrite = 0;  // next time data files will be written to disk
int Archivist::nextGenomeWrite = 0;  // next time a genome file will be written to genome.csv
int Archivist::nextDataCheckPoint = 0;  // next time data files will be written to disk
int Archivist::nextGenomeCheckPoint = 0;  // next time a genome file will be written to genome.csv

map<string, vector<string>> Archivist::files;  // list of files in use with their meta data
vector<string> Archivist::DefaultAveFileColumns = { "update", "score", "genomeSize", "gates" };
