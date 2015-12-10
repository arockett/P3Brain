#include "SSwD_Archivist.h"

int& SSwD_Archivist::SSwD_Arch_dataInterval = Parameters::register_parameter("dataInterval_SSwD", 10, "How often to save a data file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeInterval = Parameters::register_parameter("genomeInterval_SSwD", 10, "How often to save a genome file", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_dataIntervalDelay = Parameters::register_parameter("dataIntervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay before saving data", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_genomeIntervalDelay = Parameters::register_parameter("genomeIntervalDelay_SSwD", 3, "when using Snap Shot with Delay output Method, how long is the delay before saving genomes ", "ARCHIVIST_SSWD");
int& SSwD_Archivist::SSwD_Arch_cleanupInterval = Parameters::register_parameter("cleanupInterval_SSwD", 10, "How often to cleanup old checkpoints", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_DataFilePrefix = Parameters::register_parameter("dataFilePrefix_SSwD", (string) "data", "name of genome file (stores genomes)", "ARCHIVIST_SSWD");
string& SSwD_Archivist::SSwD_Arch_GenomeFilePrefix = Parameters::register_parameter("genomeFilePrefix_SSwD", (string) "genome", "name of data file (stores everything but genomes)", "ARCHIVIST_SSWD");
bool& SSwD_Archivist::SSwD_Arch_writeDataFiles = Parameters::register_parameter("writeDataFiles_SSwD", true, "if true, data files will be written", "ARCHIVIST_SSWD");
bool& SSwD_Archivist::SSwD_Arch_writeGenomeFiles = Parameters::register_parameter("writeGenomeFiles_SSwD", true, "if true, genome files will be written", "ARCHIVIST_SSWD");

SSwD_Archivist::SSwD_Archivist()
  : Archivist() {

    dataInterval = SSwD_Arch_dataInterval;
    genomeInterval = SSwD_Arch_genomeInterval;
    dataIntervalDelay = SSwD_Arch_dataIntervalDelay;
    genomeIntervalDelay = SSwD_Arch_genomeIntervalDelay;

    cleanupInterval = SSwD_Arch_cleanupInterval;
    DataFilePrefix = SSwD_Arch_DataFilePrefix;
    GenomeFilePrefix = SSwD_Arch_GenomeFilePrefix;
    writeDataFiles = SSwD_Arch_writeDataFiles;
    writeGenomeFiles = SSwD_Arch_writeGenomeFiles;

    nextDataWrite = 0;
    nextGenomeWrite = 0;
    nextDataCheckPoint = 0;
    nextGenomeCheckPoint = 0;
  }

  ///// CLEANUP / DELETE STALE CHECKPOINTS
  // if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
  // and all of it's org parents (with clear) assuming org was dead at the time
  // this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.
  void SSwD_Archivist::cleanup() {

    {
      vector<int> expiredCheckPoints;
      bool checkpointEmpty;
      for (auto checkpoint : checkpoints) {  // for every checkpoint
        if (checkpoint.first < (Global::update - max(dataIntervalDelay,genomeIntervalDelay))) {  // if that checkpoint is older then the longest intervalDelay
          checkpointEmpty = true;
          for (auto weakPtrToOrg : checkpoints[checkpoint.first]) {  // than for each element in that checkpoint
            if (auto org = weakPtrToOrg.lock()) {  // if this ptr is still good
              if ((!org->alive) && (org->timeOfDeath < (Global::update - max(dataIntervalDelay,genomeIntervalDelay)))) {  // and if the organism was dead before the current interesting data
                org->parents.clear();// clear this organisms parents
              } else {
                checkpointEmpty = false;  // there is an organism in this checkpoint that was alive later then (Global::update - intervalDelay)
                                          // this could organism could still be active
              }
            }
          }
          if (checkpointEmpty) {  // if all of the organisims in this checkpoint have been dead for a good long time...
                                  // if this checkpoint is not empty, we want to keep it so we can check it again later.
            expiredCheckPoints.push_back(checkpoint.first);// add this checkpoint to a list of checkpoints to be deleted
          }
        }
      }
      for (auto checkPointTime : expiredCheckPoints) {  // for each checkpoint in the too be deleted list
        checkpoints.erase(checkPointTime);// delete this checkpoint
      }
    }
  }

   bool SSwD_Archivist::archive(vector<shared_ptr<Organism>> population, int flush) {

    if (flush != 1) {

      if (Global::update % realtimeFilesInterval == 0) {  // do not write files on flush - these organisms have not been evaluated!
        writeRealTimeFiles(population);// write to dominant and average files
      }

      ///// Clean up the checkpoints
      if (Global::update % cleanupInterval == 0) {
        cleanup();
      }

      ///// ADDING TO THE ARCHIVE

      if ((Global::update == nextGenomeCheckPoint && writeGenomeFiles) || (Global::update == nextDataCheckPoint && writeDataFiles)) {  // if we are at a data or genome interval...
                                                                                             // we need to make a checkpoint of the current population
        for (auto org : population) {  // add the current population to checkPointTracker
          checkpoints[Global::update].push_back(org);
          org->snapShotDataMaps[Global::update] = org->dataMap;// back up state of dataMap

          if (Global::update == nextGenomeCheckPoint && Global::update <= Global::updates) {  // if this is a genome interval, add genomeAncestors to snapshot dataMap
            for (auto ancestor : org->genomeAncestors) {
              org->snapShotDataMaps[Global::update].Append("genomeAncestors", ancestor);
            }
            org->genomeAncestors.clear();  // clear genomeAncestors (this data is safe in the checkPoint)
            org->genomeAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                                 // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
          }

          if (Global::update == nextDataCheckPoint && Global::update <= Global::updates) {  // if this is a data interval, add dataAncestors to snapshot dataMap
            for (auto ancestor : org->dataAncestors) {
              org->snapShotDataMaps[Global::update].Append("dataAncestors", ancestor);
            }
            org->dataAncestors.clear();  // clear dataAncestors (this data is safe in the checkPoint)
            org->dataAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                               // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
          }
        }
        if (Global::update == nextGenomeCheckPoint && Global::update <= Global::updates) {  // we have now made a genome checkpoint, advance nextGenomeCheckPoint to get ready for the next interval
          nextGenomeCheckPoint += genomeInterval;
        }
        if (Global::update == nextDataCheckPoint && Global::update <= Global::updates) {  // we have now made a data checkpoint, advance nextDataCheckPoint to get ready for the next interval
          nextDataCheckPoint += dataInterval;
        }
      }

      ////// WRITING FROM THE ARCHIVE

      ////// WRITING GENOMES

      if ((Global::update == nextGenomeWrite + genomeIntervalDelay) &&
          (nextGenomeWrite <= Global::updates) &&
          writeGenomeFiles) {  // now it's time to write genomes in the checkpoint at time nextGenomeWrite
        string genomeFileName = GenomeFilePrefix + "_" + to_string(nextGenomeWrite) + ".csv";

        string dataString;
        size_t index = 0;
        while (index < checkpoints[nextGenomeWrite].size()) {
          if (auto org = checkpoints[nextGenomeWrite][index].lock()) {  // this ptr is still good
            dataString = to_string(org->ID) + FileManager::separator + org->snapShotDataMaps[nextGenomeWrite].Get("genomeAncestors") + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";// add interval update, genome ancestors, and genome with padding to string
            FileManager::writeToFile(genomeFileName, dataString, "ID,genomeAncestors,genome");// write data to file
            index++;// advance to nex element
          } else {  // this ptr is expired - cut it out of the vector
            swap(checkpoints[nextGenomeWrite][index], checkpoints[nextGenomeWrite].back());// swap expired ptr to back of vector
            checkpoints[nextGenomeWrite].pop_back();// pop expired ptr from back of vector
          }
        }
        nextGenomeWrite += genomeInterval;  // we are done with this interval, get ready for the next one.
      }

      ////// WRITING DATA

      if ((Global::update == nextDataWrite + dataIntervalDelay) &&
          (nextDataWrite <= Global::updates) &&
          writeDataFiles) {  // now it's time to write data in the checkpoint at time nextDataWrite
        string dataFileName = DataFilePrefix + "_" + to_string(nextDataWrite) + ".csv";

        // if file info has not been initialized yet, find a valid org and extract it's keys
        if (files.find("data") == files.end()) {
          bool found = false;
          shared_ptr<Organism> org;

          while (!found) {  // check each org in checkPointTraker[nextDataWrite] until we find a valid org
            if (auto temp_org = checkpoints[nextDataWrite][0].lock()) {  // this ptr is still good
              org = temp_org;
              found = true;
            } else {  // it' empty, swap to back and remove.
              swap(checkpoints[nextDataWrite][0], checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
              checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
            }
          }

          vector<string> tempKeysList = org->snapShotDataMaps[nextDataWrite].getKeys();  // get all keys from the valid orgs dataMap (all orgs should have the same keys in their dataMaps)
          for (auto key : tempKeysList) {  // for every key in dataMap...
            if (key != "genomeAncestors") {  // as long as it's not genomeAncestors... (genomeAncestors may be in the dataMap if a genome and data interval overlap)
              files["data"].push_back(key);// add it to the list of keys associated with the genome file.
            }
          }
        }

        // write out data for all orgs in checkPointTracker[Global::nextGenomeWrite] to "genome_" + to_string(Global::nextGenomeWrite) + ".csv"

        size_t index = 0;
        while (index < checkpoints[nextDataWrite].size()) {
          if (auto org = checkpoints[nextDataWrite][index].lock()) {  // this ptr is still good
            org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]);// append new data to the file
            index++;// advance to nex element
          } else {  // this ptr is expired - cut it out of the vector
            swap(checkpoints[nextDataWrite][index], checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
            checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
          }
        }
        nextDataWrite += dataInterval;  // we are done with this interval, get ready for the next one.
      }
    }
    // if enough time has passed to save all data and genomes, then we are done!
    return ((nextDataWrite > Global::updates || !(writeDataFiles)) && (nextGenomeWrite > Global::updates || !(writeGenomeFiles)) && Global::update >= Global::updates);
  }
