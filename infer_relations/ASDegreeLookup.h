#ifndef ASDEGREELOOKUP_H_INCLUDED
#define ASDEGREELOOKUP_H_INCLUDED

//INCLUDES
#include "ASCommon.h"

//CONSTANTS
#define LOOKUP_TABLE_SIZE   64000
#define INVALID_DISTANCE    -1
#define ZERO_DISTANCE    -2
#define ZERO_DEGREE      0


//FUNCTION DECLARATIONS
/**
* Inserts a node into a hash table else returns existing node
* @param nodeX NodeX to be inserted
* @param nodeY NodeY to be inserted
* @param relOrigin which path the relationship originated from
* @return The ASNNode
**/
ASNode* insertASNode(ASNode** ASNHashTable, ASNodeId nodeX, int degree, int parent_distance);

/**
* Retrieves an ASNNode from the hash table
* @param nodeX NodeX part of key
* @param nodeY NodeY part of key
**/
ASNode* getASNode(ASNode** ASNHashTable, ASNodeId nodeX);

/**
* Frees all nodes in the hash table and resets it
* @param hashTable Hash table
**/
void destroyNodeTable(ASNode** hashTable);

/**
* Populates the lookup table with degree and distance info
* @param lookupTable Hash table
**/
void populateLookupTable(ASNode** lookupTable);

/**
* Write contents of lookup table to a file
* @param lookupTable Hash table
**/
void writeNodeInfoToFile(ASNode** lookupTable);

#endif // ASDEGREELOOKUP_H_INCLUDED
