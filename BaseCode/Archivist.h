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

    static string& outputMethodStr; // string parameter for outputMethod;
    static int outputMethod; // 0 = LODwAP (Line of Decent with Aggressive Pruning), 1 = SSwD (SnapShot with Delay)

    static int& dataInterval; // how often to write out data
    static int& genomeInterval; // how often to write out genomes
    static int& intervalDelay; // when using SSwD, how long is the delay
    static int& pruneInterval; // how often to attempt to prune the LOD
    static string& DataFileName; // name of the Data file
    static string& GenomeFileName; // name of the Genome file (genomes on LOD)
    static string& AveFileName; // name of the Averages file (ave for all brains when file is written to)
    static string& DominantFileName; // name of the Dominant file (all stats for best brain when file is writtne to)

    static int lastPrune; // last time Genome was Pruned

    // info about files under management
    static int nextDataWrite; // next time data files will be written to disk
    static int nextGenomeWrite; // next time a genome file will be written to genome.csv
    static int nextDataCheckPoint; // next time data files contents need to be checkpointed (this is needed particularly to handle delay > interval)
    static int nextGenomeCheckPoint; // next time a genome files contents need to be checkpointed (this is needed particularly to handle delay > interval)

    static map<string, vector<string>> files; // list of files (NAME,LIST OF COLUMNS)
    static vector<string> DefaultAveFileColumns; // what columns will be written into the AveFile

    unordered_map<int, vector<weak_ptr<Organism>>> checkPointTracker; // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
                                                // key is Global::nextGenomeWrite or Global::nextDataWrite
    map<int, int> howManyTracking; // maps update to number of writes are set on that update.
                                   // If genome or data are following, value = 1, if both, value = 2
    map<int, int> orgTrackingCounts; // keeps track of number of tracks on each org

    void writeRealTimeFiles(vector<shared_ptr<Organism>> &population) { // write Ave and Dominant files NOW!
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
        if (outputMethod == -1) { // this is the first time archive is called. get the output method
            if (outputMethodStr == "LODwAP") {
                outputMethod = 0;
            } else if (outputMethodStr == "SSwD") {
                outputMethod = 1;
            } else {
                cout << "unrecognized archive method \"" << outputMethodStr << "\". Should be either \"LODwAP\" or \"SSwD\"\nExiting.\n";
            }
        }
        if (outputMethod == 0) {
            if ((Global::update % pruneInterval == 0) && (flush == 0)) { // do not write files on flush - these organisms have not been evaluated!
                writeRealTimeFiles(population); // write to dominant and average files
            }
            if ((Global::update % pruneInterval == 0) || (flush == 1)) {

                shared_ptr<Organism> sampleOrg = population[0];
                shared_ptr<Organism> MRCA = sampleOrg->getMostRecentCommonAncestor();

                if (files.find("data.csv") == files.end()) { // if file has not be initialized yet
                    files[DataFileName] = sampleOrg->dataMap.getKeys();
                }

                vector<shared_ptr<Organism>> LOD = sampleOrg->getLOD();       // get line of decent
                shared_ptr<Organism> effective_MRCA;
                if (flush) {                // if flush then we don't care about convergance
                    cout << "flushing LODwAP: using population[0] as MRCA\n";
                    effective_MRCA = *(sampleOrg->parents).begin(); // this assumes that a population was created, but not tested at the end of the evolution loop!
                } else {
                    effective_MRCA = MRCA; // find the convergance point in the LOD.
                }

                while ((effective_MRCA->timeOfBirth >= nextDataWrite) && (nextDataWrite <= Global::updates)) { // if there is convergence before the next data interval
                    shared_ptr<Organism> current = LOD[nextDataWrite - lastPrune];
                    for (auto file : files) { // for each file in files
                        current->dataMap.writeToFile(file.first, file.second); // append new data to the file
                    }
                    nextDataWrite += dataInterval;
                }

                while ((effective_MRCA->timeOfBirth >= nextGenomeWrite) && (nextGenomeWrite <= Global::updates)) { // if there is convergence before the next data interval
                    shared_ptr<Organism> current = LOD[nextGenomeWrite - lastPrune];
                    string dataString = to_string(nextGenomeWrite) + FileManager::separator + "\"[" + current->genome->convert_to_string() + "]\""; // add write update and padding to genome string
                    FileManager::writeToFile(GenomeFileName, dataString, "update,genome"); // write data to file
                    nextGenomeWrite += genomeInterval;
                }
                // data and genomes have now been written out up till the MRCA
                // so all data and genomes from before the MRCA can be deleted
//                for (auto parent : MRCA->parents) {
//                    if (parent != nullptr) { // if the MRCA is not the oldest in the LOD...
//                        parent->unFollow(); // this should set parents reference counter = 0, so parent gets deleted (and as a result all ancestors)
//                    }
//                }
                MRCA->parents.clear();
                lastPrune = MRCA->timeOfBirth; // this will hold the time of the oldest genome in RAM
            }
        }

//        if (outputMethod == 1) { // SSwD (SnapShot with Delay)
//            if (flush == 1) {
//                cout << "flushing SSwD: nothing needs to be done\n";
//            } else {
//                if (Global::update % pruneInterval == 0) {
//                    writeRealTimeFiles(population); // write to dominant and average files
//                }
//
//                ////// TRACKER CLEANUP
//                // first, see if we can do any cleanup.
//                // check each checkPointTracker[] to see if anyone can be deleted.
//                //      for each update in checkPointTracker[] check each org for RefConter == orgTrackingCounts(org->ID) (i.e. only being tracked by tracker)
//                //      if yes, erase org from checkPointTracker and orgTrackingCounts and delete the org
//
//                vector<int> trackedCheckPoints; // will hold a sorted list of the times for all the checkpoints
//
//                for (auto checkPoint : checkPointTracker) { // fill in and then sort trackedCheckPoints
//                    if (checkPoint.second.empty()) { // if a population in the tracker is empty is means someone deleted something they should not have! There must always be (at least) one!
//                        cout << "checkPoint time: " << checkPoint.first << "\n A checkPoint population is empty. This is very bad! LOD is broken! Exiting!\n";
//                        exit(1);
//                    }
//                    trackedCheckPoints.push_back(checkPoint.first); // extract the keys
//                }
//                sort(trackedCheckPoints.begin(), trackedCheckPoints.end());
//
//                set<Organism*> curr_checkPoint;
//                set<Organism*> checkedOrgs;
//                set<Organism*> recheckCheckPoint;
//
//                for (int i = (int) trackedCheckPoints.size() - 1; i >= 0; i--) {
//                    curr_checkPoint = checkPointTracker[trackedCheckPoints[i]];
//                    recheckCheckPoint.clear();
//
//                    while (!curr_checkPoint.empty()) {
//                        checkedOrgs.clear();
//
//                        for (auto org : curr_checkPoint) {
//
//                            checkedOrgs.insert(org);
//                            if (orgTrackingCounts[org->ID] > org->referenceCounter) {
//                                cout << "number of tracks on this org: " << orgTrackingCounts[org->ID] << " is greater than refs on org: " << org->referenceCounter << "\n";
//                                cout << " A checkPoint organism has too few references! This is very bad! LOD is broken! Exiting!\n";
//                                exit(1);
//                            }
//                            if ((orgTrackingCounts[org->ID] == org->referenceCounter)) { // if only the tracker is looking at this organism it can be deleted
//                                for (int j = i; j >= 0; j--) {
//                                    checkPointTracker[trackedCheckPoints[j]].erase(org); // erase the pointer from all trackers older then this one - this is a bit of a nuke from orbit (because the org maynot be in all prior checkpoints), but it's the only way to be sure.
//                                    orgTrackingCounts.erase(org->ID); // this org is being deleted we don't need to keep its tracking count (it's 0 now!)
//                                }
//                                delete (org); // the org has no decendants and no tracker is looking at it. It's time to delete the organism.
//                            }
//                        }
//                        curr_checkPoint = recheckCheckPoint;
//                        recheckCheckPoint.clear();
//                    }
//                }
//
//                ///// ADDING TO THE TRACKER
//                if (Global::update == nextGenomeCheckPoint || Global::update == nextDataCheckPoint) { // if we are at a data or genome interval...
//                                                                                                      // we need to make a checkpoint of the current population
//                    for (auto org : population) { // add the current population to checkPointTracker
//                        checkPointTracker[Global::update].insert(org);
//                        org->snapShotDataMaps[Global::update] = org->dataMap; // back up state of dataMap
//
//                        if (Global::update == nextGenomeCheckPoint) { // if this is a genome interval, add genomeAncestors to snapshot dataMap
//                            org->addFollow(); // this org is now also being followed by tracker also - for it's genome
//                            orgTrackingCounts[org->ID]++; // add a tracker
//                            for (auto ancestor : org->genomeAncestors) {
//                                org->snapShotDataMaps[Global::update].Append("genomeAncestors", ancestor);
//                            }
//                            org->genomeAncestors.clear(); // clear genomeAncestors (this data is safe in the checkPoint)
//                            org->genomeAncestors.insert(org->ID); // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
//                                                                  // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
//                        }
//
//                        if (Global::update == nextDataCheckPoint) { // if this is a data interval, add dataAncestors to snapshot dataMap
//                            org->addFollow(); // this org is now also being followed by tracker also - for it's data
//                            orgTrackingCounts[org->ID]++; // add a tracker
//                            for (auto ancestor : org->dataAncestors) {
//                                org->snapShotDataMaps[Global::update].Append("dataAncestors", ancestor);
//                            }
//                            org->dataAncestors.clear(); // clear dataAncestors (this data is safe in the checkPoint)
//                            org->dataAncestors.insert(org->ID); // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
//                                                                // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
//                        }
//                    }
//                    if (Global::update == nextGenomeCheckPoint) { // we have now made a genome checkpoint, advance ready for the next interval
//                        nextGenomeCheckPoint += genomeInterval;
//                        howManyTracking[Global::update]++;
//                    }
//                    if (Global::update == nextDataCheckPoint) { // we have now made a data checkpoint, advance ready for the next interval
//                        nextDataCheckPoint += dataInterval;
//                        howManyTracking[Global::update]++;
//                    }
//                }
//
//                ////// WRITING FROM THE TRACKER
//                if (Global::update == nextGenomeWrite + intervalDelay) {
//                    // now it's time to write genomes in the checkpointe at time nextGenomeWrite
//                    string genomeFileName = "genomes_" + to_string(nextGenomeWrite) + ".csv";
//
//                    string dataString;
//                    for (auto org : checkPointTracker[nextGenomeWrite]) {
//                        dataString = to_string(org->ID) + FileManager::separator + org->snapShotDataMaps[nextGenomeWrite].Get("genomeAncestors") + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";
//                        // add interval update, genome ancestors, and genome with padding to string
//                        FileManager::writeToFile(genomeFileName, dataString, "ID,genomeAncestors,genome"); // write data to file
//                        org->unFollow(); // tracker is no longer following this org (for it's genome). If this was the only thing tracking, the org is deleted and this checkpoint will be deleted automaticly below.
//                        // we could check here if anyone else is tracking this checkpoint (howManyTracking would = 1). if not, we could checkPointDataMap.erase(Global::nextGenomeWrite) and maybe the genome and brain also.
//                        orgTrackingCounts[org->ID]--; // one less tracker looking at this org
//                        if (orgTrackingCounts[org->ID] == 0) {
//                            orgTrackingCounts.erase(org->ID);
//                        }
//                    }
//                    howManyTracking[nextGenomeWrite]--;  // tacker is no longer interested in the genomes at this checkpoint
//                    if (howManyTracking[nextGenomeWrite] == 0) { // no one is looking at this checkpoint - all of it's orgs have been unFollowed - delete it!
//                        checkPointTracker.erase(nextGenomeWrite);
//                        howManyTracking.erase(nextGenomeWrite);
//                    }
//                    nextGenomeWrite += genomeInterval; // we are done with this interval, get ready for the next one.
//                }
//
//                if (Global::update == nextDataWrite + intervalDelay) {
//                    // now it's time to write data in the checkpoint at time nextDataWrite
//                    string dataFileName = "data_" + to_string(nextDataWrite) + ".csv";
//
//                    if (files.find("data") == files.end()) { // if file info has not been initialized yet
//                        vector<string> tempKeysList = (*checkPointTracker[nextDataWrite].begin())->snapShotDataMaps[nextDataWrite].getKeys(); // get all keys from an orgs dataMap (don't care which one - they should all be the same)
//                        for (auto key : tempKeysList) { // for every key in dataMap...
//                            if (key != "genomeAncestors") { // as long as it's not genomeAncestors... genomeAncestors may be here is a genome and data interval overlap
//                                files["data"].push_back(key); // add it to the list of keys associated with the genome file.
//                            }
//                        }
//                    }
//
//                    // write out data for all orgs in checkPointTracker[Global::nextGenomeWrite] to "genome_" + to_string(Global::nextGenomeWrite) + ".csv"
//                    for (auto org : checkPointTracker[nextDataWrite]) {
//                        org->snapShotDataMaps[nextDataWrite].writeToFile(dataFileName, files["data"]); // append new data to the file
//                        org->unFollow(); // tracker is no longer following this org (for it's data). If this was the only thing tracking, the org is deleted and this checkpoint will be deleted automaticly below.
//                        // we could check here if anyone else is tracking this checkpoint (howManyTracking would = 1). if not, we could checkPointDataMap.erase(Global::nextGenomeWrite) and maybe the genome and brain also.
//                        orgTrackingCounts[org->ID]--; // one less tracker looking at this org
//                        if (orgTrackingCounts[org->ID] == 0) {
//                            orgTrackingCounts.erase(org->ID);
//                        }
//                    }
//                    howManyTracking[nextDataWrite]--; // tacker is no longer interested in the data at this checkpoint
//                    if (howManyTracking[nextDataWrite] == 0) { // no one is looking at this checkpoint - all of it's orgs have been unFollowed - delete it!
//                        checkPointTracker.erase(nextDataWrite);
//                        howManyTracking.erase(nextDataWrite);
//                    }
//                    nextDataWrite += dataInterval; // we are done with this interval, get ready for the next one.
//                }
//            }
//        }
    }
};

#endif /* defined(__BasicMarkovBrainTemplate__Archivist__) */
