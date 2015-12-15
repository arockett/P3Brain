//
//  Genome.h
//  BasicMarkovBrainTemplate
//
//  Created by Arend Hintze on 5/29/15.
//  Copyright (c) 2015 Arend Hintze. All rights reserved.
//

#ifndef __BasicMarkovBrainTemplate__Genome__
#define __BasicMarkovBrainTemplate__Genome__

#include <stdlib.h>
#include <vector>

#include "../Utilities/Utilities.h"
#include "../Utilities/Data.h"
#include "../Utilities/Parameters.h"

using namespace std;
class Genome {
 public:
  vector<map<int, int>> codingRegions;

  static const int START_CODE = 0;
  static const int IN_COUNT_CODE = 10;
  static const int IN_ADDRESS_CODE = 11;
  static const int OUT_COUNT_CODE = 20;
  static const int OUT_ADDRESS_CODE = 21;
  static const int DATA_CODE = 30;

  Genome() = default;
  virtual ~Genome() = default;

  // randomize this genomes contents
  // the undefined action is to take no action
  virtual void fillRandom(){
  }

  // assign a value at a given location
  // the undefined action is to take no action
  virtual void assignValue(int loc, int value){
  }


  // Copy functions

  // copy the contents of another genome to this genome
  // no undefined action, this function must be defined
  virtual void copyGenome(shared_ptr<Genome> from) {
  }

  // make a mutated genome. from this genome
  // the undefined action is to return a new genome
  virtual shared_ptr<Genome> makeMutatedGenome() {
    shared_ptr<Genome> genome;
    return genome;
  }

  // make a mutated genome. from a vector or genomes
  // the undefined action is to return a new genome
  virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<Genome>> from) {
    shared_ptr<Genome> genome;
    return genome;
  }


  // Mutation functions

  // apply mutations to this genome
  // the undefined action is to take no action
  virtual void mutate() {
  }

  // IO and Data Management functions

  // gets data about genome which can be added to a data map
  // data is in pairs of strings (key, value)
  // the undefined action is to return an empty vector
  virtual vector<string> getStats() {
    vector<string> data;
    return data;
  }
  // converts the sites vector to a FileManager::separator separated list in string format.
  // the undefined action is to return an empty string
  virtual string convert_to_string() {
    string data;
    return data;
  }

  // load a genome from CSV file with headers - will return genome from saved organism with key / keyvalue pair
  // the undefined action is to take no action
  virtual void loadGenome(string fileName, string key, string keyValue) {
  }

  // Translation functions - convert genomes into usefull stuff

  // find the next start codon
  virtual int getSize() {
    return 0;
  }

  // advance an index by 1 site, or distance sites if distance is given (site is determined by the genome)
  // the undefined action is to take no action
  virtual void advanceIndex(int &index, int distance = 1) {
  }

  // advance an index by the size of that genomes start codon
  // the undefined action is to take no action
  virtual void advanceIndexPastStartCodon(int &index) {
  }

  // convert genome sites at location index into a value in range [min,max] and advance index to the next unused site
  // any sites used will be assigned code in codingRegions
  // no undefined action, this function must be defined
  virtual int extractValue(int &index, vector<int> valueRange, int code = -1) = 0;

  // convert genome sites at location index into a table and advance index to next unused site
  // no undefined action, this function must be defined
  virtual vector<vector<int>> extractTable(int &index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange) = 0;

  // Coding Region functions

  // start new coding region (new gate)
  virtual inline void newCodingRegion() {
    codingRegions.push_back( { });
  }

  // add to the last coding region
  virtual inline void assignCodingRegionValue(int index, int code) {
    if (code != -1) {
      //cout << "assigning: [" << (int) codingRegions.size() - 1 << "][" << index << "] = " << code << "\n";
      if (codingRegions.size() == 0) {
        newCodingRegion();
      }
      codingRegions[(int) codingRegions.size() - 1][index] = code;
    }
  }

  // display the getCodingRegions for a gate as (Index1,Type1,Index2,Type2...)
  virtual inline string showCodingRegions() {
    string S = "";
    for (auto r : codingRegions) {
      for (auto site : r) {
        S = S + to_string(site.first) + ":" + to_string(site.second) + "  ";
      }
      S += "\n";
    }
    return S;
  }

};

class ClassicGenome : public Genome {
 private:

 public:
  vector<unsigned char> sites;  // move to private ASAP

  static int& initialGenomeSize;
  static int& minGenomeSize;
  static int& maxGenomeSize;
  static double& pointMutationRate;
  static double& insertionRate;
  static double& deletionRate;

  ClassicGenome() = default;
  ClassicGenome(shared_ptr<ClassicGenome> from);
  virtual ~ClassicGenome() = default;

  virtual void fillRandom(int size = ClassicGenome::initialGenomeSize);
  virtual void assignValue(int loc, int value){
    sites[loc % (int) sites.size()] = value;
  }

  // copy functions
  virtual void copyGenome(shared_ptr<ClassicGenome> from);

  virtual shared_ptr<Genome> makeMutatedGenome();
  virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<ClassicGenome>> from);

  // IO functions
  virtual void loadGenome(string fileName, string key, string keyValue);

  virtual string convert_to_string();
  virtual vector<string> getStats();

  // mutation functions
  virtual void mutate();
  virtual void applyMutations(double pointMutationRate = pointMutationRate, double insertionRate = insertionRate, double deletionRate = deletionRate, int minGenomeSize = minGenomeSize, int maxGenomeSize = maxGenomeSize);
  virtual void makePointMutation();

  virtual inline int getSize() {
    return sites.size();
  }

  // advance an index by 1 site, or distance sites if distance is given
  // this should only be used if you are not worried about the actualy size of the chunk being skipped
  // if size is important, use extractValue and ignore the return value
  virtual void advanceIndex(int& genomeIndex, int distance = 1) {
    for (int i = 0; i < distance; i++) {
      genomeIndex = (genomeIndex + distance) % (int) sites.size();
    }
  }

  virtual void advanceIndexPastStartCodon(int &index) {
    newCodingRegion();
    assignCodingRegionValue(index, Genome::START_CODE);
    advanceIndex(index);  // classic genomes use two sites for their start codons
    assignCodingRegionValue(index, Genome::START_CODE);
    advanceIndex(index);  // classic genomes use two sites for their start codons
  }

  virtual int extractValue(int &index, vector<int> valueRange, int code = -1) {
    if (valueRange[0] > valueRange[1]) {
      int temp = valueRange[0];
      valueRange[1] = valueRange[0];
      valueRange[0] = temp;
    }
    int currentMax = 256;
    int genomeValue = (int) sites[index];
    assignCodingRegionValue(index, code);
    advanceIndex(index);
    while ((valueRange[1] - valueRange[0] + 1) > currentMax) {
      genomeValue = (genomeValue << 8) + (int) sites[index];
      assignCodingRegionValue(index, code);
      advanceIndex(index);
      currentMax += 256;
    }
    return (genomeValue % (valueRange[1] - valueRange[0] + 1)) + valueRange[0];
  }

// convert genome sites at location index into a table and advance index to next unused site
// no undefined action, this function must be defined

// build a table of size range[0],range[1] which is the upper left subset of a table rangeMax[0],rangeMax[1]
// the table rangeMax[0],rangeMax[0] would be filled with values from the genome (filling each row before going to the next column
// This table is assigned to the gates table field.
// set codingRegions for each used genome site value = DATA_CODE; (may need to add more support for this later!)

  virtual vector<vector<int>> extractTable(int &index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange) {
    vector<vector<int>> table;
    int x = 0;
    int y = 0;
    int Y = tableSize[0];
    int X = tableSize[1];
    int maxY = tableMaxSize[0];
    int maxX = tableMaxSize[1];

    table.resize(Y);  // set the number of rows in the table

    for (; y < (Y); y++) {
      table[y].resize(X);  // set the number of columns in this row
      for (x = 0; x < X; x++) {
        //table[y][x] = (Type) (sites[index]);
        table[y][x] = extractValue(index, valueRange, Genome::DATA_CODE);
      }
      for (; x < maxX; x++) {
        extractValue(index, valueRange, -1);
        ;  // advance genomeIndex to account for unused entries in the max sized table for this row
      }
    }
    for (; y < (maxY); y++) {
      for (x = 0; x < maxX; x++) {
        extractValue(index, valueRange, -1);  // advance to accound for unused rows
      }
    }
    return table;
  }

};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
