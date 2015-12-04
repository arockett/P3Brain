#include "Archivist.h"

string& Default_Archivist::Arch_outputMethodStr = Parameters::register_parameter("outputMethod", (string) "default", "output method, [default, LODwAP (Line of Decent with Aggressive Pruning), snapshot, SSwD (SnapShot with Delay)]", "ARCHIVIST");  // string parameter for outputMethod;

int& Default_Archivist::Arch_realtimeFilesInterval = Parameters::register_parameter("realtimeFilesInterval", 10, "How often to write to realtime data files", "ARCHIVIST");
bool& Default_Archivist::Arch_writeAveFile = Parameters::register_parameter("writeAveFile", true, "Save data to average file?", "ARCHIVIST");
bool& Default_Archivist::Arch_writeDominantFile = Parameters::register_parameter("writeDominantFile", true, "Save data to dominant file?", "ARCHIVIST");
string& Default_Archivist::Arch_AveFileName = Parameters::register_parameter("aveFileName", (string) "ave.csv", "name of average file (saves population averages)", "ARCHIVIST");
string& Default_Archivist::Arch_DominantFileName = Parameters::register_parameter("dominantFileName", (string) "ave.csv", "name of dominant file (saves data on dominant organism)", "ARCHIVIST");
string& Default_Archivist::Arch_DefaultAveFileColumnNames = Parameters::register_parameter("DefaultAveFileColumns", (string) "[update,score,genomeSize,gates]", "data to be saved into average file (must be values that can generate an average)", "ARCHIVIST");

int& LODwAP_Archivist::LODwAP_Arch_dataInterval = Parameters::register_parameter("dataInterval_LODwAP", 10, "How often to write to data file", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_LODwAP", 10, "How often to write genome file", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_pruneInterval = Parameters::register_parameter("pruneInterval_LODwAP", 10, "How often to attempt to prune LOD and actually write out to files", "ARCHIVIST_LODWAP");
int& LODwAP_Archivist::LODwAP_Arch_terminateAfter = Parameters::register_parameter("terminateAfter_LODwAP", 100, "how long to run after updates (to get better coalescence)", "ARCHIVIST_LODWAP");
string& LODwAP_Archivist::LODwAP_Arch_DataFileName = Parameters::register_parameter("dataFileName_LODwAP", (string) "data.csv", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_LODWAP");
string& LODwAP_Archivist::LODwAP_Arch_GenomeFileName = Parameters::register_parameter("genomeFileName_LODwAP", (string) "genome.csv", "name of data file (stores data i.e. scores", "ARCHIVIST_LODWAP");

int& SnapShot_Archivist::SS_Arch_snapshotInterval = Parameters::register_parameter("SS_Arch_snapshotInterval", 10, "how often to output snapshots", "ARCHIVIST_SNAPSHOT");

int& SSwD_Archivist::SSwD_Arch_dataInterval = Parameters::register_parameter("dataInterval_SSwD", 10, "How often to write to data file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SSwD", 10, "How often to write genome file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_intervalDelay = Parameters::register_parameter("intervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_cleanupInterval = Parameters::register_parameter("cleanupInterval_SSwD", 10, "How often to cleanup old checkpoints", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_DataFileName = Parameters::register_parameter("dataFileName_SSwD", (string) "data.csv", "name of genome file (stores genomes for line of decent)", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_GenomeFileName = Parameters::register_parameter("genomeFileName_SSwD", (string) "genome.csv", "name of data file (stores data i.e. scores", "ARCHIVIST_SSWD");


