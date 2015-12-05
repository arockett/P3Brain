#include "Archivist.h"

string& Default_Archivist::Arch_outputMethodStr = Parameters::register_parameter("outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), snapshot, SSwD (SnapShot with Delay)]", "ARCHIVIST");  // string parameter for outputMethod;

int& Default_Archivist::Arch_realtimeFilesInterval = Parameters::register_parameter("realtimeFilesInterval", 10, "How often to write to realtime data files", "ARCHIVIST");
bool& Default_Archivist::Arch_writeAveFile = Parameters::register_parameter("writeAveFile", true, "Save data to average file?", "ARCHIVIST");
bool& Default_Archivist::Arch_writeDominantFile = Parameters::register_parameter("writeDominantFile", true, "Save data to dominant file?", "ARCHIVIST");
string& Default_Archivist::Arch_AveFileName = Parameters::register_parameter("aveFileName", (string) "ave.csv", "name of average file (saves population averages)", "ARCHIVIST");
string& Default_Archivist::Arch_DominantFileName = Parameters::register_parameter("dominantFileName", (string) "dominant.csv", "name of dominant file (saves data on dominant organism)", "ARCHIVIST");
string& Default_Archivist::Arch_DefaultAveFileColumnNames = Parameters::register_parameter("DefaultAveFileColumns", (string) "[update,score,genomeSize,gates]", "data to be saved into average file (must be values that can generate an average)", "ARCHIVIST");

int& LODwAP_Archivist::LODwAP_Arch_dataInterval = Parameters::register_parameter("dataInterval_LODwAP", 10, "How often to write to data file", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_LODwAP", 10, "How often to write genome file", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_pruneInterval = Parameters::register_parameter("pruneInterval_LODwAP", 10, "How often to attempt to prune LOD and actually write out to files", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_terminateAfter = Parameters::register_parameter("terminateAfter_LODwAP", 100, "how long to run after updates (to get better coalescence)", "ARCHIVIST_LODWAP");
string& LODwAP_Archivist::LODwAP_Arch_DataFileName = Parameters::register_parameter("dataFileName_LODwAP", (string) "data.csv", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_LODWAP");
string& LODwAP_Archivist::LODwAP_Arch_GenomeFileName = Parameters::register_parameter("genomeFileName_LODwAP", (string) "genome.csv", "name of data file (stores data i.e. scores", "ARCHIVIST_LODWAP");
bool& LODwAP_Archivist::LODwAP_Arch_writeDataFile = Parameters::register_parameter("writeDataFile_LODwAP", true, "if true, a data file will be written", "ARCHIVIST_LODWAP");
bool& LODwAP_Archivist::LODwAP_Arch_writeGenomeFile = Parameters::register_parameter("writeGenomeFile_LODwAP", true, "if true, a genome file will be written", "ARCHIVIST_LODWAP");

int& Snapshot_Archivist::SS_Arch_dataInterval = Parameters::register_parameter("dataInterval_SS", 10, "How often to save a data file", "ARCHIVIST_SNAPSHOT");
int& Snapshot_Archivist::SS_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SS", 10, "How often to save a genome file", "ARCHIVIST_SNAPSHOT");
bool& Snapshot_Archivist::SS_Arch_writeDataFiles = Parameters::register_parameter("writeDataFiles_SS", true, "if true, data files will be written", "ARCHIVIST_SNAPSHOT");
bool& Snapshot_Archivist::SS_Arch_writeGenomeFiles = Parameters::register_parameter("writeGenomeFiles_SS", true, "if true, genome files will be written", "ARCHIVIST_SNAPSHOT");

int& SSwD_Archivist::SSwD_Arch_dataInterval = Parameters::register_parameter("dataInterval_SSwD", 10, "How often to save a data file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SSwD", 10, "How often to save a genome file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_dataIntervalDelay = Parameters::register_parameter("dataIntervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay before saving data", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeIntervalDelay = Parameters::register_parameter("genomeIntervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay before saving genomes ", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_cleanupInterval = Parameters::register_parameter("cleanupInterval_SSwD", 10, "How often to cleanup old checkpoints", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_DataFilePrefix = Parameters::register_parameter("dataFilePrefix_SSwD", (string) "data", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_GenomeFilePrefix = Parameters::register_parameter("genomeFilePrefix_SSwD", (string) "genome", "name of data file (stores data i.e. scores", "ARCHIVIST_SSWD");
bool& SSwD_Archivist::SSwD_Arch_writeDataFiles = Parameters::register_parameter("writeDataFiles_SSwD", true, "if true, data files will be written", "ARCHIVIST_SSWD");
bool& SSwD_Archivist::SSwD_Arch_writeGenomeFiles = Parameters::register_parameter("writeGenomeFiles_SSwD", true, "if true, genome files will be written", "ARCHIVIST_SSWD");


