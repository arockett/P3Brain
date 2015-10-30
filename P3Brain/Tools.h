#pragma once
/******************************************************************************
* file: Tools.h
*
*   Contains useful functions needed by the P3Brain for manipulating bits
*
* author: Aaron Beckett
* date: 10/30/2015
******************************************************************************/


/****************************
 * Some utility functions
 ****************************/

/**
 * Treate a vector of booleans as a representation of a bit string and 
 * return the unsigned integer representation of that bit string
 */
unsigned boolStringToInt( const vector<bool>& boolStr )
{
    unsigned value = 0;
    for( auto place : boolStr )
    {
        value <<= 1;
        if( place ) value |= 1;
    }
    return value;
}

/**
 * Flip the bit at a given position in the number (starting with 0 as the rightmost bit)
 */
unsigned flipBit( unsigned n, int pos )
{
    unsigned mask = 1 << pos;
    return n ^ mask;
}

/**
 * Return the bit at a given position in the number (starting with 0 as the rightmost bit)
 */
unsigned getBit( unsigned n, int pos )
{
    return ( n & (unsigned)pow(2.0, pos) ) >> pos;
}
