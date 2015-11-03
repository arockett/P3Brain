#pragma once
/******************************************************************************
* file: Tools.h
*
*   Contains useful functions needed by the P3Brain for manipulating bits
*
* author: Aaron Beckett
* date: 10/30/2015
******************************************************************************/

#include <vector>


// Define a custom assert function that prints out a message if the assert fails
#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            cerr << "Assertion '" #condition "' failed in " << __FILE__ \
                 << " line " << __LINE__ << ": " << message << endl; \
            exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { assert(condition); } while (false)
#endif


/****************************
 * Some utility functions
 ****************************/

/**
 * Treate a vector of booleans as a representation of a bit string and 
 * return the unsigned integer representation of that bit string
 */
unsigned boolStringToInt( const std::vector<bool>& boolStr );

/**
 * Flip the bit at a given position in the number (starting with 0 as the rightmost bit)
 */
unsigned flipBit( unsigned n, int pos );

/**
 * Return the bit at a given position in the number (starting with 0 as the rightmost bit)
 */
unsigned getBit( unsigned n, int pos );
