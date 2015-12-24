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

  ////////////// class Index

  class Index {
   public:
    int site;  // index->site is what this index is looking at
    bool eog;  // set to true when end of genome is reached
    int genomeSize;  // size of genome

    Index() {
      site = 0;
      genomeSize = 0;
      eog = false;
    }

    // make a new index pointing at site (first site by default);
    Index(int _genomeSize, int _site = 0) {
      site = _site;
      genomeSize = _genomeSize;
      eog = false;
    }

    // advance index by some number of sites.
    // if end of genome is reached, set eog = true and loop.
    void advance(int by = 1) {
      site = site + by;
      if (site >= genomeSize) {
        eog = true;
        site = site % genomeSize;
      }
    }

    // did we get to (or pass) the end of the genome?
    bool atEnd() {
      return eog;
    }
  };
  ////////////// end of class Index

  shared_ptr<Index> getIndex(int _site = 0) {
    return make_shared<Index>(getSize(), _site);
  }

  void copyIndex(shared_ptr<Index> from,shared_ptr<Index> to) {
    to->site = from->site;
    to->genomeSize = from->genomeSize;
    to->eog = from->eog;
  }

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
  virtual void fillRandom() {
  }

// assign a value at a given location
// the undefined action is to take no action
  virtual void assignValue(Index index, int value) {
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

// return total number of sites in this genome
  virtual int getSize() {
    return 0;
  }

// convert genome sites at location index into a value in range [min,max] and advance index to the next unused site
// any sites used will be assigned code in codingRegions
// no undefined action, this function must be defined
  virtual int extractValue(shared_ptr<Index> index, vector<int> valueRange, int code = -1) = 0;

// convert genome sites at location index into a table and advance index to next unused site
  virtual vector<vector<int>> extractTable(shared_ptr<Index> index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = DATA_CODE) {
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
        table[y][x] = extractValue(index, valueRange, code);
      }
      for (; x < maxX; x++) {
        extractValue(index, valueRange, -1);
        ;  // advance genomeIndex to account for unused entries in the max sized table for this row
      }
    }
    for (; y < (maxY); y++) {
      for (x = 0; x < maxX; x++) {
        extractValue(index, valueRange, -1);  // advance to account for unused rows
      }
    }
    return table;
  }
// Coding Region functions

// start new coding region (new gate)
  virtual inline void newCodingRegion() {
    codingRegions.push_back( { });
  }

// add to the last coding region
  virtual inline void assignCodingRegionValue(int indexKey, int code) {
    if (code != -1) {
      //cout << "assigning: [" << (int) codingRegions.size() - 1 << "][" << index << "] = " << code << "\n";
      if (codingRegions.size() == 0) {
        newCodingRegion();
      }
      codingRegions[(int) codingRegions.size() - 1][indexKey] = code;
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

}
;

class ByteGenome : public Genome {
 private:

 public:

  vector<unsigned char> sites;  // move to private ASAP

  static int& initialGenomeSize;
  static int& minGenomeSize;
  static int& maxGenomeSize;
  static double& pointMutationRate;
  static double& insertionRate;
  static double& deletionRate;

  ByteGenome() = default;
  ByteGenome(shared_ptr<ByteGenome> from);
  virtual ~ByteGenome() = default;

  virtual void fillRandom(int size = ByteGenome::initialGenomeSize);
  virtual void assignValue(int loc, int value) {
    sites[loc % (int) sites.size()] = value;
  }

  // copy functions
  virtual void copyGenome(shared_ptr<ByteGenome> from);

  virtual shared_ptr<Genome> makeMutatedGenome();
  virtual shared_ptr<Genome> makeMutatedGenome(vector<shared_ptr<ByteGenome>> from);

  // IO functions
  virtual void loadGenome(string fileName, string key, string keyValue);

  virtual string convert_to_string();
  virtual vector<string> getStats();
  virtual inline int getSize() {
    return sites.size();
  }

  // mutation functions
  virtual void mutate();
  virtual void applyMutations(double _pointMutationRate = pointMutationRate, double _insertionRate = insertionRate, double _deletionRate = deletionRate, int _minGenomeSize = minGenomeSize, int _maxGenomeSize = maxGenomeSize);
  virtual void makePointMutation();

  virtual int extractValue(shared_ptr<Index> index, vector<int> valueRange, int code = -1) {
    if (valueRange[0] > valueRange[1]) {
      int temp = valueRange[0];
      valueRange[1] = valueRange[0];
      valueRange[0] = temp;
    }
    int currentMax = 256;
    int genomeValue = (int) sites[index->site];
    assignCodingRegionValue(index->site, code);
    index->advance();
    while ((valueRange[1] - valueRange[0] + 1) > currentMax) {  // we don't have enough bits of information
      genomeValue = (genomeValue << 8) & (int) sites[index->site];  // get 8 more bits
      assignCodingRegionValue(index->site, code);
      index->advance();
      currentMax = currentMax * 256;
    }
    return (genomeValue % (valueRange[1] - valueRange[0] + 1)) + valueRange[0];
  }

// convert genome sites at location index into a table and advance index to next unused site
// no undefined action, this function must be defined

// build a table of size range[0],range[1] which is the upper left subset of a table rangeMax[0],rangeMax[1]
// the table rangeMax[0],rangeMax[0] would be filled with values from the genome (filling each row before going to the next column
// This table is assigned to the gates table field.
// set codingRegions for each used genome site value = DATA_CODE; (may need to add more support for this later!)

//  virtual vector<vector<int>> extractTable(int &index, vector<int> tableSize, vector<int> tableMaxSize, vector<int> valueRange, int code = DATA_CODE) {
//    vector<vector<int>> table;
//    int x = 0;
//    int y = 0;
//    int Y = tableSize[0];
//    int X = tableSize[1];
//    int maxY = tableMaxSize[0];
//    int maxX = tableMaxSize[1];
//
//    table.resize(Y);  // set the number of rows in the table
//
//    for (; y < (Y); y++) {
//      table[y].resize(X);  // set the number of columns in this row
//      for (x = 0; x < X; x++) {
//        //table[y][x] = (Type) (sites[index]);
//        table[y][x] = extractValue(index, valueRange, code);
//      }
//      for (; x < maxX; x++) {
//        extractValue(index, valueRange, -1);
//        ;  // advance genomeIndex to account for unused entries in the max sized table for this row
//      }
//    }
//    for (; y < (maxY); y++) {
//      for (x = 0; x < maxX; x++) {
//        extractValue(index, valueRange, -1);  // advance to accound for unused rows
//      }
//    }
//    return table;
//  }

};

#endif /* defined(__BasicMarkovBrainTemplate__Genome__) */
