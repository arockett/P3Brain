//
//  Archivist.h
//  BasicMarkovBrainTemplate
//
//  Created by Cliff Bohm on 5/30/15.
//

#ifndef __BasicMarkovBrainTemplate__Archivist__
#define __BasicMarkovBrainTemplate__Archivist__

#include <algorithm>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "Brain.h"
#include "Genome.h"
#include "Global.h"
//#include "BaseCode/Optimizer.h"
#include "Organism.h"
//#include "BaseCode/World.h"

//#include "Utilities/Parameters.h"
//#include "Utilities/Random.h"
#include "../Utilities/Data.h"
#include "../Utilities/Utilities.h"

//#include "Worlds/BerryWorld.h"

using namespace std;

class Archivist {
 public:

  static string& outputMethodStr;  // string parameter for outputMethod;
  static int outputMethod;  // 0 = LODwAP (Line of Decent with Aggressive Pruning), 1 = SSwD (SnapShot with Delay)

  static int& dataInterval;  // how often to write out data
  static int& genomeInterval;  // how often to write out genomes
  static int& intervalDelay;  // when using SSwD, how long is the delay
  static int& pruneInterval;  // how often to attempt to prune the LOD
  static string& DataFileName;  // name of the Data file
  static string& GenomeFileName;  // name of the Genome file (genomes on LOD)
  static string& AveFileName;  // name of the Averages file (ave for all brains when file is written to)
  static string& DominantFileName;  // name of the Dominant file (all stats for best brain when file is writtne to)

  static int lastPrune;  // last time Genome was Pruned

  // info about files under management
  static int nextDataWrite;  // next time data files will be written to disk
  static int nextGenomeWrite;  // next time a genome file will be written to genome.csv
  static int nextDataCheckPoint;  // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
  static int nextGenomeCheckPoint;  // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

  static map<string, vector<string>> files;  // list of files (NAME,LIST OF COLUMNS)
  static vector<string> DefaultAveFileColumns;  // what columns will be written into the AveFile

  unordered_map<int, vector<weak_ptr<Organism>>> checkpoints;  // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
  // key is Global::nextGenomeWrite or Global::nextDataWrite

  void writeRealTimeFiles(vector<shared_ptr<Organism>> &population) {  // write Ave and Dominant files NOW!
    // write out Ave
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

    // write out Dominant
    vector<double> Scores;
    for (auto org : population) {
      Scores.push_back(org->score);
    }

    int best = findGreatestInVector(Scores);
    population[best]->dataMap.writeToFile(DominantFileName);
  }

  void archive(vector<shared_ptr<Organism>> population, int flush = 0) {
    if ((Global::update % pruneInterval == 0) && (flush == 0)) {  // do not write files on flush - these organisms have not been evaluated!
      writeRealTimeFiles(population);// write to dominant and average files
    }

    if (outputMethod == -1) {  // this is the first time archive is called. get the output method
      if (outputMethodStr == "LODwAP") {
        outputMethod = 0;
      } else if (outputMethodStr == "SSwD") {
        outputMethod = 1;
      } else {
        cout << "unrecognized archive method \"" << outputMethodStr << "\". Should be either \"LODwAP\" or \"SSwD\"\nExiting.\n";
      }
    }

    if (outputMethod == 0) {
      if ((Global::update % pruneInterval == 0) || (flush == 1)) {

        if (files.find("data.csv") == files.end()) {  // if file has not be initialized yet
          files[DataFileName] = population[0]->dataMap.getKeys();
        }

        vector<shared_ptr<Organism>> LOD = population[0]->getLOD(population[0]);  // get line of decent
        shared_ptr<Organism> effective_MRCA;
        if (flush) {  // if flush then we don't care about convergance
          cout << "flushing LODwAP: using population[0] as MRCA\n";
          effective_MRCA = population[0]->parents[0];// this assumes that a population was created, but not tested at the end of the evolution loop!
        } else {
          effective_MRCA = population[0]->getMostRecentCommonAncestor(LOD);  // find the convergance point in the LOD.
        }
        while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updates)) {  // if there is convergence before the next data interval
          shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
          for (auto file : files) {  // for each file in files
            current->dataMap.writeToFile(file.first, file.second);// append new data to the file
          }
          nextDataWrite += dataInterval;
        }

        while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updates)) {  // if there is convergence before the next data interval
          shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
          string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->convert_to_string() + "]\"";// add write update and padding to genome string
          FileManager::writeToFile(GenomeFileName, dataString, "update,genome");// write data to file
          nextGenomeWrite += genomeInterval;
        }
        // data and genomes have now been written out up till the MRCA
        // so all data and genomes from before the MRCA can be deleted
        effective_MRCA->parents.clear();
        lastPrune = effective_MRCA->timeOfBirth;// this will hold the time of the oldest genome in RAM
      }
    }

    if (outputMethod == 1) {  // SSwD (SnapShot with Delay)

      if (flush == 1) {
        cout << "flushing SSwD: nothing needs to be done\n";
      } else {  // not a flush - perform normal achive

        ///// CLEANUP DELETE STALE CHECKPOINTS
        // if a checkpoint is from before Global::update - archivist::intervalDelay than delete the checkpoint
        // and all of it's org parents (with clear) assuming org was dead at the time
        // this will have the effect of a delayed pruning, but should do a good enough job keeping memory down.

        {
          vector<int> expiredCheckPoints;
          bool checkpointEmpty;
          for (auto checkpoint : checkpoints) {  // for every checkpoint
            if (checkpoint.first < (Global::update - intervalDelay)) {  // if that checkpoint is older then the intervalDelay
              checkpointEmpty = true;
              for (auto weakPtrToOrg : checkpoints[checkpoint.first]) {  // than for each element in that checkpoint
                if(auto org = weakPtrToOrg.lock()) {  // if this ptr is still good
                  if((!org->alive) && (org->timeOfDeath < (Global::update - intervalDelay))) {  // and if the organism was dead at the time of this checkpoint
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

        ///// ADDING TO THE ARCHIVE

        if (Global::update == nextGenomeCheckPoint || Global::update == nextDataCheckPoint) {  // if we are at a data or genome interval...
                                                                                               // we need to make a checkpoint of the current population
          for (auto org : population) {  // add the current population to checkPointTracker
            checkpoints[Global::update].push_back(org);
            org->snapShotDataMaps[Global::update] = org->dataMap;// back up state of dataMap

            if (Global::update == nextGenomeCheckPoint) {  // if this is a genome interval, add genomeAncestors to snapshot dataMap
              for (auto ancestor : org->genomeAncestors) {
                org->snapShotDataMaps[Global::update].Append("genomeAncestors",ancestor);
              }
              org->genomeAncestors.clear();  // clear genomeAncestors (this data is safe in the checkPoint)
              org->genomeAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                                   // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
            }

            if (Global::update == nextDataCheckPoint) {  // if this is a data interval, add dataAncestors to snapshot dataMap
              for (auto ancestor : org->dataAncestors) {
                org->snapShotDataMaps[Global::update].Append("dataAncestors",ancestor);
              }
              org->dataAncestors.clear();  // clear dataAncestors (this data is safe in the checkPoint)
              org->dataAncestors.insert(org->ID);// now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                                 // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
            }
          }
          if (Global::update == nextGenomeCheckPoint) {  // we have now made a genome checkpoint, advance nextGenomeCheckPoint to get ready for the next interval
            nextGenomeCheckPoint += genomeInterval;
          }
          if (Global::update == nextDataCheckPoint) {  // we have now made a data checkpoint, advance nextDataCheckPoint to get ready for the next interval
            nextDataCheckPoint += dataInterval;
          }
        }

        ////// WRITING FROM THE ARCHIVE

        ////// WRITING GENOMES

        if (Global::update == nextGenomeWrite + intervalDelay) {  // now it's time to write genomes in the checkpoint at time nextGenomeWrite
          string genomeFileName = "genomes_" + to_string(nextGenomeWrite) + ".csv";

          string dataString;
          size_t index = 0;
          while (index<checkpoints[nextGenomeWrite].size()) {
            if(auto org = checkpoints[nextGenomeWrite][index].lock()) {  // this ptr is still good
              dataString = to_string(org->ID) + FileManager::separator + org->snapShotDataMaps[nextGenomeWrite].Get("genomeAncestors") + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";// add interval update, genome ancestors, and genome with padding to string
              FileManager::writeToFile(genomeFileName, dataString, "ID,genomeAncestors,genome");// write data to file
              index++;// advance to nex element
            } else {  // this ptr is expired - cut it out of the vector
              swap(checkpoints[nextGenomeWrite][index],checkpoints[nextGenomeWrite].back());// swap expired ptr to back of vector
              checkpoints[nextGenomeWrite].pop_back();// pop expired ptr from back of vector
            }
          }
          nextGenomeWrite += genomeInterval;  // we are done with this interval, get ready for the next one.
        }

        ////// WRITING DATA

        if (Global::update == nextDataWrite + intervalDelay) {
          // now it's time to write data in the checkpoint at time nextDataWrite
          string dataFileName = "data_" + to_string(nextDataWrite) + ".csv";

          if (files.find("data") == files.end()) {  // if file info has not been initialized yet, find a valid org and extract it's keys
            bool found = false;
            shared_ptr<Organism> org;

            while (!found) {  // check each org in checkPointTraker[nextDataWrite] until we find a valid org
              if (auto temp_org = checkpoints[nextDataWrite][0].lock()) {  // this ptr is still good
                org = temp_org;
                found = true;
              } else {  // it' empty, swap to back and remove.
                swap(checkpoints[nextDataWrite][0],checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
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
          while (index<checkpoints[nextDataWrite].size()) {
            if(auto org = checkpoints[nextDataWrite][index].lock()) {  // this ptr is still good
              org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]);// append new data to the file
              index++;// advance to nex element
            } else {  // this ptr is expired - cut it out of the vector
              swap(checkpoints[nextDataWrite][index],checkpoints[nextDataWrite].back());// swap expired ptr to back of vector
              checkpoints[nextDataWrite].pop_back();// pop expired ptr from back of vector
            }
          }
          nextDataWrite += dataInterval;  // we are done with this interval, get ready for the next one.
        }
      }
    }
  }
}
;

#endif /* defined(__BasicMarkovBrainTemplate__Archivist__) */
