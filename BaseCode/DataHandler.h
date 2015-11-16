//
//  DataHandler.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/30/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__DataHandler__
#define __BasicMarkovBrainTemplate__DataHandler__

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

class DataHandler {

    map<int, set<Organism*>> checkPointTracker; // used by SSwD only - this keeps lists of orgs that may be written (if they have living decendents)
                                                // key is Global::nextGenomeWrite or Global::nextDataWrite
    map<int, int> howManyTracking; // maps update to number of writes are set on that update.
                                   // If genome or data are following, value = 1, if both, value = 2
    map<int, int> orgTrackingCounts; // keeps track of number of tracks on each org

public:
    void writeRealTimeFiles(vector<Organism*> population) { // write Ave and Dominant files NOW!
        // write out Ave
        double aveValue, temp;
        DataMap AveMap;
        for (auto key : Global::DefaultAveFileColumns) {
            aveValue = 0;
            for (auto org : population) {
                stringstream ss(org->dataMap.Get(key));
                ss >> temp;
                aveValue += temp;
            }
            aveValue /= population.size();
            AveMap.Set(key, aveValue);
        }
        AveMap.writeToFile(Global::AveFileName, Global::DefaultAveFileColumns);

        // write out Dominant
        vector<double> Scores;
        for (auto org : population) {
            Scores.push_back(org->score);
        }

        int best = findGreatestInVector(Scores);
        population[best]->dataMap.writeToFile(Global::DominantFileName);
    }

    void saveData(vector<Organism*> population, int outputMethod) {
        if (outputMethod == 0) {
            if (Global::update % Global::pruneInterval == 0) {
                writeRealTimeFiles(population); // write to dominant and average files
                Organism::MRCA = population[0]->getMostRecentCommonAncestor();
                population[0]->saveDataOnLOD(); // write out data and genomes
                // data and genomes have now been written out up till the MRCA
                // so all data and genomes from before the MRCA can be deleted
                for (auto parent : Organism::MRCA->parents) {
                    if (parent != nullptr) { // if the MRCA is not the oldest in the LOD...
                        parent->unFollow(); // this should set parents reference counter = 0, so parent gets deleted (and as a result all ancestors)
                    }
                }
                Organism::MRCA->parents.clear();
                Global::lastPrune = Organism::MRCA->birthDate; // this will hold the time of the oldest genome in RAM
            }
        }
        if (outputMethod == 1) { // SSwD (SnapShot with Delay)
            if (Global::update % Global::pruneInterval == 0) {
                writeRealTimeFiles(population); // write to dominant and average files
            }

            ////// TRACKER CLEANUP
            // first, see if we can do any cleanup.
            // check each checkPointTracker[] to see if anyone can be deleted.
            //      for each update in checkPointTracker[] check each org for RefConter == orgTrackingCounts(org->ID) (i.e. only being tracked by tracker)
            //      if yes, erase org from checkPointTracker and orgTrackingCounts and delete the org

            vector<int> trackedCheckPoints; // will hold a sorted list of the times for all the checkpoints

            for (auto checkPoint : checkPointTracker) { // fill in and then sort trackedCheckPoints
                if (checkPoint.second.empty()) { // if a population in the tracker is empty is means someone deleted something they should not have! There must always be (atleast) one!
                    cout << "checkPoint time: " << checkPoint.first << "\n";
                    cout << " A checkPoint population is empty. This is very bad! LOD is broken! Exiting!\n";
                    exit(1);
                }
                trackedCheckPoints.push_back(checkPoint.first); // extract the keys
            }
            sort(trackedCheckPoints.begin(), trackedCheckPoints.end());

            for (int i = (int) trackedCheckPoints.size() - 1; i >= 0; i--) {
                set<Organism*> curr_checkPoint = checkPointTracker[trackedCheckPoints[i]];
                for (auto org : curr_checkPoint) {
                    if (orgTrackingCounts[org->ID] > org->referenceCounter) {
                        cout << "number of tracks on this org: " << orgTrackingCounts[org->ID] << " is greater than refs on org: " << org->referenceCounter << "\n";
                        cout << " A checkPoint organism has too few references! This is very bad! LOD is broken! Exiting!\n";
                        exit(1);
                    }
                    if ((orgTrackingCounts[org->ID] == org->referenceCounter)) { // if only the tracker is looking at this organism it can be deleted
                        for (int j = i; j >= 0; j--) {
                            checkPointTracker[trackedCheckPoints[j]].erase(org); // erase the pointer from all trackers older then this one - this is a bit of a nuke from orbit (because the org maynot be in all prior checkpoints), but it's the only way to be sure.
                        }
                        orgTrackingCounts.erase(org->ID); // this org is being deleted we don't need to keep its tracking count (it's 0 now!)
                        delete(org); // the org has no decendants and no tracker is looking at it. It's time to delete the organism.
                    }
                }
            }

            ///// ADDING TO THE TRACKER
            if (Global::update == Global::nextGenomeCheckPoint || Global::update == Global::nextDataCheckPoint) { // if we are at a data or genome interval...
                                                                                                                  // we need to make a checkpoint of the current population
                for (auto org : population) { // add the current population to checkPointTracker
                    checkPointTracker[Global::update].insert(org);
                    org->checkPointDataMap[Global::update] = org->dataMap; // back up state of dataMap
                    if (Global::update == Global::nextGenomeCheckPoint) { // if this is a genome interval, add genomeAncestors to snapshot dataMap
                        org->addFollow(); // this org is now also being followed by tracker also - for it's genome
                        orgTrackingCounts[org->ID]++; // add a tracker
                        for (auto ancestor : org->genomeAncestors) {
                            org->checkPointDataMap[Global::update].Append("genomeAncestors", ancestor);
                        }
                        org->genomeAncestors.clear(); // clear genomeAncestors (this data is safe in the checkPoint)
                        org->genomeAncestors.insert(org->ID); // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                                              // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
                    }

                    if (Global::update == Global::nextDataCheckPoint) { // if this is a data interval, add dataAncestors to snapshot dataMap
                        org->addFollow(); // this org is now also being followed by tracker also - for it's data
                        orgTrackingCounts[org->ID]++; // add a tracker
                        for (auto ancestor : org->dataAncestors) {
                            org->checkPointDataMap[Global::update].Append("dataAncestors", ancestor);
                        }
                        org->dataAncestors.clear(); // clear dataAncestors (this data is safe in the checkPoint)
                       org->dataAncestors.insert(org->ID); // now that we have saved the ancestor data, set ancestors to self (so that others will inherit correctly)
                                                            // also, if this survives over intervals, it'll be pointing to self as ancestor in files (which is good)
                    }
                }

                if (Global::update == Global::nextGenomeCheckPoint) { // we have now made a genome checkpoint, advance ready for the next interval
                    Global::nextGenomeCheckPoint += Global::genomeInterval;
                    howManyTracking[Global::update]++;
                }
                if (Global::update == Global::nextDataCheckPoint) { // we have now made a data checkpoint, advance ready for the next interval
                    Global::nextDataCheckPoint += Global::dataInterval;
                    howManyTracking[Global::update]++;
                }
            }

            ////// WRITING FROM THE TRACKER
            if (Global::update == Global::nextGenomeWrite + Global::intervalDelay) {
                // now it's time to write genomes that were checkpointed at time Global::nextGenomeWrite
                string genomeFileName = "genomes_" + to_string(Global::nextGenomeWrite) + ".csv";

                string dataString;
                for (auto org : checkPointTracker[Global::nextGenomeWrite]) {
                    dataString = to_string(org->ID) + FileManager::separator + org->checkPointDataMap[Global::nextGenomeWrite].Get("genomeAncestors")
                            + FileManager::separator + "\"[" + org->genome->convert_to_string() + "]\"";
                    // add interval update, genome ancestors, and genome with padding to string
                    FileManager::writeToFile(genomeFileName, dataString, "ID,genomeAncestors,genome"); // write data to file
                    org->unFollow(); // tracker is no longer following this org (for it's genome). If this was the only thing tracking, the org is deleted and this checkpoint will be deleted automaticly below.
                    // we could check here if anyone else is tracking this checkpoint (howManyTracking would = 1). if not, we could checkPointDataMap.erase(Global::nextGenomeWrite) and maybe the genome and brain also.
                    orgTrackingCounts[org->ID]--; // one less tracker looking at this org

                }
                howManyTracking[Global::nextGenomeWrite]--;  // tacker is no longer interested in the genomes at this checkpoint
                if (howManyTracking[Global::nextGenomeWrite] == 0) { // no one is looking at this checkpoint - all of it's orgs have been unFollowed - delete it!
                    checkPointTracker.erase(Global::nextGenomeWrite);
                    howManyTracking.erase(Global::nextGenomeWrite);
                }
                Global::nextGenomeWrite += Global::genomeInterval; // we are done with this interval, get ready for the next one.
            }

            if (Global::update == Global::nextDataWrite + Global::intervalDelay) {
                // now it's time to write data that was checkpointed at time Global::nextDataWrite
                string dataFileName = "data_" + to_string(Global::nextDataWrite) + ".csv";

                if (Global::files.find("data") == Global::files.end()) { // if file info has not been initialized yet
                    vector<string> tempKeysList = (*checkPointTracker[Global::nextDataWrite].begin())->checkPointDataMap[Global::nextDataWrite].getKeys(); // get all keys from an orgs dataMap (don't care which one - they should all be the same)
                    for (auto key : tempKeysList) { // for every key in dataMap...
                        if (key != "genomeAncestors") { // as long as it's not genomeAncestors... genomeAncestors may be here is a genome and data interval overlap
                            Global::files["data"].push_back(key); // add it to the list of keys associated with the genome file.
                        }
                    }
                }

                // write out data for all orgs in checkPointTracker[Global::nextGenomeWrite] to "genome_" + to_string(Global::nextGenomeWrite) + ".csv"
                for (auto org : checkPointTracker[Global::nextDataWrite]) {
                    org->checkPointDataMap[Global::nextDataWrite].writeToFile(dataFileName, Global::files["data"]); // append new data to the file
                    org->unFollow(); // tracker is no longer following this org (for it's data). If this was the only thing tracking, the org is deleted and this checkpoint will be deleted automaticly below.
                    // we could check here if anyone else is tracking this checkpoint (howManyTracking would = 1). if not, we could checkPointDataMap.erase(Global::nextGenomeWrite) and maybe the genome and brain also.
                    orgTrackingCounts[org->ID]--; // one less tracker looking at this org
                }
                howManyTracking[Global::nextDataWrite]--; // tacker is no longer interested in the data at this checkpoint
                if (howManyTracking[Global::nextDataWrite] == 0) { // no one is looking at this checkpoint - all of it's orgs have been unFollowed - delete it!
                    checkPointTracker.erase(Global::nextDataWrite);
                    howManyTracking.erase(Global::nextDataWrite);
                }
                Global::nextDataWrite += Global::dataInterval; // we are done with this interval, get ready for the next one.
            }
        }
    }
};

#endif /* defined(__BasicMarkovBrainTemplate__Gate__) */
